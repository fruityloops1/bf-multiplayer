#include "pe/Util.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

namespace pe {

std::string readStringFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void writeStringToFile(const std::string& path, const std::string& data)
{
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    file << data;
}

void writeBytesToFile(const std::string& path, const uint8_t* data, size_t size)
{
    FILE* file = fopen(path.c_str(), "wb");
    fwrite(data, size, 1, file);
    fclose(file);
}

uint8_t* readBytesFromFile(const std::string& path, size_t* outSize)
{
    FILE* file = fopen(path.c_str(), "rb");
    fseek(file, 0, SEEK_END);

    *outSize = ftell(file);

    rewind(file);
    uint8_t* data = (uint8_t*)malloc(*outSize);
    fread(data, *outSize, 1, file);
    fclose(file);
    return data;
}

char* InetNtoa(unsigned int addr)
{
    in_addr inaddr;
#ifdef _WIN32
        inaddr.S_un.S_addr = addr;
#else
        inaddr = { addr };
#endif
    return inet_ntoa(inaddr);
}

} // namespace pe
