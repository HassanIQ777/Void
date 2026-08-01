#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "libutils/src/CLIParser.hpp"
#include "libutils/src/File.hpp"
#include "libutils/src/Log.hpp"
#include "libutils/src/Random.hpp"
#include "libutils/src/funcs.hpp"

using funcs::print;

bool voidFile(const std::string &filepath, int iterations = 3);

int main(int argc, char *argv[]) {
  Random::seed(0);
  CLIParser parser(argc, argv);
  const std::string VERSION = "1.0";

  if (argc == 1 || parser.hasFlag("-h")) {
    print("void, a program that deletes a file in a way it can't be "
          "recovered.\n");
    print("Usage:\n");
    print("  ", parser.getArg(0), " -h",
          "                            print help", "\n");
    print("  ", parser.getArg(0), " -v",
          "                            print version", "\n");
    print("  ", parser.getArg(0), " -i <VALUE> <FILE>",
          "             iterations, default=3", "\n");
    print("  ", parser.getArg(0), " <FILE1> <FILE2> ...",
          "           print version", "\n");
    return 0;
  }

  if (parser.hasFlag("-v")) {
    print("void version ", VERSION, "\n");
    return 0;
  }

  int iterations = 3;
  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    std::string arg = parser.getArg(i);

    if (arg == "-i") {
      if (i + 1 >= argc) {
        print("void: -i requires a value\n");
        return 1;
      }
      try {
        iterations = std::stoi(parser.getArg(i + 1));
      } catch (...) {
        print("void: invalid iteration count '", parser.getArg(i + 1), "'\n");
        return 1;
      }
      i++; // skip
      continue;
    }

    files.push_back(arg);
  }

  if (files.empty()) {
    print("void: no files to delete");
    return 1;
  }

  int exitCode = 0;

  for (const auto &file : files) {
#ifdef DEBUG
    Log::setLogLevel(Log::LogLevel::Debug);
    Log::debug("void: voiding '", file, "'");
#endif
    if (File::isdirectory(file)) {
      print("void: cannot remove '", file, "': Can't void directories\n");
      exitCode = 1;
      continue;
    }
    if (!File::isfile(file)) {
      print("void: cannot remove '", file, "': No such file\n");
      exitCode = 1;
      continue;
    }
    if (!voidFile(file, iterations)) {
      print("void: cannot remove '", file, "': ", std::strerror(errno), "\n");
      exitCode = 1;
    }
  }

  return exitCode;
}

bool voidFile(const std::string &filepath, int iterations) {
  for (int i = 0; i < iterations; i++) {
    std::fstream file(filepath,
                      std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open())
      return false;

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    if (size <= 0)
      break; // nothing left to shred, stop overwriting — but still fall through
             // to remove below

    std::vector<char> buffer(size);
    for (auto &byte : buffer) {
      byte = static_cast<char>(Random::getint(0, 255));
    }

    file.seekp(0, std::ios::beg);
    file.write(buffer.data(), size);
    file.flush();
    file.close();
  }

  // truncate once, after all overwrite passes are done — not per-pass
  {
    std::ofstream truncateFile(filepath, std::ios::out | std::ios::trunc);
  }

  // the part that was missing the whole time: actually unlink it
  if (std::remove(filepath.c_str()) != 0) {
    return false;
  }
  return true;
}