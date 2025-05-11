#pragma once
#include "constants.hpp"

#define RS_LANG_KEYWORDS {{"true", {token_type::KW_TRUE,0}}, \
    {"false", {token_type::KW_FALSE,0}}, \
    {"int", {token_type::TYPE_DEF, 1}}, \
    {"float", {token_type::TYPE_DEF, 2}}, \
    {"bool", {token_type::TYPE_DEF, 3} }, \
    {"string", {token_type::TYPE_DEF, 4}}, \
    {"list", {token_type::TYPE_DEF, 5}}, \
    {"object", {token_type::TYPE_DEF, 6}}, \
    {"any", {token_type::TYPE_DEF, 0}}, \
    {"return", {token_type::KW_RETURN,0}}, \
    {"method", {token_type::KW_METHOD,0}}, \
    {"const", {token_type::KW_CONST,0}}, \
    {"for", {token_type::KW_FOR,0}}, \
    {"while", {token_type::KW_WHILE,0}}, \
    {"break", {token_type::KW_BREAK,0}}, \
    {"continue", {token_type::KW_CONTINUE,0}}, \
    {"or", {token_type::KW_OR,0}}, \
    {"and", {token_type::KW_AND,0}}, \
    {"null", {token_type::KW_NULL,0}}, \
    {"asm", {token_type::KW_ASM,0}}}

