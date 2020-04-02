#include <boost/filesystem.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include "seed.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::move;
using std::regex;
using std::regex_replace;
using std::string;
using std::unique_ptr;
using std::vector;

namespace fs = boost::filesystem;

void sub_process(const string &dir) {
  std::cout << "decode: " + dir << std::endl;
  string outloc(move(dir));
  const regex mp3_regex{"\\.(qmc3|qmc0)$"};
  const regex ogg_regex{"\\.qmcogg$"};
  const regex flac_regex{"\\.qmcflac$"};
  auto mp3_outloc = regex_replace(outloc, mp3_regex, ".mp3");
  auto flac_outloc = regex_replace(outloc, flac_regex, ".flac");
  auto ogg_outloc = regex_replace(outloc, ogg_regex, ".ogg");
  if (mp3_outloc != outloc)
    outloc = mp3_outloc;
  else if (flac_outloc != outloc)
    outloc = flac_outloc;
  else
    outloc = ogg_outloc;
#ifndef _WIN32
  FILE *infile = fopen(dir.c_str(), "rb");
#else
  std::wstring dir_w;
  dir_w.resize(dir.size());
  int newSize =
      MultiByteToWideChar(CP_UTF8, 0, dir.c_str(), dir.length(),
                          const_cast<wchar_t *>(dir_w.c_str()), dir_w.size());
  filePathW.resize(newSize);
  FILE *infile = _wfopen(dir_w.c_str(), L"rb");
#endif

  if (infile == NULL) {
    cerr << "failed read file: " << outloc << endl;
    return;
  }
  unique_ptr<FILE, std::function<void(FILE *)>> autoclose_infile(
      infile, [](FILE *f) { fclose(f); });

  int res = fseek(infile, 0, SEEK_END);
  if (res != 0) {
    cerr << "seek file failed" << endl;
    return;
  }

  auto len = ftell(infile);
  res = fseek(infile, 0, SEEK_SET);

  char *buffer = new (std::nothrow) char[len];
  if (buffer == nullptr) {
    cerr << "create buffer error" << endl;
    return;
  }
  unique_ptr<char[]> auto_delete(buffer);

  auto fres = fread(buffer, 1, len, infile);
  if (fres != len) {
    cerr << "read file error" << endl;
  }

  qmc_decoder::seed seed_;
  for (int i = 0; i < len; ++i) {
    buffer[i] = seed_.next_mask() ^ buffer[i];
  }

#ifndef _WIN32
  FILE *outfile = fopen(outloc.c_str(), "wb");
#else
  std::wstring outloc_w;
  outloc_w.resize(outloc.size());
  int newSize = MultiByteToWideChar(CP_UTF8, 0, outloc.c_str(), outloc.length(),
                                    const_cast<wchar_t *>(outloc_w.c_str()),
                                    outloc_w.size());
  outloc_w.resize(newSize);
  FILE *infile = _wfopen(outloc_w.c_str(), L"rb");
#endif

  if (outfile == NULL) {
    cerr << "failed write file: " << outloc << endl;
    return;
  }

  std::unique_ptr<FILE, std::function<void(FILE *)>> autoclose_outfile(
      outfile, [](FILE *f) { fclose(f); });
  fres = fwrite(buffer, 1, len, outfile);
  if (fres != len) {
    cerr << "write file error" << endl;
  }
}

int main(int argc, char **argv) {
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

    if ((fs::status(*i).permissions() & fs::perms::owner_read) !=
            fs::perms::no_perms &&
        fs::is_regular_file(*i) && regex_match(file_path, qmc_regex)) {
      qmc_paths.emplace_back(std::move(file_path));
    }
  };

  for (auto &&elem : qmc_paths) sub_process(elem);

  return 0;
}
