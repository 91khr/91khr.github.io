#if 0  // Self compile
g++ -std=c++2a $0 -o build -lstdc++fs -fconcepts -g
exit $?
#endif
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstdarg>
#include <filesystem>
#include <vector>
#include <set>
#include <unordered_map>
#include <functional>
using std::string_literals::operator""s;
namespace fs = std::filesystem;

// Implement hash for fs::path
namespace std
{
template<>
struct hash<fs::path>
{
    size_t operator()(const fs::path &s) const noexcept
    {
        return fs::hash_value(s);
    }
};
}

struct FileIO
{
    FILE *handle;
    FileIO() : handle(nullptr) {}
    FileIO(FileIO&) = delete;
    FileIO(FileIO&&) = delete;
    FileIO(const std::string &fname, const char *type) : handle(fopen(fname.c_str(), type)) {}
    ~FileIO() { close(); }
    void close() { if (handle) fclose(handle); handle = nullptr; }
    void reopen(const std::string &fname, const char *type)
    {
        if (handle) fclose(handle);
        handle = fopen(fname.c_str(), type);
    }
    int printf(const char *fmt, ...)
    {
        va_list arg;
        va_start(arg, fmt);
        return vfprintf(handle, fmt, arg);
    }
    int scanf(const char *fmt, ...)
    {
        va_list arg;
        va_start(arg, fmt);
        return vfscanf(handle, fmt, arg);
    }
};

struct ProcFileHandle
{
    enum class IndexType {
        article, index,
    };
    void operator()(const fs::path &inpath);
    void write_index(const fs::path &basepath, IndexType type);
    void init();
    void finish();
    struct IndexInfo
    {
        int index_size = 0, index_count = 0;
        FileIO out;
    };
    std::unordered_map<fs::path, IndexInfo> indices;
    std::unordered_map<fs::path, IndexInfo> catalogs;
} proc_file;

// Constants and options
namespace env {
const char helpmsg[] = R"(
Usage: %s [OPTION]...
A site generator that read markdown source file from src/,
output compiled result and other generated information to out/.

-c, --clean    clean the out/ and index/ directory
-i, --index    (re)generate the index
-f, --force    forcely regenerate all files
-h, --help     print this message
)";
const std::string build_args = R"(--toc --standalone --lua-filter=filter.lua -t html --katex --template=etc/)";
const int max_index_size = 1926;
const auto ignored_files = ([] () -> std::set<fs::path> {
    std::vector<fs::path> res = {
        "parallelized_trans",
        "tech/quick_vim_tutor",
    };
    for (auto &i : res)
        i = ("src" / i).replace_extension(".md");
    return std::set(res.begin(), res.end());
})();
const auto unindexed_files = ([] () -> std::set<fs::path> {
    std::vector<fs::path> res = {
        "friends",
        "test",
    };
    for (auto &i : res)
        i = ("src" / i).replace_extension(".md");
    return std::set(res.begin(), res.end());
})();

bool is_index = false;
bool is_force = false;
}

namespace helper {
void clean();
}

int main(int argc, char **argv)
{
    // cd to base path
    fs::current_path(fs::path(argv[0]).remove_filename());
    // Process arguments
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
                            printf("Error: unrecognized option %c in %s\n", *ch, *arg);
                            help();
                            return 1;
                        }
            }
    }

    // walk the source and build them
    std::vector<fs::path> indices(
            fs::recursive_directory_iterator("src"),
            fs::recursive_directory_iterator());
    indices.erase(
            std::remove_if(indices.begin(), indices.end(),
                [] (const auto &p) { return p.extension() != ".md" || env::ignored_files.count(p); }),
            indices.end());
    std::sort(indices.begin(), indices.end(),
            [] (const auto &a, const auto &b) {
                return fs::last_write_time(a) > fs::last_write_time(b);
            });
    proc_file.init();
    for (const auto &nowpath : indices)
        proc_file(nowpath);
    proc_file.finish();

    return 0;
}

namespace helper {
void clean()
{
    const std::set<fs::path> reserves = {
    };
    // Walk output and remove unnessary files
    auto walkpath = [&reserves] (const char *base) {
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

int pandoc(
        const std::string &in,
        const std::string &out,
        const std::string &args,
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

void ensure_path(const fs::path &path) { fs::create_directories(path); }
}

void ProcFileHandle::operator()(const fs::path &inpath)
{
    bool logged = false;
    fs::path basepath = inpath.lexically_relative("src").replace_extension(".html");
    fs::path outpath = "out" / basepath;
    bool is_index = inpath.filename() == "index.md";
    // Make directory for output & index
    helper::ensure_path(fs::path(outpath).remove_filename());
    helper::ensure_path(fs::path("index" / basepath).remove_filename());
    // Build file itself
    if (env::is_force || !fs::exists(outpath) ||
            fs::last_write_time(inpath) > fs::last_write_time(outpath))
    {
        printf("Building %s\n", inpath.c_str());
        logged = true;
        helper::pandoc(inpath.string(), outpath.string(), env::build_args +
                (is_index ? "index_template.html" : "template.html"),
                { { "basePath", basepath.string() } });
    }
    // Generate index
    if (env::is_index && !env::unindexed_files.count(inpath))
    {
        if (!logged)
            printf("Generating index for %s\n", inpath.c_str());
        write_index(basepath, is_index ? IndexType::index : IndexType::article);
    }
}

void ProcFileHandle::init() {}

void ProcFileHandle::write_index(const fs::path &basepath, IndexType type)
{
    const std::string prefix = type == IndexType::article ? "index" : "catalog";
    // Get index content
    char *buf = nullptr;
    size_t len = 0;
    FileIO in("index" / basepath, "r");
    size_t nowsize = getdelim(&buf, &len, EOF, in.handle);
    // Write to all timelines of parent catalogs
    auto nowpath = ("." / basepath).remove_filename().lexically_normal();
    auto get_index_path = [prefix] (int index, const fs::path &path) -> fs::path {
        return "out/index" / path / (prefix + '.' + std::to_string(index) + ".html");
    };
    auto touch_collection = [prefix] (auto &collection, const fs::path &path) -> auto & {
        auto &index = collection[path];
        auto outpath = "out/index" / path;
        helper::ensure_path(outpath);
        index.out.reopen(outpath / (prefix + ".1.html"), "w");
        index.index_count = 1;
        return index;
    };
    if (type == IndexType::index)
    {
        // Touch the catalog in order not to have no catalog content
        if (!fs::exists(get_index_path(1, nowpath)))
            touch_collection(catalogs, nowpath);
        nowpath = (nowpath / "..").lexically_normal();
    }
    for (; nowpath != ".."; nowpath = (nowpath / "..").lexically_normal())
    {
        // Get/create index handler according to the directory
        IndexInfo &nowindex = ([nowpath, prefix, touch_collection] (auto &collection) -> auto & {
            if (auto it = collection.find(nowpath); it != collection.end())
                return it->second;
            else
                return touch_collection(collection, nowpath);
        })(type == IndexType::article ? indices : catalogs);
        // Accumulate size of index file
        if (nowsize + nowindex.index_size > env::max_index_size)
        {
            nowindex.index_size = 0;
            ++nowindex.index_count;
            nowindex.out.reopen(get_index_path(nowindex.index_count, nowpath), "w");
        }
        else
            nowindex.index_size += nowsize;
        // Write actual content
        nowindex.out.printf("%s\n", buf);
    }
    free(buf);
}

void ProcFileHandle::finish()
{
#ifdef _WIN32
    const char devnull[] = "nul";
#else
    const char devnull[] = "/dev/null";
#endif
    if (!env::is_index)
        return;
    // Create indices
    for (const auto &it : indices)
    {
        FileIO(fs::path("out/index") / it.first / "index.count.txt", "w")
            .printf("%d\n", it.second.index_count);
        // Create index for directories without index.md
        if (auto index_file = "src" / it.first / "index.md"; !fs::exists(index_file))
        {
            printf("Creating alternative index for %s\n", it.first.c_str());
            auto index_out = "out" / it.first / "index.html";
            if (!fs::exists(index_out) || env::is_force)
                helper::pandoc(devnull, index_out,
                        env::build_args + "index_template.html -f markdown",
                        { { "basePath", it.first / "index.html" }, { "alterIndex", "1" } });
            write_index(index_out.lexically_relative("out"), IndexType::index);
        }
    }
    printf("Creating catalog...\n");
    // Create catalogs
    for (const auto &it : catalogs)
    {
        FileIO(fs::path("out/index") / it.first / "catalog.count.txt", "w")
            .printf("%d\n", it.second.index_count);
    }
}

