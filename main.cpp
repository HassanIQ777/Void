#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "libutils/src/CLIParser.hpp"
#include "libutils/src/Random.hpp"
#include "libutils/src/color.hpp"
#include "libutils/src/funcs.hpp"
using funcs::print;

bool voidFile(const std::string &filepath, int iterations = 3) {
  for (int i = 0; i < iterations; i++) {

    std::fstream file(filepath,
                      std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open())
      return false;

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    if (size <= 0)
      return true;

    // random data to overwrite the file
    std::vector<char> buffer(size);

    for (auto &byte : buffer) {
      byte = static_cast<char>(Random::getint(0, 255));
    }

    // overwrite
    file.seekp(0, std::ios::beg);
    file.write(buffer.data(), size);

    // flush data out of RAM caches
    file.flush();
    file.close();

    std::ofstream truncateFile(filepath, std::ios::out | std::ios::trunc);
  }
  return true;
}

int main(int argc, char *argv[]) {
  Random::seed(0);
  CLIParser parser(argc, argv);
  const std::string VERSION = "1.0";

  if (argc == 1 || parser.hasFlag("-h")) {
    print("void, a program that deletes a file in a way it can't be "
          "recovered.\n");
    print("Usage:\n");
    print("  ", parser.getArg(0), " -h", "           print help", "\n");
    print("  ", parser.getArg(0), " -v", "           print version", "\n");
    print("  ", parser.getArg(0), " -i <VALUE>", "   iterations, default=3",
          "\n");
    return 0;
  }

  if (parser.hasFlag("-v")) {
    print("void version ", VERSION, "\n");
    return 0;
  }
}
