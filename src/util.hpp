#pragma once
#include <cctype>  // for std::isalnum
#include <algorithm>

inline std::string removeSpecialCharacters(const std::string& input) {
    std::string output;
    for (char c : input) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.') {
            output += c;
        }
        else if (c == '-')
        {
            output += '_';
        }
    }
    return output;
}
inline void toLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c){ return std::tolower(c); });
}
template<typename T>
inline T& unmove(T&& x)
{
    return x;
}


namespace util
{
    template<typename T>
    constexpr T copy(const T& t)
    {
        return T(t);
    }
}
