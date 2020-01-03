#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>
#include <vector>

#include "seed.hpp"

using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::lock_guard;
using std::move;
using std::mutex;
using std::regex;
using std::regex_replace;
using std::string;
using std::thread;
using std::unique_ptr;
using std::vector;

namespace fs = std::filesystem;

mutex mtx;

template <typename T>
void safe_out(const T &data) {
  lock_guard<mutex> lock(mtx);
  cout << data << endl;
}

void sub_process(const string &dir) {
  safe_out("decode: " + dir);
  fstream infile(dir, ios::in | ios::binary);
  if (!infile.is_open()) {
    safe_out("qmc file read error");
    return;
  }

  string outloc(move(dir));
  const regex mp3_regex{"\\.(qmc3|qmc0|qmcogg)$"}, flac_regex{"\\.qmcflac$"};
  auto mp3_outloc = regex_replace(outloc, mp3_regex, ".mp3");
  auto flac_outloc = regex_replace(outloc, flac_regex, ".flac");
  outloc = (outloc != mp3_outloc ? mp3_outloc : flac_outloc);
  auto len = infile.seekg(0, ios::end).tellg();
  infile.seekg(0, ios::beg);
  char *buffer = new (std::nothrow) char[len];
  if (buffer == nullptr) {
    safe_out("create buffer error");
    return;
  }
  unique_ptr<char[]> auto_delete(buffer);

  infile.read(buffer, len);
  infile.close();

  qmc_decoder::seed seed_;
  for (int i = 0; i < len; ++i) {
    buffer[i] = seed_.next_mask() ^ buffer[i];
  }

  fstream outfile(outloc.c_str(), ios::out | ios::binary);

  if (outfile.is_open()) {
    outfile.write(buffer, len);
    outfile.close();
  } else {
    safe_out("open dump file error");
  }
}

int main(int argc, char **argv) {
  // Set locale encoding to properly handle files containing non-ASCii characters in the file name
  setlocale(LC_ALL,".UTF-8");
  std::locale::global(std::locale(".UTF-8"));
  if (argc > 1) {
    std::cerr
        << "put decoder binary file in your qmc file directory, then run it."
        << std::endl;
    return -1;
  }

  if ((fs::status(fs::path(".")).permissions() & fs::perms::owner_write) ==
      fs::perms::none) {
    std::cerr << "please check if you have the write permissions on this dir."
              << std::endl;
    return -1;
  }
  vector<string> qmc_paths;
  const regex qmc_regex{"^.+\\.(qmc3|qmc0|qmcflac|qmcogg)$"};

  for (fs::recursive_directory_iterator i{fs::path(".")}, end; i != end; ++i) {
    auto file_path = i->path().string();
    // std::cout<<file_path<<endl;
    if ((fs::status(*i).permissions() & fs::perms::owner_read) !=
            fs::perms::none &&
        fs::is_regular_file(*i) && regex_match(file_path, qmc_regex)) {
      qmc_paths.emplace_back(std::move(file_path));
    }
  };

  const auto n_thread = thread::hardware_concurrency();
  vector<thread> td_group;

  for (size_t i = 0; i < n_thread - 1; ++i) {
    td_group.emplace_back(
        [&qmc_paths, &n_thread](int index) {
          for (size_t j = index; j < qmc_paths.size(); j += n_thread) {
            sub_process(qmc_paths[j]);
          }
        },
        i);
  }

  for (auto &&td : td_group) {
    td.join();
  }

  return 0;
}
