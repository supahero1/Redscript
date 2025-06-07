#include "file.hpp"

std::string readFile(const std::filesystem::path& path)
{
    std::fstream stream(path);

    if(!stream.good()) return std::string();

    std::stringstream ss;

    ss << stream.rdbuf();

    return ss.str();
}