#include <cstdint>
#include <fstream>
#include <vector>
#include "fmplay.hpp"

std::vector<uint8_t>
read_file(const char *filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    file.exceptions(std::ifstream::failbit);
    file.seekg(0, std::ios_base::end);
    std::vector<uint8_t> contents(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char *>(&contents[0]), contents.size());
    return contents;
}
