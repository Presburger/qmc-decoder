/*
 * Author: mayusheng - mayusheng@huawei.com
 * Last modified: 2020-06-29 10:56
 * Filename: decoder.cpp
 *
 * Description: qmc file auto decode
 *
 *
 */
#include <algorithm>
#include <iostream>
#include <memory>
#include <regex>
#include <vector>

#include "seed.hpp"

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

namespace {
void close_file(std::FILE* fp) { std::fclose(fp); }
using smartFilePtr = std::unique_ptr<std::FILE, decltype(&close_file)>;

enum class openMode { read, write };
/**
 * @brief open a file
 *
 * @param aDir
 * @param aOpenMode
 * @return smartFilePtr
 */
smartFilePtr openFile(const std::string& aPath, openMode aOpenMode) {
#ifndef _WIN32
  std::FILE* fp =
      fopen(aPath.c_str(), aOpenMode == openMode::read ? "rb" : "wb");
#else
  std::wstring aPath_w;
  aPath_w.resize(aPath.size());
  int newSize = MultiByteToWideChar(CP_UTF8, 0, aPath.c_str(), static_cast<int>(aPath.length()),
                                    const_cast<wchar_t*>(aPath_w.c_str()),
                                    static_cast<int>(aPath_w.size()));
  aPath_w.resize(newSize);
  std::FILE* fp = NULL;
  _wfopen_s(&fp, aPath_w.c_str(), aOpenMode == openMode::read ? L"rb" : L"wb");
#endif
  return smartFilePtr(fp, &close_file);
}

static const std::regex mp3_regex{"\\.(qmc3|qmc0)$"};
static const std::regex ogg_regex{"\\.qmcogg$"};
static const std::regex flac_regex{"\\.qmcflac$"};

void sub_process(std::string dir) {
  std::cout << "decode: " + dir << std::endl;
  std::string outloc(dir);

  auto mp3_outloc = regex_replace(outloc, mp3_regex, ".mp3");
  auto flac_outloc = regex_replace(outloc, flac_regex, ".flac");
  auto ogg_outloc = regex_replace(outloc, ogg_regex, ".ogg");

  if (mp3_outloc != outloc)
    outloc = mp3_outloc;
  else if (flac_outloc != outloc)
    outloc = flac_outloc;
  else
    outloc = ogg_outloc;

  auto infile = openFile(dir, openMode::read);

  if (infile == nullptr) {
    std::cerr << "failed read file: " << outloc << std::endl;
    return;
  }

  int res = fseek(infile.get(), 0, SEEK_END);
  if (res != 0) {
    std::cerr << "seek file failed" << std::endl;
    return;
  }

  auto len = ftell(infile.get());
  res = fseek(infile.get(), 0, SEEK_SET);

  std::unique_ptr<char[]> buffer(new (std::nothrow) char[len]);
  if (buffer == nullptr) {
    std::cerr << "create buffer error" << std::endl;
    return;
  }

  auto fres = fread(buffer.get(), 1, len, infile.get());
  if (fres != len) {
    std::cerr << "read file error" << std::endl;
  }

  qmc_decoder::seed seed_;
  for (int i = 0; i < len; ++i) {
    buffer[i] = seed_.next_mask() ^ buffer[i];
  }

  auto outfile = openFile(outloc, openMode::write);

  if (outfile == nullptr) {
    std::cerr << "failed write file: " << outloc << std::endl;
    return;
  }

  fres = fwrite(buffer.get(), 1, len, outfile.get());
  if (fres != len) {
    std::cerr << "write file error" << std::endl;
  }
}

static const std::regex qmc_regex{"^.+\\.(qmc3|qmc0|qmcflac|qmcogg)$"};
}  // namespace

int main(int argc, char** argv) {
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
  std::vector<std::string> qmc_paths;

  for (auto& p : fs::recursive_directory_iterator(fs::path("."))) {
    auto file_path = p.path().string();

    if ((fs::status(p).permissions() & fs::perms::owner_read) !=
            fs::perms::none &&
        fs::is_regular_file(p) && regex_match(file_path, qmc_regex)) {
      qmc_paths.emplace_back(std::move(file_path));
    }
  };

  std::for_each(qmc_paths.begin(), qmc_paths.end(), sub_process);

  return 0;
}
