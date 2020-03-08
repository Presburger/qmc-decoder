#include <boost/filesystem.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include "seed.hpp"

using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::move;
using std::regex;
using std::regex_replace;
using std::string;
using std::unique_ptr;
using std::vector;

namespace fs = boost::filesystem;

void sub_process(const string &dir) {
  std::cout << "decode: " + dir << std::endl;
  fstream infile(dir, ios::in | ios::binary);
  if (!infile.is_open()) {
    std::cerr << "qmc file read error" << std::endl;
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
    std::cerr << "create buffer error" << std::endl;
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
    std::cerr << "open dump file error" << std::endl;
  }
}

int main(int argc, char **argv) {
  // Set locale encoding to properly handle files containing non-ASCii
  // characters in the file name
  std::locale::global(std::locale("en_US.UTF-8"));
  if (argc > 1) {
    std::cerr
        << "put decoder binary file in your qmc file directory, then run it."
        << std::endl;
    return -1;
  }

  if ((fs::status(fs::path(".")).permissions() & fs::perms::owner_write) ==
      fs::perms::no_perms) {
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
            fs::perms::no_perms &&
        fs::is_regular_file(*i) && regex_match(file_path, qmc_regex)) {
      qmc_paths.emplace_back(std::move(file_path));
    }
  };

  for (auto &&elem : qmc_paths) sub_process(elem);

  return 0;
}
