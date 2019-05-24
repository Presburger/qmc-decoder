#include <algorithm>
#include <assert.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>


#ifndef _WIN64
#include <mutex>
#include <thread>

#define THREAD_NUM 4
#endif

using namespace std;
namespace fs = std::filesystem;

class Seed {
public:
  Seed() : x(-1), y(8), dx(1), index(-1) {
    seedMap = {{0x4a, 0xd6, 0xca, 0x90, 0x67, 0xf7, 0x52},
               {0x5e, 0x95, 0x23, 0x9f, 0x13, 0x11, 0x7e},
               {0x47, 0x74, 0x3d, 0x90, 0xaa, 0x3f, 0x51},
               {0xc6, 0x09, 0xd5, 0x9f, 0xfa, 0x66, 0xf9},
               {0xf3, 0xd6, 0xa1, 0x90, 0xa0, 0xf7, 0xf0},
               {0x1d, 0x95, 0xde, 0x9f, 0x84, 0x11, 0xf4},
               {0x0e, 0x74, 0xbb, 0x90, 0xbc, 0x3f, 0x92},
               {0x00, 0x09, 0x5b, 0x9f, 0x62, 0x66, 0xa1}};
  }

  uint8_t NextMask() {
    uint8_t ret;
    index++;
    if (x < 0) {
      dx = 1;
      y = (8 - y) % 8;
      ret = (8 - y) % 8;
      ret = 0xc3;
    } else if (x > 6) {
      dx = -1;
      y = 7 - y;
      ret = 0xd8;
    } else {
      ret = seedMap[y][x];
    }

    x += dx;
    if (index == 0x8000 || (index > 0x8000 && (index + 1) % 0x8000 == 0))
      return NextMask();
    return ret;
  }

private:
  int x;
  int y;
  int dx;
  int index;
  vector<vector<uint8_t>> seedMap;
};
#ifndef _WIN64
std::mutex mtx;
void print_thread_s(const char *str, std::ostream &stm) {
  std::lock_guard<std::mutex> lock(mtx);
  stm << str << endl;
}
#endif

void process(string dir) {
  std::string print_str("decode: ");
  print_str += dir;
#ifdef _WIN64
  std::cout << print_str << endl;
#else
  print_thread_s(print_str.c_str(), std::cout);
#endif
  std::fstream infile(dir.c_str(), std::ios::in | std::ios::binary);
  if (!infile.is_open()) {
#ifdef _WIN64
    std::cerr << "qmc file read error" << endl;
#else
    print_thread_s("qmc file read error", std::cerr);
#endif
    return;
  }

  string outloc(std::move(dir));

  string bak;
  char x;
  while ((x = outloc.back()) != '.') {
    bak.push_back(x);
    outloc.pop_back();
  }

  if (bak != "0cmq" && bak != "calfcmq" && bak != "3cmq")
    return;

  assert(bak.size() > 3);
  for (int u = 0; u < 3; ++u)
    bak.pop_back();
  std::reverse(bak.begin(), bak.end());
  if (bak == "0" || bak == "3")
    bak = "mp3";
  outloc += bak;

  auto len = infile.seekg(0, std::ios::end).tellg();
  infile.seekg(0, ios::beg);
  char *buffer = new char[len];

  infile.read(buffer, len);
  infile.close();

  Seed seed;
  for (int i = 0; i < len; ++i) {
    buffer[i] = seed.NextMask() ^ buffer[i];
  }

  std::fstream outfile(outloc.c_str(), ios::out | ios::binary);

  if (outfile.is_open()) {
    outfile.write(buffer, len);
    outfile.close();
  } else {
#ifdef _WIN64
    std::cerr << "open dump file error" << endl;
#else
    print_thread_s("open dump file error", std::cerr);
#endif
  }
  delete[] buffer;
}

#ifndef _WIN64
void thread_block(const vector<string> *qmc_collection, int id) {
  for (int i = id; i < qmc_collection->size(); i += THREAD_NUM) {
    process(qmc_collection->operator[](i));
  }
}
#endif

int main(int argc, char **argv) {

  if (argc > 1) {
#ifndef _WIN64
    print_thread_s(
        "put decoder binary file in your qmc file directory, then run it.",
        std::cout);
#else
    cout
        << "put decoder binary file in your qmc file directory, then run it.\n";
#endif
    return 1;
  }

  fs::path qmc_dir(".");

  fs::recursive_directory_iterator eod;

  vector<string> qmc_collection;

  for (fs::recursive_directory_iterator i(qmc_dir); i != eod; ++i) {
    fs::path fp = *i;
    if (fs::is_regular_file(fp)) {
      if (fp.string().find(".qmc3") != string::npos ||
          fp.string().find(".qmc0") != string::npos ||
          fp.string().find(".qmcflac") != string::npos)
        qmc_collection.emplace_back(fp.string());
    }
  };
#ifdef _WIN64
  for (auto &&qmc_file : qmc_collection)
    process(qmc_file);

#else
  vector<std::thread> td_group;
  for (int i = 1; i < THREAD_NUM; ++i)
    td_group.emplace_back(thread_block, &qmc_collection, i);
  thread_block(&qmc_collection, 0);
  for (auto &&x : td_group)
    x.join();
#endif

  return 0;
}
