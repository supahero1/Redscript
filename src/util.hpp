#pragma once
#include <cctype>  // for std::isalnum

inline std::string removeSpecialCharacters(const std::string& input) {
    std::string output;
    for (char c : input) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.') {
            output += c;
        }
    }
    return output;
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
