#pragma once

#include <cstdint>
#include <string>

namespace pe {

std::string readStringFromFile(const std::string& path);
void writeStringToFile(const std::string& path, const std::string& data);
void writeBytesToFile(const std::string& path, const uint8_t* data, size_t size);
uint8_t* readBytesFromFile(const std::string& path, size_t* outSize);

char* InetNtoa(unsigned int addr);

} // namespace pe
