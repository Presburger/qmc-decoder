#include "seed.hpp"
#include <cassert>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>
#include <vector>

using std::fstream;
using std::ios;
using std::move;
using std::regex;
using std::regex_replace;
using std::string;
using std::thread;
using std::vector;
using std::cout;
using std::mutex;
using std::lock_guard;
using std::mutex;

namespace fs = boost::filesystem;

mutex mtx;

inline void safe_out(const string &data)
{
    lock_guard<std::mutex> lock(mtx);
    cout << data << std::endl;
}
inline void safe_out(const char *data)
{
    lock_guard<std::mutex> lock(mtx);
    cout << data << std::endl;
}

void process(const string &dir)
{
    safe_out("decode: " + dir);
    fstream infile(dir, ios::in | ios::binary);
    if (!infile.is_open())
    {
        safe_out("qmc file read error");
        return;
    }

    string outloc(move(dir));
    const regex mp3_regex{"\\.(qmc3|qmc0)$"}, flac_regex{"\\.qmcflac$"};
    auto mp3_outloc = regex_replace(outloc, mp3_regex, ".mp3");
    auto flac_outloc = regex_replace(outloc, flac_regex, ".flac");

    assert(mp3_outloc != flac_outloc);
    outloc = (outloc != mp3_outloc ? mp3_outloc : flac_outloc);

    auto len = infile.seekg(0, ios::end).tellg();
    infile.seekg(0, ios::beg);
    char *buffer = new char[len];

    infile.read(buffer, len);
    infile.close();

    qmc_decoder::seed seed_;
    for (int i = 0; i < len; ++i)
    {
        buffer[i] = seed_.NextMask() ^ buffer[i];
    }

    fstream outfile(outloc.c_str(), ios::out | ios::binary);

    if (outfile.is_open())
    {
        outfile.write(buffer, len);
        outfile.close();
    }
    else
    {
        safe_out("open dump file error");
    }
    delete[] buffer;
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        std::cerr << "put decoder binary file in your qmc file directory, then run it." << std::endl;
        return 1;
    }

    vector<string> qmc_paths;
    const regex qmc_regex{"^.+\\.(qmc3|qmc0|qmcflac)$"};

    for (fs::recursive_directory_iterator i{fs::path(".")}, end; i != end; ++i)
    {
        auto file_path = i->path().string();
        if (fs::is_regular_file(*i) && regex_match(file_path, qmc_regex))
        {
            qmc_paths.emplace_back(file_path);
        }
    };


    const auto n_thread = thread::hardware_concurrency();
    vector<thread> td_group;

    for (size_t i = 0; i < n_thread - 1; ++i)
    {
        td_group.emplace_back([&qmc_paths, &n_thread](int index) {
            for (size_t j = index; j < qmc_paths.size(); j += n_thread)
            {
                process(qmc_paths[j]);
            }
        },
                              i);
    }
    for (size_t j = n_thread - 1; j < qmc_paths.size(); j += n_thread)
    {
        process(qmc_paths[j]);
    }

    for (auto &&td : td_group)
    {
        td.join();
    }

    return 0;
}