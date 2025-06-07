#pragma once

#include <unordered_map>
#include <variant>
#include "file.hpp"
#include "error.hpp"

typedef std::variant<int, std::string> rs_config_value;

struct rs_config
{
    std::unordered_map<std::string, rs_config_value> dict;

    template<typename _V>
    inline _V& get(const std::string& s)
    {
        auto f = dict.find(s);

        return std::get<_V>(f->second);
    }
    inline bool exists(const std::string& s)
    {
        return dict.find(s) != dict.end();
    }
};

rs_config readConfig(const std::string& path, rs_error* err);