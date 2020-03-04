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
using std::string_literals::operator""s;
namespace fs = std::filesystem;

struct FileIO
{
    FILE *handle;
    FileIO() : handle(nullptr) {}
    FileIO(const std::string &fname, const char *type) : handle(fopen(fname.c_str(), type)) {}
    ~FileIO() { if (handle) fclose(handle); }
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

// Constants and options
std::string build_args = R"(--template=template.html -s --lua-filter=filter.lua -t html --katex)";
const int index_size = 1926;
const auto ignored_files = ([] () -> std::set<fs::path> {
    std::vector<fs::path> res = {
        "parallelized_trans",
    };
    for (auto &i : res)
        i = ("src" / i).replace_extension(".md");
    return std::set(res.begin(), res.end());
})();

int pandoc(
        const std::string &args,
        const std::vector<std::pair<std::string, std::string>> env);
void clean();
void generate_index(std::vector<fs::path> &&indices);

int main(int argc, char **argv)
{
    // cd to base path
    fs::current_path(fs::path(argv[0]).remove_filename());
    // clean if needed
    if (argc > 1 && argv[1] == "clean"s)
    {
        clean();
        return 0;
    }

    // walk the source and build them
    std::vector<fs::path> indices(
            fs::recursive_directory_iterator("src"),
            fs::recursive_directory_iterator());
    indices.erase(
            std::remove_if(indices.begin(), indices.end(),
                [] (const auto &p) { return p.extension() != ".md" || ignored_files.count(p); }),
            indices.end());
    for (const auto &nowpath : indices)
    {
        auto basepath = nowpath.lexically_relative("src");
        auto outpath = ("out" / basepath).replace_extension(".html");
        if (!fs::exists(outpath) ||
                fs::last_write_time(nowpath) > fs::last_write_time(outpath))
        {
            fs::create_directories(fs::path(outpath).remove_filename());
            printf("building %s\n", nowpath.c_str());
            pandoc(nowpath.string() + " -o " + outpath.string() + " " + build_args,
                    { { "basePath", basepath.string() } });
        }
    }

    if (argc > 1 && argv[1] == "-i"s)
    {
        printf("generating index...\n");
        generate_index(std::move(indices));
    }

    // Regenerate all indices

    return 0;
}

void clean()
{
    const std::set<fs::path> reserves = {
        "out/index.html",
        "out/index.js",
        "out/index.css",
        "out/highlight.css",
    };
    // Walk output and remove unnessary files
    auto walkpath = [&reserves] (const char *base) {
        for (auto &it : fs::recursive_directory_iterator(base))
            if (!it.is_directory() && !reserves.count(it.path()))
                fs::remove(it.path());
    };
    walkpath("out");
    walkpath("index");
}

void generate_index(std::vector<fs::path> &&indices)
{
    std::sort(indices.begin(), indices.end(),
            [] (const auto &a, const auto &b) {
                return fs::last_write_time(a) < fs::last_write_time(b);
            });
    for (auto &it : indices)
        it = "index" / it.lexically_relative("src");
    size_t idxcount = 1;
    std::string content;
    FileIO nowfile("index/index1.md", "w");
    for (auto &it : indices)
    {
        FileIO in(it.c_str(), "r");
        char *buf = nullptr;
        size_t len = 0;
        size_t nowsize = getdelim(&buf, &len, EOF, in.handle);
        if (content.size() + nowsize >= index_size)
        {
            FileIO("index/index" + std::to_string(idxcount) + ".md", "w").
                printf("%s\n", content.c_str());
            pandoc("index/index" + std::to_string(idxcount) + ".md" +
                    " -o out/index/" + std::to_string(idxcount) + ".html", {});
            ++idxcount;
            content.clear();
        }
        content = buf + "\n"s + content;
        free(buf);
    }
    FileIO("index/index" + std::to_string(idxcount) + ".md", "w").
        printf("%s\n", content.c_str());
    pandoc("index/index" + std::to_string(idxcount) + ".md" +
            " -o out/index/" + std::to_string(idxcount) + ".html", {});
    FileIO("out/index.count.txt", "w").printf("%d", idxcount);
}

int pandoc(
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
    cmd += "pandoc " + args;
    return system(cmd.c_str());
}

