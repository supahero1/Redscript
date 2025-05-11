#include "lexer.hpp"
// ex:
// LEX_ERROR(RS_SYNTAX_ERROR, "Something went wrong because of the number {}", 44);
#define LEX_ERROR(_ec, message, ...)                                    \
    {                                                                   \
        trace.ec = _ec;                                                 \
        *err = rs_error(message, content, trace, fName, ##__VA_ARGS__); \
        return tokens;                                                  \
    }
#define LEX_ERRORF(_ec, message, start, ...)                                   \
    {                                                                          \
        trace.ec = _ec;                                                        \
        *err = rs_error(message, content, trace, fName, start, ##__VA_ARGS__); \
        return tokens;                                                         \
    }
token_list tlex(const std::string &fName, std::string &content, rs_error *err = nullptr)
{
    lex_info LEX_INFO;

    size_t _At = -1;
    size_t S = content.length();

    stack_trace trace(0, std::make_shared<size_t>(_At));
    token_list tokens;

    std::function<char()> adv = [&]() -> char
    {
        // 0
        if (++_At >= S)
            return 0;

        char c = content.at(_At);

        if (c == '\n')
        {
            trace.caret = 0;
            trace.line++;
            trace.nlindex = _At;
        }
        else
            trace.caret++;
        return c;
    };
    char ch;
    while (ch = adv())
    {
        if (ch == '\n')
        {
            trace.line++;
            trace.nlindex = _At;
            trace.caret = 0;
            continue;
        }
        if (ch == '"' || ch == '\'')
        {
            size_t start = _At + 1;
            bool escaped = false;
            while (ch = adv())
            {
                if (ch == '\\')
                    escaped = true;
                else if (!escaped && (ch == '"' || ch == '\''))
                    break;
                else if (escaped)
                    escaped = false;
            }
            if (_At == S)
                LEX_ERRORF(RS_SYNTAX_ERROR, "Unterminated string-literal.", start);

            tokens.push_back(token{content.substr(start, _At - start - 1), token_type::STRING_LITERAL, 0, trace, start});
        }
        else if (std::isdigit(ch))
        {
            size_t start = _At;
            bool decimal = false;

            while (ch = adv())
            {
                if (ch == '.')
                {
                    if (decimal)
                        LEX_ERROR(RS_SYNTAX_ERROR, "Invalid floating point notation.");
                    decimal = true;
                }
                else if (!std::isdigit(ch))
                    break;
            }
            tokens.push_back(token{content.substr(start, _At - start), decimal ? token_type::FLOAT_LITERAL : token_type::INT_LITERAL, 0, trace, start});
            _At--; // go back 1 char
        }
        else if (ch == '_' || ch == '@' || std::isalpha(ch))
        {
            bool isSelectorLiteral = ch == '@';
            size_t start = isSelectorLiteral ? _At + 1 : _At;
            while ((ch = adv()) && (std::isalpha(ch) || ch == '_'));

            token t{content.substr(start, _At - start), isSelectorLiteral ? token_type::SELECTOR_LITERAL : token_type::WORD, 0, trace, start};
            
            _At --; // go back 1 char

            auto keyword = LEX_INFO.keywords.find(t.repr);

            if (keyword != LEX_INFO.keywords.end())
            {
                if (isSelectorLiteral) LEX_ERRORF(RS_SYNTAX_ERROR, "Expected selector literal, not keyword '{}'", start, t.repr);

                auto& tupl = keyword->second;
                t.type = std::get<0>(tupl);
                t.info = std::get<1>(tupl);
            }
            tokens.push_back(t);
        }
        else if (ch == '/' && _At + 1 < S && content.at(_At + 1) == '/')
        {
            adv();
            while(adv() && ch != '\n')
                if (ch == '\\' && _At + 1 < S && content.at(_At + 1) == '\n')
                    LEX_ERROR(RS_SYNTAX_ERROR, "A backslash cannot terminate a single lined comment.");
        }
        else if (ch == '/' && _At + 1 < S && content.at(_At + 1) == '*')
        {
            adv();
            bool found = false;
            while(adv())
            {
                if (ch == '\\')
                    adv();
                else if (ch == '*' && _At + 1 < S && content.at(_At + 1) == '/')
                {
                    found = true;    
                    break;
                }
            }
            if (!found)
                LEX_ERROR(RS_SYNTAX_ERROR, "Unterminated multi-line comment.");
        }
        else
        {
            token_type customType = token_type::SYMBOL;
            switch (ch)
            {
            case '\t':
            case ' ':
                continue;
            case ';':
                customType = token_type::LINE_END;
                break;
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
                customType = token_type::OPERATOR;
                break;
            case '(':
                customType = token_type::BRACKET_OPEN;
                break;
            case ')':
                customType = token_type::BRACKET_CLOSED;
                break;
            case '[':
                customType = token_type::SQBRACKET_OPEN;
                break;
            case ']':
                customType = token_type::SQBRACKET_CLOSED;
                break;
            case '{':
                customType = token_type::CBRACKET_OPEN;
                break;
            case '}':
                customType = token_type::CBRACKET_CLOSED;
                break;
            }

            tokens.push_back(token{std::string(1, ch), customType, (uint32_t)ch, trace});
        }
    }

    return tokens;
}
#undef LEX_ERRORF
#undef LEX_ERROR
