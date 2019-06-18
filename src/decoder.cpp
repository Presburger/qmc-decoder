#include "Seed.hpp"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>
#include <vector>

using namespace std;
namespace fs = filesystem;

void
thread_block(const vector<string> collection, int id, int count);
void
print_thread_s(string str, ostream& stm);
void
process(string dir);

mutex mtx;
const regex qmc_regex{ "^.+\\.(qmc3|qmc0|qmcflac)$" };

int
main(int argc, char** argv)
{
  if (argc > 1) {
    print_thread_s(
      "put decoder binary file in your qmc file directory, then run it.", cout);
    return 1;
  }

  vector<string> qmc_paths;

  for (fs::recursive_directory_iterator i{ fs::current_path() }, end; i != end;
       ++i) {
    auto file_path = i->path().string();
    if (i->is_regular_file() && regex_match(file_path, qmc_regex)) {
      qmc_paths.emplace_back(file_path);
    }
  };

  const auto n_thread = thread::hardware_concurrency();
  vector<thread> td_group;

  for (size_t i = 0; i < n_thread; ++i) {
    td_group.emplace_back(thread_block, qmc_paths, i, n_thread);
  }

  for (auto&& td : td_group) {
    td.join();
  }

  return 0;
}

void
thread_block(const vector<string> collection, int id, int count)
{
  for (size_t i = id; i < collection.size(); i += count) {
    process(collection[i]);
  }
}

void
print_thread_s(string str, ostream& stm)
{
  lock_guard<mutex> lock(mtx);
  stm << str << endl;
}

void
process(string dir)
{
  string print_str("decode: ");
  print_str += dir;
  print_thread_s(print_str, cout);
  fstream infile(dir, ios::in | ios::binary);
  if (!infile.is_open()) {
    print_thread_s("qmc file read error", cerr);
    return;
  }

  string outloc(move(dir));

  string bak;
  char x;
  while ((x = outloc.back()) != '.') {
    bak.push_back(x);
    outloc.pop_back();
  }
  static regex tmp_regex{ "(0cmq|calfcmq|3cmq)" };
  if (!regex_match(bak, tmp_regex))
    return;

  assert(bak.size() > 3);
  for (int u = 0; u < 3; ++u)
    bak.pop_back();
  reverse(bak.begin(), bak.end());
  if (bak == "0" || bak == "3")
    bak = "mp3";
  outloc += bak;

  auto len = infile.seekg(0, ios::end).tellg();
  infile.seekg(0, ios::beg);
  char* buffer = new char[len];

  infile.read(buffer, len);
  infile.close();

  Seed seed;
  for (int i = 0; i < len; ++i) {
    buffer[i] = seed.NextMask() ^ buffer[i];
  }

  fstream outfile(outloc.c_str(), ios::out | ios::binary);

  if (outfile.is_open()) {
    outfile.write(buffer, len);
    outfile.close();
  } else {
    print_thread_s("open dump file error", cerr);
  }
  delete[] buffer;
}
