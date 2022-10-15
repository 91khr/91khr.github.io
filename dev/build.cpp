// vim: fdm=marker
// {{{ Premable
#include "helpers.hpp"

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iterator>
#include <lua.hpp>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#if __cplusplus >= 202002L
#include <concepts>
#endif  // __cplusplus
#include <optional>
using std::string_literals::operator""s;
namespace fs = std::filesystem;
// }}} End premable

// {{{ Workaround and helpers
// {{{ Workaround for compilers without implementation to hash<fs::path>
#if __cplusplus < 202002L
namespace std
{
template<>
struct hash<fs::path>
{
    size_t operator()(const fs::path &s) const noexcept { return fs::hash_value(s); }
};
}
#endif  // __cplusplus
// }}} End hash workaround

// Constants and options
namespace env
{
const char helpmsg[] = R"(
Usage: %s [OPTION]...
A site generator that read markdown source file from src/,
output compiled result and other generated information to out/.

-c, --clean    clean the out/ and index/ directory
-i, --index    (re)generate the index
-f, --force    forcely regenerate all files
-h, --help     print this message
)";
const std::string build_args = R"(--standalone --lua-filter=dev/filter.lua -t html --katex --template=dev/)";
const int max_index_size = 1926;
#ifdef _WIN32
const char devnull[] = "nul";
#else
const char devnull[] = "/dev/null";
#endif

bool is_index = false;
bool is_force = false;
}

// {{{ Logger
struct Logger
{
    enum Level : unsigned
    {
        Error,
        Output,    // Required output
        Progress,  // The main progress of a process
        Debug,     // Used for debugging
        LV_MAX_,
    };
    unsigned curlv = LV_MAX_, depth = 0;
    inline static const char *LevelMsg[] = {
        "[ERROR] ",  // Error message
        ":: ",       // Required output shouldn't be logged
        "=> ",       // Progress separator
        "[DBG] ",    // Debugging message
    };
    static_assert(sizeof(LevelMsg) / sizeof(*LevelMsg) == Level::LV_MAX_,
                  "There should be exactly one message for each of the log levels");

    void log(Level lv, const char *fmt, const auto &...args)
    {
        if (lv > curlv)
            return;
        auto target = lv == Error ? stderr : stdout;
        fprintf(target, "%s%s", std::string(depth * 2, ' ').c_str(), LevelMsg[lv]);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        fprintf(target, fmt, args...);
#pragma GCC diagnostic pop
        fflush(target);
    }

    auto withstage(Level lv, const char *fmt, const auto &...args)
    {
        log(lv, fmt, args...);
        return [this](auto f) {
            ++depth;
            f();
            --depth;
        };
    }
} logger;
// }}} End logger

namespace helper
{
// {{{ clean
void clean()
{
    const std::set<fs::path> reserves = {};
    // Walk output and remove unnessary files
    auto walkpath = [&reserves](const char *base) {
        for (auto &it : fs::recursive_directory_iterator(base))
            if (!it.is_directory() && !reserves.count(it.path()))
            {
                printf("removing %s\n", it.path().c_str());
                fs::remove(it.path());
            }
    };
    walkpath("out");
    walkpath("index");
}
// }}} end clean

// {{{ pandoc
int pandoc(const std::string &in, const std::string &out, const std::string &args,
           const std::vector<std::pair<std::string, std::string>> env)
{
    std::string cmd;
#ifdef _WIN32
    for (const auto &it : env)
        cmd += "set " + it.first + "=" + it.second + ";";
#else
    cmd = "env ";
    for (const auto &it : env)
        cmd += it.first + "=" + it.second + " ";
#endif
    cmd += "pandoc " + in + " -o " + out + " " + args;
    return system(cmd.c_str());
}
// }}} end pandoc

void ensure_path(const fs::path &path) { fs::create_directories(path); }
}
// }}} End workaround and helpers

// {{{ Workers
struct DirHandler
{
    // The generated HTML files of the articles and catalogs of the subdir,
    // where 'catalogs' refers to the `index.html` file.
    // The entries should be sorted by descending order
    struct SubdirInfo
    {
        struct CatInfo
        {
            fs::path index;
            fs::file_time_type time;
            friend bool operator<(const CatInfo &a, const CatInfo &b) { return a.time > b.time; }
        };
        std::vector<fs::path> articles = {};
        std::vector<CatInfo> catalogs = {};
        fs::file_time_type last_mod = {};
    };
    struct FilterInfo
    {
        std::vector<fs::path> articles = {};
        std::vector<fs::path> subdirs = {};
        std::vector<fs::path> unindiced = {};
        std::string build_args = {};
    };

    decltype(helper::read_dates()) artdates = helper::read_dates();

    // {{{ Lua script operation
    lua_State *Lua = nullptr;
    void init()
    {
        Lua = luaL_newstate();
        luaL_openlibs(Lua);
        // Default arguments of pandoc
        lua_pushstring(Lua, env::build_args.c_str());
        lua_setglobal(Lua, "default_build_args");
        lua_pushcfunction(Lua, [](lua_State *Lua) -> int {
            lua_newtable(Lua);
            for (auto f : fs::directory_iterator(lua_tostring(Lua, 1)))
            {
                lua_createtable(Lua, 0, 2);
                lua_pushstring(Lua, f.path().c_str());
                lua_setfield(Lua, -2, "path");
                lua_pushstring(Lua, f.is_directory() ? "dir" : "file");
                lua_setfield(Lua, -2, "type");
                lua_seti(Lua, -2, luaL_len(Lua, -2) + 1);
            }
            return 1;
        });
        lua_setglobal(Lua, "list_dir");
    }
    ~DirHandler() { lua_close(Lua); }
    // }}} End lua

    // {{{ filter_dir: Filter out files, subdirs, and other info of the directory
    FilterInfo filter_dir(fs::path dir)
    {
        if (auto luafilter = dir / "filter.lua"; fs::exists(luafilter))
        {
            logger.log(Logger::Progress, "Executing lua filter...\n");
            if (luaL_loadfile(Lua, luafilter.c_str()) != LUA_OK)
            {
                logger.log(Logger::Error, "Failed to load lua filter: %s\n", luaL_tolstring(Lua, -1, NULL));
                return {};
            }
            lua_pushstring(Lua, luafilter.remove_filename().c_str());
            if (lua_pcall(Lua, 1, 1, 0) != LUA_OK)
            {
                logger.log(Logger::Error, "Failed to execute lua filter: %s\n", luaL_tolstring(Lua, -1, NULL));
                return {};
            }
            auto get_content = [this]() -> std::vector<fs::path> {
                if (lua_isnil(Lua, -1))
                    return {};
                std::vector<fs::path> ctnt;
                lua_pushnil(Lua);
                while (lua_next(Lua, -2) != 0)
                {
                    ctnt.push_back(lua_tostring(Lua, -1));
                    lua_pop(Lua, 1);
                }
                return ctnt;
            };
            auto with_field = [this](auto name, auto f) {
                lua_getfield(Lua, -1, name);
                f();
                lua_pop(Lua, 1);
            };
            FilterInfo res;
            with_field("articles", [&] { res.articles = get_content(); });
            with_field("subdirs", [&] { res.subdirs = get_content(); });
            with_field("unindiced", [&] { res.unindiced = get_content(); });
            with_field("build_args", [&] {
                if (!lua_isnil(Lua, -1))
                    res.build_args = lua_tostring(Lua, -1);
                else
                    res.build_args = env::build_args;
            });
            lua_pop(Lua, 1);
            return res;
        }
        else
        {
            FilterInfo res;
            res.build_args = env::build_args;
            for (auto f : fs::directory_iterator(dir))
                if (f.is_directory())
                    res.subdirs.push_back(f);
                else if (f.path().extension() == ".md")
                    res.articles.push_back(f);
            return res;
        }
    }
    // }}} End filter_dir

    // {{{ write_index: Write index files
    template<class IterF>
#if __cplusplus >= 202002L
        // clang-format off
        requires requires(IterF f) { { f() } -> std::same_as<std::optional<fs::path>>; }
        // clang-format on
#endif  // __cplusplus
    void write_index(fs::path base, const char *prefix, IterF fn)
    {
        thread_local struct CtntBuf
        {
            char *data = nullptr;
            size_t len = 0;
            ~CtntBuf() { free(data); }
        } buf;
        size_t cursize = 0;
        int count = 0;
        helper::ensure_path("out/index" / base);
        FileIO dstf { "out/index" / base / (prefix + "."s + std::to_string(++count) + ".html"), "w" };
        for (decltype(fn()) cur_o; (cur_o = fn());)
        {
            fs::path cur = cur_o.value();
            auto fname = "index" / cur.lexically_relative("src").replace_extension(".html");
            FileIO src { fname, "r" };
            size_t incsize = getdelim(&buf.data, &buf.len, EOF, src.handle);
            if (cursize + incsize > env::max_index_size)
            {
                dstf.reopen("out/index" / base / (prefix + "."s + std::to_string(++count) + ".html"), "w");
                cursize = 0;
            }
            cursize += incsize;
            dstf.printf("%s\n", buf.data);
        }
        dstf.reopen("out/index" / base / (prefix + ".count.txt"s), "w");
        dstf.printf("%d", count);
    }
    // }}} End write_index

    SubdirInfo walk_dir(fs::path dir)
    {
        SubdirInfo res;
        auto srcrelpath = dir.lexically_relative("src");
        auto art_comp = [&](auto a, auto b) { return artdates[a] > artdates[b]; };
        logger.withstage(Logger::Output, "Process %s\n", dir.c_str())([&] {
            FilterInfo info = filter_dir(dir);
            // {{{2 Compile articles
            {
                helper::ensure_path("out" / srcrelpath);
                helper::ensure_path("index" / srcrelpath);
                bool has_index = false;
                for (auto art : info.articles)
                {
                    bool is_index = art.filename() == "index.md";
                    has_index = std::max(has_index, is_index);
                    auto outpath = "out" / art.lexically_relative("src").replace_extension(".html");
                    if (env::is_force || !fs::exists(outpath) ||
                        fs::last_write_time(art) > fs::last_write_time(outpath))
                    {
                        logger.log(Logger::Progress, "Compile %s\n", art.c_str());
                        res.last_mod = std::max(res.last_mod, fs::last_write_time(art));
                        helper::pandoc(art.string(), outpath.string(),
                                       info.build_args + (is_index ? "index_template.html" : "template.html"),
                                       { { "basePath", srcrelpath / outpath.filename() } });
                    }
                }
                // Generate alternative index
                if (auto index_out = "out" / srcrelpath / "index.html"; env::is_index && !has_index)
                {
                    logger.log(Logger::Progress, "Generate alternative index\n");
                    helper::pandoc(env::devnull, index_out, info.build_args + "index_template.html -f markdown",
                                   { { "basePath", srcrelpath / "index.html" }, { "alterIndex", "1" } });
                }
            }
            // {{{2 Filter out unindiced files, set result articles
            {
                std::sort(info.articles.begin(), info.articles.end(), art_comp);
                std::sort(info.unindiced.begin(), info.unindiced.end(), art_comp);
                auto elem = info.articles.begin(), front = elem;
                auto filter = info.unindiced.begin();
                for (; front != info.articles.end(); ++front)
                {
                    while (filter != info.unindiced.end() && *filter < *front)
                        ++filter;
                    if (front->filename() != "index.md" &&
                        (filter == info.unindiced.end() || !fs::equivalent(*filter, *front)))
                        *elem++ = *front;
                }
                info.articles.erase(elem, info.articles.end());
                res.articles = std::move(info.articles);
            }
            // {{{2 Process subdirs
            for (auto subdir : info.subdirs)
            {
                auto sub = walk_dir(subdir);
                auto origsize = res.articles.size();
                res.articles.reserve(res.articles.size() + sub.articles.size());
                std::move(sub.articles.begin(), sub.articles.end(), std::back_inserter(res.articles));
                std::inplace_merge(res.articles.begin(), res.articles.begin() + origsize, res.articles.end(), art_comp);
                origsize = res.catalogs.size();
                res.catalogs.reserve(res.catalogs.size() + sub.catalogs.size());
                std::move(sub.catalogs.begin(), sub.catalogs.end(), std::back_inserter(res.catalogs));
                std::inplace_merge(res.catalogs.begin(), res.catalogs.begin() + origsize, res.catalogs.end(),
                                   [&](auto a, auto b) { return artdates[a.index] > artdates[b.index]; });
            }
            // {{{2 Write indices
            if (env::is_index)
            {
                logger.log(Logger::Progress, "Writing indices\n");
                auto artit = res.articles.begin();
                write_index(srcrelpath, "index",
                            [&] { return artit == res.articles.end() ? std::nullopt : std::optional(*artit++); });
                logger.log(Logger::Progress, "Writing catalogs\n");
                auto catit = res.catalogs.begin();
                write_index(srcrelpath, "catalog",
                            [&] { return catit == res.catalogs.end() ? std::nullopt : std::optional(catit++->index); });
            }
            // {{{2 Insert current index
            if (env::is_index)
            {
                std::error_code ec;
                SubdirInfo::CatInfo curindex = { dir / "index.md", fs::last_write_time(dir / "index.md", ec) };
                bool inserted = false;
                for (auto ins = res.catalogs.begin(); ins != res.catalogs.end(); ++ins)
                    if (curindex < *ins)
                    {
                        res.catalogs.insert(ins, curindex);
                        inserted = true;
                        break;
                    }
                if (!inserted)
                    res.catalogs.push_back(curindex);
            }
            // }}}2 End processing
        });
        return res;
    }
};
// }}} End workers

// {{{ main
int main(int, char **argv)
{
    // {{{ Process arguments
    {
        const auto help = [argv] { printf(env::helpmsg, argv[0]); };
        const auto index = [] { env::is_index = true; };
        const auto force = [] { env::is_force = true; };
        struct FnInfo
        {
            std::function<void()> fn;
            bool isquit;
        };
        std::unordered_map<char, FnInfo> shortOpt = {
            { 'c', { helper::clean, true } },
            { 'h', { help, true } },
            { 'i', { index, false } },
            { 'f', { force, false } },
        };
        std::unordered_map<std::string, FnInfo> longOpt = {
            { "clean", { helper::clean, true } },
            { "help", { help, true } },
            { "index", { index, false } },
            { "force", { force, false } },
        };
        for (++argv; *argv != nullptr; ++argv)
            if (const char *arg = *argv; arg[0] == '-')
            {
                // Long format
                if (arg[1] == '-')
                {
                    if (auto it = longOpt.find(arg + 2); it != longOpt.end())
                    {
                        it->second.fn();
                        if (it->second.isquit)
                            return 0;
                    }
                    else
                    {
                        printf("Error: unrecognized argument: %s\n", arg);
                        help();
                        return 1;
                    }
                }
                // Short format
                else
                    for (const char *ch = arg + 1; *ch; ++ch)
                        if (auto it = shortOpt.find(*ch); it != shortOpt.end())
                        {
                            it->second.fn();
                            if (it->second.isquit)
                                return 0;
                        }
                        else
                        {
                            printf("Error: unrecognized option %c in %s\n", *ch, arg);
                            help();
                            return 1;
                        }
            }
    }
    // }}} End processing arguments

    DirHandler dir;
    dir.init();
    dir.walk_dir("src");
}
// }}} End main
