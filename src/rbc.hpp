#pragma once
#include <vector>
#include <functional>
#include <any>
#include <memory>
#include <unordered_map>

struct rs_variable;

#include "mc.hpp"
#include "token.hpp"
#include "error.hpp"
#include "util.hpp"

enum class rbc_instruction
{
    CALL,
    SAVE,
    DEL,
    EQ,
    NEQ,
    GT,
    LT,
    IF,
    NIF,
    RET,
};

class rbc_value
{
public:
    const uint32_t val_type; 
    std::any       val;
    const uint32_t encapsulator_type = 0;

    rbc_value(uint32_t _encapsulator_type, uint32_t _val_type = 0, std::any _val = NULL)
        : val_type(_val_type), encapsulator_type(_encapsulator_type), val(_val)
    {}
};
class rbc_register : public rbc_value
{
#define REGISTER_ENCAPSULATOR_ID 1
#define RBC_REGISTER_PLAYER "_CPU"
    public:
    bool operable;
    bool vacant = true;
    std::string name;

    rbc_register(std::string _name, bool _operable, bool _vacant)
        : name(_name), operable(_operable), vacant(_vacant), rbc_value(REGISTER_ENCAPSULATOR_ID)
    {}
};


struct rbc_command
{
    rbc_instruction type;
    std::vector<std::unique_ptr<rbc_value>> parameters;

    template<typename... _RBCValues>
    rbc_command(rbc_instruction _type, _RBCValues&&... values)
        : type(_type)
    {
        (parameters.emplace_back(make_value_ptr(std::forward<_RBCValues>(values))), ...);
    }

private:
    template<typename T>
    static std::unique_ptr<rbc_value> make_value_ptr(T&& value)
    {
        using U = std::decay_t<T>;
        static_assert(std::is_base_of_v<rbc_value, U>, "All parameters must inherit from rbc_value");
        return std::make_unique<U>(std::forward<T>(value));
    }
};

struct rbc_function
{
    std::string name;
    std::vector<std::shared_ptr<rs_variable>> localVariables;
    std::vector<rbc_command> instructions;
};

struct rbc_program
{
    rs_error* context;
    uint32_t  currentScope = 0;
    std::vector<std::shared_ptr<rs_variable>> globalVariables;
    std::unordered_map<std::string, std::shared_ptr<rbc_function>> functions;
    std::shared_ptr<rbc_function> currentFunction = nullptr;

public:
    std::shared_ptr<rs_variable> getVariable(const std::string& name);
    
};

rbc_program torbc(token_list&, std::string, std::string&, rs_error*);

mc_program tomc(rbc_program&, rs_error*);