#ifndef HELPERS_HPP_INCLUDED
#define HELPERS_HPP_INCLUDED

#include <cstdio>
#include <cstdarg>
#include <string>
#include <unordered_map>
#include <chrono>
namespace chrono = std::chrono;

struct FileIO
{
    FILE *handle;
    FileIO() : handle(nullptr) {}
    FileIO(FileIO &) = delete;
    FileIO(FileIO &&) = delete;
    FileIO(FILE *f) : handle(f) {}
    FileIO(const std::string &fname, const char *type) : handle(fopen(fname.c_str(), type)) {}
    ~FileIO() { close(); }
    void close()
    {
        if (handle)
            fclose(handle);
        handle = nullptr;
    }
    void reopen(const std::string &fname, const char *type)
    {
        if (handle)
            fclose(handle);
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
    std::string getline()
    {
        thread_local struct LineBuf {
            char *data = nullptr;
            size_t len = 0;
            ~LineBuf() { free(data); }
        } buf;
        ::getline(&buf.data, &buf.len, handle);
        std::string res(buf.data);
        res.pop_back();  // Trim the trailing '\n'
        return res;
    }
    bool eof() const { return feof(handle); }
};

namespace helper
{
std::unordered_map<std::string, chrono::system_clock::time_point> read_dates()
{
    std::unordered_map<std::string, chrono::system_clock::time_point> res;
    auto f = FileIO("timestamp.txt", "r");
    while (!f.eof())
    {
        long long time;
        fscanf(f.handle, "%llu ", &time);
        res[f.getline()] = chrono::system_clock::time_point(chrono::seconds(time));
    }
    return res;
}

}  // End namespace helper

#endif  // HELPERS_HPP_INCLUDED
