#pragma once
#include "constants.hpp"
#define RS_INT_KW_ID 1
#define RS_FLOAT_KW_ID 2
#define RS_BOOL_KW_ID 3
#define RS_STRING_KW_ID 4
#define RS_LIST_KW_ID 5
#define RS_OBJECT_KW_ID 6
#define RS_ANY_KW_ID 0

#define RS_LANG_KEYWORDS {{"true", {token_type::KW_TRUE,0}}, \
    {"false", {token_type::KW_FALSE,0}}, \
    {"int", {token_type::TYPE_DEF, 1}}, \
    {"float", {token_type::TYPE_DEF, 2}}, \
    {"bool", {token_type::TYPE_DEF, 3} }, \
    {"string", {token_type::TYPE_DEF, 4}}, \
    {"list", {token_type::TYPE_DEF, 5}}, \
    {"object", {token_type::TYPE_DEF, 6}}, \
    {"selector", {token_type::TYPE_DEF, 7}}, \
    {"any", {token_type::TYPE_DEF, 0}}, \
    {"void", {token_type::TYPE_DEF, -1}}, \
    {"return", {token_type::KW_RETURN,0}}, \
    {"method", {token_type::KW_METHOD,0}}, \
    {"module", {token_type::KW_MODULE,0}}, \
    {"const", {token_type::KW_CONST,0}}, \
    {"optional", {token_type::KW_OPTIONAL,0}}, \
    {"required", {token_type::KW_REQUIRED,0}}, \
    {"seperate", {token_type::KW_SEPERATE,0}}, \
    {"for", {token_type::KW_FOR,0}}, \
    {"while", {token_type::KW_WHILE,0}}, \
    {"break", {token_type::KW_BREAK,0}}, \
    {"continue", {token_type::KW_CONTINUE,0}}, \
    {"use", {token_type::KW_USE,0}}, \
    {"if", {token_type::KW_IF,0}}, \
    {"else", {token_type::KW_ELSE,0}}, \
    {"elif", {token_type::KW_ELIF,0}}, \
    {"or", {token_type::KW_OR,0}}, \
    {"not", {token_type::KW_NOT,0}}, \
    {"and", {token_type::KW_AND,0}}, \
    {"null", {token_type::KW_NULL,0}}, \
    {"asm", {token_type::KW_ASM,0}}}

