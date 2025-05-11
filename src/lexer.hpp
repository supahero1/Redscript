#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <tuple>
#include "token.hpp"
#include "error.hpp"
#include "constants.hpp"

struct lex_info
{
    const std::unordered_map<std::string, std::tuple<token_type, uint32_t>> keywords = RS_LANG_KEYWORDS; 
};

token_list tlex(const std::string&, std::string&, rs_error*);
