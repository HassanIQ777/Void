#include <fstream>
#include <iostream>
#include <random>
#include <vector>

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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (auto &byte : buffer) {
      byte = static_cast<char>(dis(gen));
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

int main() {
  
}
