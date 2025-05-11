#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <format>

#include "error.hpp"

enum class token_type
{
    WORD,
    STRING_LITERAL,
    INT_LITERAL,
    FLOAT_LITERAL,
    LIST_LITERAL,
    OBJECT_LITERAL,
    SELECTOR_LITERAL, // @<name>

    COMMENT_INLINE,
    COMMENT_MULTILINE,
    
    BRACKET_OPEN,
    BRACKET_CLOSED,
    CBRACKET_OPEN,
    CBRACKET_CLOSED,
    SQBRACKET_OPEN,
    SQBRACKET_CLOSED,
    UNKNOWN,
    
    /* -- SYMBOL GROUPS -- */
    OPERATOR,

    SYMBOL,

    /* -- KEYWORDS -- */
    KW_OR, KW_AND,
    KW_TRUE, KW_FALSE,

    TYPE_DEF,

    KW_INT_TYPE,
    KW_FLOAT_TYPE,
    KW_BOOL_TYPE,
    KW_STRING_TYPE,
    KW_LIST_TYPE,
    KW_OBJECT_TYPE,
    KW_ANY_TYPE,

    KW_RETURN,
    KW_METHOD,

    KW_CONST,

    KW_FOR,
    KW_WHILE,
    KW_BREAK,
    KW_CONTINUE,

    KW_ASM,
    KW_NULL,

    LINE_END

};

struct token
{
    std::string repr;
    token_type  type;
    uint32_t    info;

    raw_trace_info trace;

    std::string str()
    {
        if (type == token_type::STRING_LITERAL)
            return std::format("{{\"{}\", {}, {}}}", repr, static_cast<int>(type), info);
        return std::format("{{{}, {}, {}}}", repr, static_cast<int>(type), info);
    }
    token(std::string _repr, token_type _type, uint32_t _info, raw_trace_info _trace, size_t start)
        : repr(_repr), type(_type), info(_info), trace(_trace)
    {
        trace.start = start;
    }
    token(std::string _repr, token_type _type, uint32_t _info, raw_trace_info _trace)
        : repr(_repr), type(_type), info(_info), trace(_trace) {}
};

typedef std::vector<token> token_list;