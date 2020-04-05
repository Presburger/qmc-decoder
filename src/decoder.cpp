#include <algorithm>
#include <boost/filesystem.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <memory>

#include "seed.hpp"

namespace fs = boost::filesystem;

namespace {
  void close_file(std::FILE* fp) { std::fclose(fp);}
  using smartFilePtr = std::unique_ptr<std::FILE, decltype(&close_file)>;

  enum class openMode {
    read,
    write
  };

  smartFilePtr openFile(const std::string& aDir, openMode aOpenMode)  {
    #ifndef _WIN32
      std::FILE* fp = fopen(aDir.c_str(), aOpenMode == openMode::read ? "rb" : "wb");
    #else
      std::wstring dir_w;
      dir_w.resize(aDir.size());
      int newSize =
          MultiByteToWideChar(CP_UTF8, 0, aDir.c_str(), aDir.length(),
                              const_cast<wchar_t *>(dir_w.c_str()), dir_w.size());
      filePathW.resize(newSize);
      std::FILE* fp = _wfopen(dir_w.c_str(), aOpenMode == openMode::read ? L"rb" : L"wb");
    #endif
      return smartFilePtr(fp, &close_file);
  }
}

static const std::regex mp3_regex{"\\.(qmc3|qmc0)$"};
static const std::regex ogg_regex{"\\.qmcogg$"};
static  const std::regex flac_regex{"\\.qmcflac$"};

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

  std::unique_ptr<char[]> buffer(new(std::nothrow) char[len]);
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
  std::vector<std::string> qmc_paths;

  for (auto & p : fs::recursive_directory_iterator(fs::path("."))) {
    auto file_path = p.path().string();

    if ((fs::status(p).permissions() & fs::perms::owner_read) !=
            fs::perms::no_perms &&
        fs::is_regular_file(p) && regex_match(file_path, qmc_regex)) {
      qmc_paths.emplace_back(std::move(file_path));
    }
  };

  std::for_each(qmc_paths.begin(), qmc_paths.end(), sub_process);

  return 0;
}
