#include "helpers.hpp"
#include <vector>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

int main()
{
    auto dates = helper::read_dates();
    FileIO is(stdin);
    const std::chrono::system_clock::time_point epoch = std::chrono::system_clock::from_time_t(0);
    while (!feof(stdin))
    {
        scanf(" %*s ");
        if (feof(stdin))
            break;
        auto fname = is.getline();
        if (fs::exists(fname))
        {
            auto ftime = fs::last_write_time(fname);
            dates[fname] = chrono::file_clock::to_sys(ftime);
        }
        else
            dates.erase(fname);
    }
    std::vector<std::pair<long long, std::string>> sorted;
    for (auto [name, time]: dates)
    {
        auto stamp = chrono::duration_cast<chrono::seconds>(time - epoch).count();
        sorted.push_back({ stamp, name });
    }
    std::sort(sorted.begin(), sorted.end());
    FileIO out("timestamp.txt", "w");
    for (auto [time, name]: sorted)
    {
        out.printf("%lld %s\n", time, name.c_str());
    }
    is.handle = nullptr;
}
