#include "rbc.hpp"
#include "lang.hpp"
#include "lexer.hpp"
#include "file.hpp"
#include "mchelpers.hpp"

#include <regex>

namespace rbc_commands
{
    namespace registers
    {
        rbc_command occupy(std::shared_ptr<rbc_register> reg, rbc_value val)
        {
            reg->vacant = false;
            return rbc_command(rbc_instruction::SAVE, reg, val);
        }
        rbc_command operate(std::shared_ptr<rbc_register> reg, rbc_value val, uint op)
        {
            return rbc_command(rbc_instruction::MATH, reg, val, rbc_constant(token_type::INT_LITERAL, std::to_string(op)));
        }
    }
    namespace variables
    {
        rbc_command set(std::shared_ptr<rs_variable> var, rbc_value val)
        {
            return rbc_command(rbc_instruction::SAVE, rbc_value(var), val);
        }
        rbc_command create(std::shared_ptr<rs_variable> var, rbc_value val)
        {
            return rbc_command(rbc_instruction::CREATE, rbc_value(var), val);
        }
        rbc_command create(std::shared_ptr<rs_variable> var)
        {
            return rbc_command(rbc_instruction::CREATE, rbc_value(var));
        }
    }
}

#pragma region decorators
rbc_function_decorator parseDecorator(const std::string& name)
{
    if (name == "__inbuilt__") return rbc_function_decorator::INBUILT;
    if (name == "__single__")  return rbc_function_decorator::SINGLE;
    if (name == "__cpp__") return rbc_function_decorator::CPP;
    return rbc_function_decorator::UNKNOWN;
}

#pragma endregion decorators
#pragma region operators

void rbc_program::operator()(std::vector<rbc_command>& instructions)
{
    if (currentFunction)
    {
        std::vector<rbc_command>& toAppend = currentFunction->instructions;
        toAppend.insert(toAppend.end(), instructions.begin(), instructions.end());
    }
    else
        globalFunction.instructions.insert(globalFunction.instructions.end(),
                                            instructions.begin(), instructions.end());
}
void rbc_program::operator ()(const rbc_command& instruction)
{
    if (currentFunction) currentFunction->instructions.push_back(instruction);
    else globalFunction.instructions.push_back(instruction);
}

#pragma endregion operators
std::string rbc_function::toStr()
{
    std::stringstream stream;
    stream << "{name=" << name;

    int i = 1;
    for(auto& var : localVariables)
    {
        stream << ',';
        if(var.second.second) // parameter
        {
            stream << 'p' << i << '=' << var.second.first->tostr();
        }
    }
    return stream.str();
}
std::string rbc_function::toHumanStr()
{
    std::stringstream stream;
    stream << name << ':';
    for(auto& instruction : instructions)
    {
        stream << '\n';
        stream << '\t' << instruction.toHumanStr();
    }

    return stream.str();
}
std::string rbc_command::tostr()
{
    std::stringstream stream;
    stream << '{' << static_cast<int>(type);
    for(auto& p : parameters)
    {
        switch(p->index())
        {
            case 0:
                stream << ", " << std::get<0>(*p).tostr();
                break;
            case 1:
                stream << ", " << std::get<1>(*p)->tostr();
                break;
            case 2:
                stream << ", " << std::get<2>(*p)->tostr();
                break;
            case 3:
                stream << " " << std::get<3>(*p)->tostr();
                break;
        }
    }
    stream << '}';
    return stream.str();
}
std::string rbc_command::toHumanStr()
{
    std::stringstream stream;
    switch(type)
    {
        case rbc_instruction::CALL:
            stream << "CALL ";
            break;
        case rbc_instruction::CREATE:
            stream << "CREATE ";
            break;
        case rbc_instruction::DEL:
            stream << "DEL ";
            break;
        case rbc_instruction::INC:
            stream << "INC";
            break;
        case rbc_instruction::DEC:
            stream << "DEC";
            break;
        case rbc_instruction::EQ:
            stream << "EQ ";
            break;
        case rbc_instruction::GT:
            stream << "GT ";
            break;
        case rbc_instruction::IF:
            stream << "IF ";
            break;
        case rbc_instruction::ELSE:
            stream << "ELSE";
            break;
        case rbc_instruction::ENDIF:
            stream << "ENDIF";
            break;
        case rbc_instruction::LT:
            stream << "LT ";
            break;
        case rbc_instruction::MATH:
            stream << "MATH ";
            break;
        case rbc_instruction::NEQ:
            stream << "NEQ ";
            break;
        case rbc_instruction::NIF:
            stream << "NIF ";
            break;
        case rbc_instruction::RET:
            stream << "RET ";
            break;
        case rbc_instruction::SAVE:
            stream << "SAVE ";
            break;
        case rbc_instruction::PUSH:
            stream << "PUSH ";
            break;
        case rbc_instruction::POP:
            stream << "POP ";
            break;
    }
    int c = 0;
    for(auto& p : parameters)
    {
        if (c > 0)
            stream << ", ";
        switch(p->index())
        {
            case 0:
                stream << std::get<0>(*p).tostr();
                break;
            case 1:
                stream << std::get<1>(*p)->tostr();
                break;
            case 2:
                stream << std::get<2>(*p)->tostr();
                break;
            case 3:
                stream << std::get<3>(*p)->tostr();
                break;
        }
        c++;
    }
    return stream.str();
}
std::shared_ptr<rs_variable> rbc_program::getVariable(const std::string& name)
{
    auto result = std::find_if(globalVariables.begin(), globalVariables.end(),
    [&](std::shared_ptr<rs_variable>& var)
        {return var->name == name;}
    );
    if(result != globalVariables.end()) return *result;
    
    if (!currentFunction) return nullptr;

    auto nresult = currentFunction->localVariables.find(name);
    
    if(nresult != currentFunction->localVariables.end()
    && nresult->second.first->scope <= currentScope) return nresult->second.first;

    return nullptr;
}
sharedt<rbc_register> rbc_program::getFreeRegister(bool operable)
{
    for(auto& reg : registers)
    {
        if (reg->vacant)
        {
            if (operable)
                if (reg->operable) return reg;
            else return reg;
        }
    }
    return nullptr;
}
sharedt<rbc_register> rbc_program::makeRegister(bool operable, bool vacant)
{
    uint id = registers.size();
    registers.push_back(std::make_shared<rbc_register>(id, operable, vacant));
    
    return registers[id];
}

#define COMP_ERROR(_ec, message, ...)                                    \
    {                                                                    \
        *err = rs_error(message, content, current->trace, fName, current->trace.start, ##__VA_ARGS__);  \
        err->trace.ec = _ec;                                                  \
        return program;                                                   \
    }
#define COMP_ERROR_R(_ec, message, ret, ...)                                    \
    {                                                                    \
        *err = rs_error(message, content, current->trace, fName, current->trace.start, ##__VA_ARGS__);  \
        err->trace.ec = _ec;                                                  \
        return ret;                                                   \
    }
rbc_program torbc(token_list& tokens, std::string fName, std::string& content, rs_error* err)
{
    rbc_program program(err);
    
    long _At = 0;

    // overriding if not set.
    err->trace.at = std::make_shared<long>(_At);
    err->content  = std::make_shared<std::string>(content);
    err->fName    = fName;
    
    size_t S   = tokens.size();
    
    if (S == 0) return program;
    
    token* current = &tokens.at(0);

    auto resync = [&]() -> bool
    {
        if (_At >= S - 1) return false;

        current = &tokens.at(_At);
        return true;
    };
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

#pragma region variables
    auto typeparse = [&]() -> rs_type_info
    {
        rs_type_info tinfo;
    _get_type:
        token* next = adv();

        if(!next)
            COMP_ERROR_R(RS_EOF_ERROR, "Expected type, not EOF.", tinfo);
        int typeID = next->info;

        // its not any kw
        if(typeID == 0 && next->type != token_type::TYPE_DEF)
        {
            // TODO find type
            auto custom = program.objectTypes.find(next->repr);
            if (custom == program.objectTypes.end())
                COMP_ERROR_R(RS_SYNTAX_ERROR, "Type name unknown or not supported.", tinfo);
            typeID = custom->second->typeID;
        }
        next = peek();
        bool optional = false;
        bool strict = false;
        if(next)
        {
            if (next->info == '?')
            {
                optional = true;
                adv();
            }
            else if (next->info == '!')
            {
                strict   = true;
                adv();
            }
        }

        uint arrayCount = 0;
        while (next = follows(token_type::SQBRACKET_OPEN))
        {
            arrayCount++;
            if(!match(token_type::BRACKET_CLOSED))
                COMP_ERROR_R(RS_SYNTAX_ERROR, "Unclosed type specified array.", tinfo);
        }
        if(tinfo.type_id == -1)
        {
            tinfo.type_id = typeID;
            tinfo.strict = strict;
            tinfo.optional = optional;
            tinfo.array_count = arrayCount;
        }
        else
            tinfo.otherTypes.push_back(rs_type_info{typeID, arrayCount, optional, strict});
        if(!adv())
            COMP_ERROR_R(RS_SYNTAX_ERROR, "Missing semicolon.", tinfo);

        if (current->info == '|') goto _get_type;
        if (current->info == '?' || current->info == '!')
            COMP_ERROR_R(RS_SYNTAX_ERROR, "Invalid type notation.", tinfo);
        return tinfo;
    };
    // must be called at the index of the token after the variable name, ie myVar:int, at the colon.
    auto varparse = [&](token& name, bool needsTermination = true, bool parameter = false, bool obj = false) -> std::shared_ptr<rs_variable>
    {
        if (program.functions.find(name.repr) != program.functions.end()
        || (program.currentFunction && program.currentFunction->name == name.repr))
            COMP_ERROR_R(RS_SYNTAX_ERROR, "The name '{}' already exists as a function.", nullptr, name.repr);
        std::shared_ptr<rs_variable> variable = program.getVariable(name.repr);
        bool exists = (bool)variable;
    _eval:
        switch(current->info)
        {
        case ':': // init variable's type info
        {
            if(exists)
                COMP_ERROR_R(RS_SYNTAX_ERROR, "Variable cannot be reinitialized with a different type.", nullptr);

            rs_type_info type = typeparse();

            if(err->trace.ec)
                return nullptr;

            variable = std::make_shared<rs_variable>(name, program.currentScope, !program.currentFunction);
            variable->type_info = type;
            switch(current->info)
            {
                case ';':
                    break;
                case '=':
                    goto _eval_expr;
                default:
                    if (!needsTermination)
                        break;
                    COMP_ERROR_R(RS_SYNTAX_ERROR, "Expected end of expression.", nullptr);
            }
            break;
        }    
        case '=': // assign a value to variable
        {
        _eval_expr:
            bool needsCreation = !exists;
            if(!variable)
            {
                variable = std::make_shared<rs_variable>(name, program.currentScope, !program.currentFunction);
                needsCreation = true;
            }
            if(!adv())
                COMP_ERROR_R(RS_EOF_ERROR, "Expected expression, not EOF.", nullptr);
            
            rs_expression expr = expreval(program, tokens, _At, err);
            if(err->trace.ec)
                return nullptr;
            variable->value = std::make_shared<rs_expression>(expr);
            // we dont want to create variables defined in an object. We handle that another way.
            if (expr.operation.isSingular() && !obj && !expr.nonOperationalResult)
            {
                auto& value = std::get<token>(*expr.operation.left);

                rbc_value val = value.type == token_type::WORD ? program.getVariable(value.repr) : rbc_value(rbc_constant(value.type, value.repr, &value.trace));
                // no need to evaluate.
                if (needsCreation)
                    program(rbc_commands::variables::create(variable, val));
                else
                    program(rbc_commands::variables::set(variable, val));
            }
            else if (!obj)
            {
                auto result = expr.rbc_evaluate(program, err);
                if(err->trace.ec)
                    return nullptr;
                
                if (needsCreation)
                    program(rbc_commands::variables::create(variable, result));
                else
                    program(rbc_commands::variables::set(variable, result));
            }
            break;
        }      
        case '.': // access variable's contents (only if real_type_info says it's an object)
            break;
        case ';':
            break;
        case ',':
            if (parameter) break;

            break;
        default:
            WARN("Unexpected token after variable usage ('%s').", current->repr.c_str());
        }
        if (!exists && variable && !obj)
        {
            if(!program.currentFunction)
                program.globalVariables.push_back(variable);
            else
                program.currentFunction->localVariables.insert({variable->name, {variable, parameter}});
        }
        return variable;
    };
#pragma endregion variables
#pragma region function_calls
    // must be called at index of open br, example: f() => ( <-
    auto callparse = [&](std::string& name, bool needsTermination = true) -> bool
    {
        auto func = program.functions.find(name);
        bool inbuilt = false;
        bool internal = false;
        if (func == program.functions.end())
            COMP_ERROR_R(RS_SYNTAX_ERROR, "Unknown function name.", false);
        int pc = 0; // param count
        token* start = current;
        auto& decorators = func->second->decorators;
        if(std::find(decorators.begin(), decorators.end(), rbc_function_decorator::INBUILT) != decorators.end())
            inbuilt = true;
        if (std::find(decorators.begin(), decorators.end(), rbc_function_decorator::CPP) != decorators.end())
            internal = true;
        while(current->type != token_type::BRACKET_CLOSED)
        {
            adv();
            rs_expression expr = expreval(program, tokens, _At, err, true, false);
            resync(); // reassign current
            if (expr.nonOperationalResult)
                adv(); // object parsing finishes at }, not: ,
            if(err->trace.ec)
                return false;
            auto result = expr.rbc_evaluate(program, err);
            if(err->trace.ec)
                return false;
            program(rbc_command(rbc_instruction::PUSH, result));

            pc ++;
            if (current->info != ',' && current->type != token_type::BRACKET_CLOSED)
                COMP_ERROR_R(RS_SYNTAX_ERROR, "Unexpected token.", false);
        }
        if(_At >= S)
        {
            // for error checking
            current = start;
            COMP_ERROR_R(RS_SYNTAX_ERROR, "Missing closing bracket | semi-colon.", false);
        }
        // todo handle parameter storing better.
        int actualpc = 0;
        for(auto& var : func->second->localVariables)
        {
            // is param
            if (var.second.second)
            {
                actualpc ++;
            }
        }
        if (actualpc != pc)
            COMP_ERROR_R(RS_SYNTAX_ERROR, "No matching function call with pc of {}", false, pc);
        program(rbc_command(rbc_instruction::CALL, rbc_constant(token_type::STRING_LITERAL, name, &start->trace)));
        if (!internal)
            for(int i = 0; i < pc; i++)
                program(rbc_command(rbc_instruction::POP));


        if (needsTermination && adv() && current->type != token_type::LINE_END)
            COMP_ERROR_R(RS_SYNTAX_ERROR, "Missing semi-colon.", false);
        return true;
    };
#pragma endregion function_calls
#pragma region objects
    // must be called at the index of opening bracket
    auto objparse = [&](std::string& name) -> std::shared_ptr<rs_object>
    {
        if (current->type != token_type::CBRACKET_OPEN)
            COMP_ERROR_R(RS_SYNTAX_ERROR, "Expected object body.", nullptr);
        rs_object obj{name};
        rs_object_member_decorator decorator = rs_object_member_decorator::OPTIONAL;
        while(adv() && current->type != token_type::CBRACKET_CLOSED)
        {
            // dont append to global scope
            token* name;
            token_type& t = current->type;
            switch(t)
            {
                case token_type::WORD:
                    name = current;
                    break;
                case token_type::KW_OPTIONAL:
                case token_type::KW_REQUIRED:
                case token_type::KW_SEPERATE:
                {
                    if (t == token_type::KW_REQUIRED)
                        decorator = rs_object_member_decorator::REQUIRED;
                    else if (t == token_type::KW_SEPERATE)
                        decorator = rs_object_member_decorator::SEPERATE;
                    
                    if (!adv() || current->type != token_type::WORD)
                        COMP_ERROR_R(RS_SYNTAX_ERROR, "Expected variable definition.", nullptr);
                    name = current;
                    break;
                }
                default:
                    COMP_ERROR_R(RS_SYNTAX_ERROR, "Unexpected token.", nullptr);
            }
            adv();
            auto variable = varparse(*name, true, false, true);
            if(err->trace.ec)
                return nullptr;
            if (obj.members.find(variable->name) != obj.members.end())
                COMP_ERROR_R(RS_SYNTAX_ERROR, "Duplicate object member name.", nullptr);

            obj.members.insert({variable->name, {*variable, decorator}});
        }
        if (_At == S - 1)
            COMP_ERROR_R(RS_EOF_ERROR, "Unterminated object body.", nullptr);
        return std::make_shared<rs_object>(obj);
    };
#pragma endregion objects
    do
    {
        switch(current->type)
        {
        case token_type::WORD:
        {
            token& word = *current;
            if (follows(token_type::SYMBOL))
            {
                if (!varparse(word))
                    return program;
            }
            else if (follows(token_type::BRACKET_OPEN))
            {
                if(!callparse(word.repr))
                    return program;
            }
            break;
        }
        case token_type::KW_METHOD:
        {
#pragma region function_definitions
            if (!adv())
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected name, not EOF.");
            
            rs_type_info retType;
            std::string name;
            if (current->info == ':')
            {
                // we are defining the return type
                retType = typeparse();
                if (err->trace.ec)
                    return program;
                name = current->repr;
            }
            else if(current->type == token_type::WORD)
                name = current->repr;
            else
                COMP_ERROR(RS_SYNTAX_ERROR, "Invalid function name.");
            
            if (!adv())
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected function definition, not EOF.");  
            
            if(current->type != token_type::BRACKET_OPEN)
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected '('.");
            
            program.currentFunction = std::make_shared<rbc_function>(name);
            program.currentFunction->returnType = std::make_shared<rs_type_info>(retType);
            program.scopeStack.push(rbc_scope_type::FUNCTION);
            program.currentScope ++;

            do
            {
                adv();
                switch(current->type)
                {
                    case token_type::WORD:
                    {
                        token& varName = *current;
                        if (!adv())
                            COMP_ERROR(RS_EOF_ERROR, "Unexpected EOF.");
                        // false as we do not need to terminate variable usage with ; or =
                        if(!varparse(varName, false, true)) // varparse adds any instructions to program.currentFunction
                            return program;
                        break;
                    }
                    default:
                        COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected token.");
                }
            }
            while(current->type != token_type::BRACKET_CLOSED);
                
                
            if(_At >= S)
                COMP_ERROR(RS_SYNTAX_ERROR, "Missing function definition or semi-colon.");
            
            while (adv() && current->type == token_type::WORD)
            {
                const std::string& dName = current->repr;

                auto decorator = parseDecorator(dName);

                if(decorator == rbc_function_decorator::UNKNOWN)
                    COMP_ERROR(RS_SYNTAX_ERROR, "Unknown function decorator: '%s'.", dName);

                auto& decorators = program.currentFunction->decorators;
                if (std::find(decorators.begin(), decorators.end(), decorator) != decorators.end())
                    COMP_ERROR(RS_SYNTAX_ERROR, "Duplicate function decorator: '%s'.", dName);

                decorators.push_back(decorator);
            }
            if(_At >= S)
                COMP_ERROR(RS_SYNTAX_ERROR, "Missing function definition or semi-colon.");
            switch(current->type)
            {
                case token_type::CBRACKET_OPEN:
                {
                    // break, hand over parsing to main loop, scope is incremented earlier for parameters. it will decrement when it reaches
                    // a closing curly brace.
                    break;
                }
                case token_type::LINE_END:
                {
                    program.currentFunction->hasBody = false;
                    goto _decrement_scope;
                }
                default:
                    COMP_ERROR(RS_SYNTAX_ERROR, "Expected function definition or semi-colon.");
            }
            break;
#pragma endregion
        }
        case token_type::CBRACKET_CLOSED:
        {
        _decrement_scope:
            if(program.scopeStack.empty())
                COMP_ERROR(RS_SYNTAX_ERROR, "Unmatched closing bracket.");
            rbc_scope_type scope = program.scopeStack.top();
            program.scopeStack.pop();
            program.lastScope = scope;
            switch(scope)
            {
                case rbc_scope_type::FUNCTION:
                {
                    if (!program.currentFunction)
                        COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected token.");

                    // TODO: should functions have global scope access? if so, we need to keep track of when they are created.
                    // I think not.
                    program.functions.insert({program.currentFunction->name, program.currentFunction});
                    program.currentFunction = nullptr;
                    break;
                }
                case rbc_scope_type::IF:
                case rbc_scope_type::ELIF:
                {
                    token* next = peek();
                    if (!next || (next->type != token_type::KW_ELSE && next->type != token_type::KW_ELIF))
                        program(rbc_command(rbc_instruction::ENDIF));
                    break;
                }
                case rbc_scope_type::ELSE:
                {
                    token* next = peek();
                    if (next && (next->type == token_type::KW_ELSE || next->type == token_type::KW_ELIF))
                        COMP_ERROR(RS_SYNTAX_ERROR, "Else & Elif blocks cannot follow an else block.");
                    program(rbc_command(rbc_instruction::ENDIF));
                    break;
                }
                case rbc_scope_type::NONE:
                {
                    program(rbc_command(rbc_instruction::DEC));
                    break;
                }
            }
            program.currentScope--;
            if (program.currentScope < 0)
                COMP_ERROR(RS_SYNTAX_ERROR, "Unmatched closing bracket.");
            break;
        }
        case token_type::CBRACKET_OPEN:
        {
            program.scopeStack.push(rbc_scope_type::NONE);
            program(rbc_command(rbc_instruction::INC));
            program.currentScope ++;
            break;
        }
        case token_type::KW_RETURN:
        {
            if (!program.currentFunction)
                COMP_ERROR(RS_SYNTAX_ERROR, "Return statements can only exist inside a function.");
            
            if (!adv())
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected expression.");
            
            if (current->type == token_type::LINE_END)
            {
                program(rbc_command(rbc_instruction::RET));
                break;
            }

            // possible expression OR function call.
            token& start = *current;

            if(follows(token_type::BRACKET_OPEN))
            {
                // function call TODO
                if(!callparse(start.repr))
                    return program;
            }
            else
            {
                rs_expression expr = expreval(program, tokens, _At, err);
                if(err->trace.ec)
                    return program;
                auto result = expr.rbc_evaluate(program, err); // evaluate and compute return statement
                if(err->trace.ec)
                    return program;
                program(rbc_command(rbc_instruction::RET, result));
            }
            break;
        }
        case token_type::KW_IF:
        {
            if (!adv() || current->type != token_type::BRACKET_OPEN)
                COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected token.");
            
            if(!adv())
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected expression, not EOF.");
            rs_expression left = expreval(program, tokens, _At, err, false, false);
            if(err->trace.ec)
                return program;
            rbc_value lVal = left.rbc_evaluate(program, err);
            if(err->trace.ec)
                return program;
            
            resync();
            token& op         = *current;
            token_type compop = op.type;   
            resync();
            if(compop != token_type::COMPARE_EQUAL && compop != token_type::COMPARE_NOTEQUAL)
                COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected token.");

            if(!adv())
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected expression, not EOF.");
            // br = true as if we hit the closing bracket of the if statement we should return.
            rs_expression right = expreval(program, tokens, _At, err, true, false);
            if(err->trace.ec)
                return program;
            rbc_value rVal = right.rbc_evaluate(program, err);
            if(err->trace.ec)
                return program;
            resync();
            if (current->type != token_type::BRACKET_CLOSED)
                COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected token.");
            
            program(rbc_command(rbc_instruction::IF, lVal, rbc_constant(compop, op.repr, &op.trace), rVal));
            
            if (!adv())
                COMP_ERROR(RS_EOF_ERROR, "Unexpected EOF.");

            if (current->type != token_type::CBRACKET_OPEN)
                COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected token.");
            
            program.currentScope++;
            program.scopeStack.push(rbc_scope_type::IF);
            break;
        }
        case token_type::KW_ELSE:
        {
            if (program.lastScope != rbc_scope_type::IF && program.lastScope != rbc_scope_type::ELIF)
                COMP_ERROR(RS_SYNTAX_ERROR, "Else and elif blocks can only be used after an if block.");
            

            if (!adv() || current->type != token_type::CBRACKET_OPEN)
                COMP_ERROR(RS_SYNTAX_ERROR, "Expected else block.");
            
            program.scopeStack.push(rbc_scope_type::ELSE);
            program(rbc_command(rbc_instruction::ELSE));
            program.currentScope++;
            break;
        }
        case token_type::TYPE_DEF:
        {
            switch (current->info)
            {
                case RS_OBJECT_KW_ID:
                {
                    if (!adv())
                        COMP_ERROR(RS_EOF_ERROR, "Unexpected EOF.");
                    
                    std::string& name = current->repr;

                    if (current->type != token_type::WORD)
                        COMP_ERROR(RS_SYNTAX_ERROR, "Unexpected keyword.");

                    if (program.objectTypes.find(name) != program.objectTypes.end())
                        COMP_ERROR(RS_SYNTAX_ERROR, "Object with name already exists.");
                    
                    adv();

                    auto obj = objparse(name);
                    if (err->trace.ec)
                        return program;
                    obj->typeID = rs_object::TYPE_CARET_START + program.objectTypes.size();
                    program.objectTypes.insert({name, obj});
                    break;
                }
                default:
                    WARN("Unimplemented type.");
                    break;
            }
            
            break;
        }
        }
    } while(adv());

    return program;
}
void preprocess(token_list& tokens, std::string fName, std::string& content, rs_error* err,
                std::shared_ptr<std::vector<std::filesystem::path>> visited)
{
    long         _At = 0;
    const size_t S   = tokens.size();
    std::filesystem::path rootPath = std::filesystem::absolute(fName);

    if(!visited)
        visited = std::make_shared<std::vector<std::filesystem::path>>();

    do
    {
        token* current = &tokens.at(_At);

        switch(current->type)
        {
            case token_type::KW_USE:
            {
                if (_At + 1 >= S) 
                    COMP_ERROR_R(RS_SYNTAX_ERROR, "Expected file to import, not EOF.",);
                
                token& path = tokens.at(++_At);
                std::string file = (std::regex_replace(path.repr, std::regex("\\."), "/") + ".rsc");
                std::filesystem::path filePath = rootPath.parent_path() / file;
                if (visited && std::find(visited->begin(), visited->end(), filePath) != visited->end())
                    COMP_ERROR_R(RS_ALREADY_INCLUDED_ERROR, "This file has already been included.",);
                visited->push_back(filePath);
                std::string fileContent = readFile(filePath);

                if (fileContent.empty())
                {
                    if (RS_CONFIG.exists("lib"))
                    {
                        std::filesystem::path libPath = std::filesystem::absolute(RS_CONFIG.get<std::string>("lib"));
                        filePath = libPath / file;

                        fileContent = readFile(filePath);

                        if (fileContent.empty())
                            COMP_ERROR_R(RS_SYNTAX_ERROR, "Could not find import '{}'.", , path.repr);
                    }
                    else
                        COMP_ERROR_R(RS_SYNTAX_ERROR, "Could not find import '{}'.", , path.repr);
                }
                if (path.type != token_type::WORD)
                    COMP_ERROR_R(RS_SYNTAX_ERROR, "Expected file name.",);

                if (_At + 1 >= S || tokens.at(++_At).type != token_type::LINE_END)
                    COMP_ERROR_R(RS_SYNTAX_ERROR, "Missing semicolon.",);
                std::string filePathStr = filePath.string();
                token_list fileTokens = tlex(filePathStr, fileContent, err);

                preprocess(fileTokens, filePathStr, fileContent, err, visited);

                if(err->trace.ec)
                    return;
                const size_t offset = fileContent.length() + 1; // + 1 for \n
                // const auto   lines  = std::count(fileContent.begin(), fileContent.end(), '\n');
                for(size_t i = 0; i < tokens.size(); i++)
                {
                    raw_trace_info& trace = tokens.at(i).trace;

                    trace.at += offset;
                    trace.nlindex += offset;
                }
                tokens.insert(tokens.begin(), fileTokens.begin(), fileTokens.end());

                content = fileContent + '\n' + content;
                _At += fileTokens.size();

                break;
            }
        }
    } while(++_At < S);
}
#define RS_ASSERTC(C, m) if (!(C)) {err=m;return mcprogram;}
#define RS_ASSERT_SIZE(C) RS_ASSERTC(C, "Invalid byte code parameter count. This error is a bug, flag it on github.")
#define RS_ASSERT_SUCCESS if (!err.empty()) {return mcprogram;}
mc_program tomc(rbc_program& program, std::string& err)
{
    mc_program mcprogram;
    conversion::CommandFactory factory(mcprogram, program);
    
    factory.initProgram();

    try{
        for(size_t i = 0; i < program.globalFunction.instructions.size(); i++)
        {

            auto& instruction = program.globalFunction.instructions.at(i);
            const size_t size = instruction.parameters.size();

            switch(instruction.type)
            {
                case rbc_instruction::CREATE:
                {
                    RS_ASSERT_SIZE(size > 0);
                    rs_variable& var = *std::get<sharedt<rs_variable>>(*instruction.parameters.at(0));
                    if (instruction.parameters.size() == 1)
                        factory.createVariable(var);
                    else
                    {
                        rbc_value& val = *instruction.parameters.at(1);
                        factory.createVariable(var, val);
                    }
                    // RS_ASSERT_SUCCESS;
                    break;
                }
                case rbc_instruction::SAVE:
                {
                    RS_ASSERT_SIZE(size == 2);
                    rbc_value& reg = *instruction.parameters.at(0);
                    switch(reg.index())
                    {
                        // register
                        case 1:
                        {
                            factory.setRegisterValue(*std::get<sharedt<rbc_register>>(reg),
                                                      *instruction.parameters.at(1));
                            break;
                        }
                        case 2:
                        {
                            factory.setVariableValue(*std::get<sharedt<rs_variable>>(reg), *instruction.parameters.at(1));
                        }
                    }
                    break;
                }
                case rbc_instruction::MATH:
                {
                    RS_ASSERT_SIZE(size > 2);
                    rbc_constant& val = std::get<rbc_constant>(*instruction.parameters.at(2));
                    bst_operation_type operation = bst_operation_type::NONE;
                    int operatorID = std::stoi(val.val);
                    switch(operatorID)
                    {
                        case 0:
                            operation = bst_operation_type::ADD;
                            break;
                        case 1:
                            operation = bst_operation_type::SUB;
                            break;
                        case 2:
                            operation = bst_operation_type::MUL;
                            break;
                        case 3:
                            operation = bst_operation_type::DIV;
                            break;
                        case 4:
                            operation = bst_operation_type::MOD;
                            break;
                        case 5:
                            operation = bst_operation_type::XOR;
                            break;
                        case 6:
                            operation = bst_operation_type::POW;
                            break;
                    }
                    factory.math(*instruction.parameters.at(0), *instruction.parameters.at(1), operation); 
                    break;
                }
                case rbc_instruction::CALL:
                {
                    RS_ASSERT_SIZE(size > 0);
                    rbc_constant& name = std::get<rbc_constant>(*instruction.parameters.at(0));
                    rbc_function& func = *program.functions.find(name.val)->second;

                    if (std::find(func.decorators.begin(), func.decorators.end(), rbc_function_decorator::CPP) != func.decorators.end())
                    {
                        long caret = i;
                        std::vector<rbc_value> parameters;
                        rbc_command* cmd;
                        while(--caret >= 0 && (cmd = &program.globalFunction.instructions.at(caret))->type == rbc_instruction::PUSH)
                        {
                            parameters.push_back(*cmd->parameters.at(0));
                            factory.pop_back(); // remove all parsed commands to push parameters onto stack. Bad?
                        }
                        std::vector<rbc_value> reversed;
                        reversed.reserve(parameters.size());

                        for (auto it = parameters.rbegin(); it != parameters.rend(); ++it) {
                            reversed.push_back(std::move(*it));
                        }
                        parameters = std::move(reversed);
                        auto decl = inb_impls::INB_IMPLS_MAP.find(name.val);
                        if (decl == inb_impls::INB_IMPLS_MAP.end())
                        {
                            err = "Fatal: inbuilt (__cpp__ decl) c++ function mapping for '" + name.val + "' doesn't exist. This could be due to a mismatch in versions.";
                            return mcprogram;
                        }
                        decl->second(program, factory, parameters, err);
                        if (!err.empty())
                            return mcprogram; // todo can printerr here!!!
                    }
                    else
                        factory.invoke(func);

                    while (i + 1 < program.globalFunction.instructions.size() && program.globalFunction.instructions.at(i + 1).type == rbc_instruction::POP)
                    {
                        i++;
                        factory.popParameter();
                    }

                    // todo
                    break;
                }
                case rbc_instruction::PUSH:
                {
                    RS_ASSERT_SIZE(size == 1);
                    factory.pushParameter(*instruction.parameters.at(0));
                    break;
                }
            }
        }
        mcprogram.globalFunction.commands = factory.package();
    } catch (std::exception& e)
    {
        err = std::string("Internal error: ") + e.what();
        return mcprogram;
    }
    return mcprogram;
}
namespace conversion
{
    void                  CommandFactory::initProgram      ()
    {
        // ROOT
        create_and_push(MC_DATA_CMD_ID, MC_DATA(merge storage, RS_PROGRAM_DATA_DEFAULT));

        // OPERABLE REGISTERS
        size_t operableRegisterCount = 0;
        for(auto& reg : rbc_compiler.registers)
        {
            if (reg->operable)
            {
                create_and_push(MC_SCOREBOARD_CMD_ID, MC_CREATE_OPERABLE_REG(operableRegisterCount, "dummy"));
                operableRegisterCount++;
            }
            else
                WARN("Non operable register not created.");
        }
    }
    CommandFactory::_This CommandFactory::pushParameter    (rbc_value& val)
    {
        switch(val.index())
        {
            case 0:
            {
                rbc_constant& c = std::get<rbc_constant>(val);
                c.quoteIfStr();
                create_and_push(MC_DATA_CMD_ID, MC_STACK_PUSH_CONST(c.val));
                break;
            }
            case 1:
            {
                rbc_register& reg = *std::get<sharedt<rbc_register>>(val);
                add(getRegisterValue(reg).storeResult(
                    PADR(storage) RS_PROGRAM_DATA SEP RS_PROGRAM_STACK
                ));
                break;
            }
            case 2:
            {
                rs_variable& var = *std::get<sharedt<rs_variable>>(val);
                appendStorage(RS_PROGRAM_STORAGE SEP RS_PROGRAM_STACK, MC_VARIABLE_VALUE(var.comp_info.varIndex));
                break;
            }
        }
        return THIS;
    }
    CommandFactory::_This CommandFactory::invoke           (rbc_function& func)
    {
        // TODO: MACROS & NAMESPACES
        create_and_push(MC_FUNCTION_CMD_ID, func.name);
        return THIS;
    }
    CommandFactory::_This CommandFactory::popParameter     ()
    {
        create_and_push(MC_DATA_CMD_ID, MC_DATA(remove storage, RS_PROGRAM_STACK "[-1]"));
        return THIS;
    }
    mc_command            CommandFactory::getRegisterValue (rbc_register& reg)
    {
        if (reg.operable)
            return mc_command(false, MC_SCOREBOARD_CMD_ID, MC_OPERABLE_REG_GET(reg.id));
        return mc_command(false, MC_DATA_CMD_ID, MC_NOPERABLE_REG_GET(reg.id));
    }
    CommandFactory::_This CommandFactory::setVariableValue (rs_variable& var, rbc_value& val)
    {
        switch(val.index())
        {
            // constant
            case 0:
            {
                rbc_constant& c = std::get<0>(val);
                c.quoteIfStr();
                create_and_push(MC_DATA_CMD_ID, MC_VARIABLE_SET_CONST(var.comp_info.varIndex, c.val));
                break;
            }
            default:
                ERROR("Unsupported SAVE operation. TODO implement!");
        }
        return THIS;
    }
    CommandFactory::_This CommandFactory::setRegisterValue (rbc_register& reg, rbc_value& value)
    {
        switch(value.index())
        {
            case 0:
            {
                rbc_constant& val = std::get<rbc_constant>(value);
                val.quoteIfStr();
                if (reg.operable)
                    create_and_push(MC_SCOREBOARD_CMD_ID, MC_OPERABLE_REG_SET(reg.id, val.val));
                else
                    create_and_push(MC_DATA_CMD_ID, MC_DATA(modify storage, ARR_AT(RS_PROGRAM_REGISTERS, STR(reg.id)))
                                                            PAD(set value) INS_L(val.val));
                break;
            }
            case 1:
            {
                // TODO
                break;
            }
            case 2:
            {
                rs_variable& var = *std::get<2>(value);

                if (reg.operable)
                {
                    mc_command cmd = CommandFactory::getVariableValue(var).storeResult(
                        PADR(score) MC_OPERABLE_REG(INS_L(STR(reg.id)))
                    );
                    
                    add(cmd);
                }
                else
                {
                    ERROR("Unsupported! TODO FIX");
                }
            }
        }
        return THIS;
    }
    mc_command            CommandFactory::getVariableValue (rs_variable& var)
    {
        return mc_command(false, MC_DATA_CMD_ID, MC_GET_VARIABLE_VALUE(var.comp_info.varIndex));
    }
    void                  CommandFactory::make             (mc_command& cmd)
    {
        if (!context.lastIfStatement)
            return;
        else
        {
            // depending if last comparison was == or !=
            // TODO
        }

    }
    CommandFactory::_This CommandFactory::appendStorage    (const std::string& dest, const std::string& _const)
    {
        create_and_push(MC_DATA_CMD_ID, MC_DATA(modify storage, INS(dest)) PAD(append value) INS_L(_const));
        return THIS;
    }
    CommandFactory::_This CommandFactory::copyStorage      (const std::string& dest, const std::string& src)
    {
        create_and_push(MC_DATA_CMD_ID, MC_DATA(modify storage, INS(dest))
                                            SEP
                                        MC_DATA(set from storage, INS_L(src)));
        return THIS;
    }
    CommandFactory::_This CommandFactory::createVariable   (rs_variable& var)
    {
        create_and_push(MC_DATA_CMD_ID,
                MC_DATA(modify storage, RS_PROGRAM_VARIABLES)
                    PAD(append value)
                MC_VARIABLE_JSON_DEFAULT(std::to_string(var.scope),
                                        std::to_string(var.type_info.type_id))
                        );
        var.comp_info.varIndex = context.varStackCount++;
        return THIS;
    }
    CommandFactory::_This CommandFactory::createVariable   (rs_variable& var, rbc_value& val)
    {
        switch(val.index())
        {
            case 0:
            {
                var.comp_info.varIndex = context.varStackCount++;

                rbc_constant& c = std::get<0>(val);
                c.quoteIfStr();
                create_and_push(MC_DATA_CMD_ID,
                    MC_DATA(modify storage, RS_PROGRAM_VARIABLES)
                        PAD(append value)
                    MC_VARIABLE_JSON_VAL(c.val, std::to_string(var.scope),
                                                std::to_string(var.type_info.type_id))
                                );
                break;
            }
            case 1:
            {
                // var.comp_info.varIndex = context.varStackCount++;
                // handled in create variable
                sharedt<rbc_register>& reg = std::get<1>(val);
                createVariable(var);
                add( getRegisterValue(*reg).storeResult(PADR(storage) MC_VARIABLE_VALUE_FULL(var.comp_info.varIndex), "int", 1) );
                
                break;
            }
            case 2:
            {
                // handled in create variable
                sharedt<rs_variable>& variable = std::get<2>(val);
                createVariable(var);
                copyStorage(MC_VARIABLE_VALUE(var.comp_info.varIndex), MC_VARIABLE_VALUE(variable->comp_info.varIndex));
                break;
            }
            case 3:
            {
                // TODO
                break;
            }
        }
        return THIS;
    }
    CommandFactory::_This CommandFactory::op_reg_math      (rbc_register& reg, rbc_value& val, bst_operation_type t)
    {
        switch(val.index())
        {
            case 0:
            {
                rbc_constant& c = std::get<0>(val);
                c.quoteIfStr();
                // we can add/subtract constants easily using scoreboard add/remove.
                // with other operations however, we cant, and need to store this constant in the next free register.
                
                if (t == bst_operation_type::ADD)
                {
                    create_and_push(MC_SCOREBOARD_CMD_ID, MC_REG_INCREMENT_CONST(reg.id, c.val));
                    return THIS;
                }
                else if (t == bst_operation_type::SUB)
                {
                    create_and_push(MC_SCOREBOARD_CMD_ID, MC_REG_DECREMENT_CONST(reg.id, c.val));
                    return THIS;
                }
                sharedt<rbc_register> rhReg = rbc_compiler.getFreeRegister(true);
                if (!rhReg)
                    rhReg = rbc_compiler.makeRegister(true);
                setRegisterValue(*rhReg, val);
                const std::string opStr = operationTypeToStr(t) + '=';
                switch(t)
                {
                    case bst_operation_type::MUL:
                    case bst_operation_type::DIV:
                    case bst_operation_type::MOD:
                    case bst_operation_type::XOR:
                    {
                        create_and_push(MC_SCOREBOARD_CMD_ID, MC_REG_OPERATE(reg.id, opStr, rhReg->id));
                        break;
                    }
                    default:
                        ERROR("Unknown/Unsupported math operation between register and constant.");
                }
                break;
            }
            case 1:
            {
                break;
            }
            case 2:
            {
                break;
            }
            default:
                ERROR("Register cannot store unsupported value.");
                return THIS;
        }
        return THIS;
    }
    CommandFactory::_This CommandFactory::math             (rbc_value& lhs, rbc_value& rhs, bst_operation_type op)
    {

        switch(lhs.index())
        {
            case 1:
            {
                rbc_register& reg = *std::get<1>(lhs);

                return reg.operable ? op_reg_math(reg, rhs, op) : nop_reg_math(reg, rhs, op);
            }
            case 2:
            {
                switch(rhs.index())
                {
                    case 1:
                    {
                        rbc_register& reg  = *std::get<1>(rhs);
                        return reg.operable ? op_reg_math(reg, lhs, op) : nop_reg_math(reg, lhs, op);
                    }
                    default:
                        ERROR("Constant operation is unsupported here.");
                        return THIS;
                }
            }
            default:
                ERROR("Constant operation is unsupported here.");
                return THIS;
        }
    }
}


#undef COMP_ERROR
#undef COMP_ERROR_R