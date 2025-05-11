#include "rbc.hpp"
#include "lang.hpp"

std::shared_ptr<rs_variable> rbc_program::getVariable(const std::string& name)
{
    auto result = std::find_if(globalVariables.begin(), globalVariables.end(),
    [&](std::shared_ptr<rs_variable>& var)
        {return var->name == name;}
    );
    if(result != globalVariables.end()) return *result;
    
    if (!currentFunction) return nullptr;

    result = std::find_if(currentFunction->localVariables.begin(), currentFunction->localVariables.end(),
    [&](std::shared_ptr<rs_variable>& var)
        {return var->name == name;}
    );
    
    if(result != globalVariables.end()) return *result;

    return nullptr;
}

#define COMP_ERROR(_ec, message, ...)                                    \
    {                                                                    \
        *err = rs_error(message, content, current->trace, fName, current->trace.start, ##__VA_ARGS__);  \
        err->trace.ec = _ec;                                                  \
        return program;                                                   \
    }
rbc_program torbc(token_list& tokens, std::string fName, std::string& content, rs_error* err)
{
    rbc_program program(err);
    
    size_t _At = -1;

    // overriding if not set.
    err->trace.at = std::make_shared<size_t>(_At);
    err->content  = std::make_shared<std::string>(content);
    err->fName    = fName;
    
    size_t S   = tokens.size();
    
    if (S == 0) return program;
    
    token* current = &tokens.at(0);

    auto adv     = [&](int i = 1) -> token*
    {
        if (_At + i >= S)
            return nullptr;

        _At += i;
        current = &tokens.at(_At);

        // stack trace needs to be updated here,
        // i.e all tokens should contain a stack trace within them.

        return current;
    };
    auto peek    = [&](int x = 1) -> token*
    {
        if (_At + x >= S)
            return nullptr;

        token* t = &tokens.at(_At + x);

        // stack trace needs to be updated here,
        // i.e all tokens should contain a stack trace within them.

        return t;
    };
    auto match   = [&](token_type tt, int info = -1) -> bool
    {
        token* next = peek();
        if(next && next->type == tt)
        {
            if(info > -1 && next->info != info) return false;

            adv();
            return true;
        }

        return false;
    };
    auto follows = [&](token_type tt, int info = -1) -> token*
    {
        token* next = peek();
        if(next && next->type == tt)
        {
            if(info > -1 && next->info != info) return nullptr;
            current = next;
            _At++;
            return next;
        }
        return nullptr;
    };

    do
    {
        switch(current->type)
        {
        case token_type::WORD:
        {
            token& word = *current;
            if (follows(token_type::SYMBOL))
            {
#pragma region variables
                std::shared_ptr<rs_variable> variable = program.getVariable(word.repr);
                
                switch(current->info)
                {
                case ':': // init variable's type info
                    break;
                case '=': // assign a value to variable
                {
                    if(!variable)
                        variable = std::make_shared<rs_variable>(word, program.currentScope, !program.currentFunction);
                    if(!adv())
                        COMP_ERROR(RS_EOF_ERROR, "Expected expression, not EOF.");
                    
                    rs_expression expr = expreval(program, tokens, _At, err);
                    break;
                }
                case '.': // access variable's contents (only if real_type_info says it's an object)
                    break;
                default:
                    WARN("Unexpected token after variable usage ('%s').", current->repr.c_str());
                }
#pragma endregion variables
            }
            else if (follows(token_type::BRACKET_OPEN))
            {
#pragma region    function_calls
#pragma endregion function_calls
            }
            break;
        }
        case token_type::KW_METHOD:
        {
        }
        }
    } while(adv());

    return program;
}
#undef COMP_ERROR

mc_program tomc(rbc_program&, rs_error*)
{
    return mc_program{};
}