#pragma once
#include <vector>
#include <functional>
#include <any>
#include <memory>
#include <unordered_map>
#include <stack>
#include <filesystem>
#include <cstdint>

struct rs_variable;
struct rs_type_info;
struct rs_object;

typedef unsigned int uint;

#include "globals.hpp"
#include "mc.hpp"
#include "token.hpp"
#include "error.hpp"
#include "util.hpp"
#include "inb.hpp"

enum class rbc_instruction
{
    CREATE,
    CALL,
    SAVE,
    MATH,
    DEL,
    EQ,
    NEQ,
    GT,
    LT,
    IF,
    NIF,
    ENDIF,
    ELSE,
    RET,
    PUSH,
    POP,
    INC, // inc scope
    DEC  // dec scope
};
enum class rbc_scope_type
{
    IF,
    ELIF,
    ELSE,
    FUNCTION,
    NONE
};



#define RBC_CONST_INT_ID 0
#define RBC_CONST_STR_ID 1
#define RBC_CONST_FLOAT_ID 2
#define RBC_CONST_LIST_ID 3
enum class rbc_value_type
{
    CONSTANT,
    NBT,
    OREG,
    N_OREG,
    OBJECT
};
enum class rbc_function_decorator
{
    INBUILT, // inbuilt functions
    SINGLE,  // functions with 1 single call, redundant to compile.
    CPP,
    UNKNOWN
};

rbc_function_decorator parseDecorator(const std::string&);

class rbc_constant
{
public:
    void quoteIfStr()
    {
        if (val_type == token_type::STRING_LITERAL)
            val = '"' + val + '"';
    }
    const token_type val_type; 
    std::string       val;
    raw_trace_info* trace = nullptr;
    rbc_constant(token_type _val_type, std::string _val)
        : val_type(_val_type), val(_val)
    {
    }
    rbc_constant(token_type _val_type, std::string _val, raw_trace_info* _trace)
        : val_type(_val_type), val(_val), trace(_trace)
    {
    }
    inline std::string tostr()
    {
        return "(const){T=" + std::to_string(static_cast<uint>(val_type)) + ", v=" + val + '}'; 
    }
};
class rbc_register
{
public:
    bool operable;
    bool vacant = true;
    uint id;

    rbc_register(uint _id, bool _operable, bool _vacant)
        : id(_id), operable(_operable), vacant(_vacant)
    {}
    inline void free()
    {
        vacant = true;
    }
    inline std::string tostr()
    {
        return "(reg){(id=" + std::to_string(id) + ") op=" + std::to_string(operable) + ", vacant=" + std::to_string(vacant) + '}'; 
    }
};
template<typename _T>
using sharedt = std::shared_ptr<_T>;
typedef std::variant<rbc_constant, sharedt<rbc_register>, sharedt<rs_variable>, sharedt<rs_object>> rbc_value;
struct rbc_command
{
    rbc_instruction type;
    std::vector<std::shared_ptr<rbc_value>> parameters;

    template<typename... _RBCValues>
    rbc_command(rbc_instruction _type, _RBCValues&&... values)
        : type(_type)
    {
        (parameters.emplace_back(std::make_shared<rbc_value>(std::forward<_RBCValues>(values))), ...);
    }

    // defined outside due to forward decl
    std::string tostr();
    std::string toHumanStr();
    
};

typedef std::pair<std::shared_ptr<rs_variable>, bool> rbc_func_var_t;

struct raw_rbc_function
{
    std::unordered_map<std::string, rbc_func_var_t> localVariables;
    std::vector<rbc_command> instructions;
};
struct rbc_function
{
    std::string name;
    std::unordered_map<std::string, rbc_func_var_t> localVariables;
    std::vector<rbc_command> instructions;
    std::vector<rbc_function_decorator> decorators;
    // made shared because of forward declaration
    std::shared_ptr<rs_type_info> returnType;
    

    bool hasBody = true;

    std::string toStr();
    std::string toHumanStr();
};

struct rbc_program
{
    rs_error* context;
    uint32_t  currentScope = 0;
    std::stack<rbc_scope_type> scopeStack;
    std::vector<std::shared_ptr<rs_variable>> globalVariables;
    std::unordered_map<std::string, std::shared_ptr<rs_object>> objectTypes;
    std::unordered_map<std::string, std::shared_ptr<rbc_function>> functions;
    std::shared_ptr<rbc_function> currentFunction = nullptr;
    std::vector<std::shared_ptr<rbc_register>> registers;
    raw_rbc_function globalFunction;
    rbc_scope_type lastScope;
public:
    sharedt<rs_variable> getVariable(const std::string& name);
    sharedt<rbc_register> getFreeRegister(bool operable = false);
    sharedt<rbc_register> makeRegister(bool operable = false, bool vacant = true);

    void operator ()(std::vector<rbc_command>& instructions);
    void operator ()(const rbc_command& instruction);
    template<typename First, typename Second, typename... _Command>
    inline void operator ()(First&& f, Second&& s, _Command&&... commands)
    {
        operator()(f); operator()(s);
        (operator()(commands), ...);
    }

};

namespace rbc_commands
{
    namespace registers
    {
        rbc_command occupy(std::shared_ptr<rbc_register> reg, rbc_value val);
        // needs register copies to not seg fault when converting to variant.
        rbc_command operate(std::shared_ptr<rbc_register> reg, rbc_value val, uint op);
    };
    namespace variables
    {
        rbc_command create(std::shared_ptr<rs_variable> v, rbc_value val);
        rbc_command create(std::shared_ptr<rs_variable> v);
        rbc_command set(std::shared_ptr<rs_variable> v, rbc_value val);
    };
};
void preprocess(token_list&, std::string, std::string&, rs_error*,
                std::shared_ptr<std::vector<std::filesystem::path>> = nullptr);
rbc_program torbc(token_list&, std::string, std::string&, rs_error*);

namespace conversion
{
    class CommandFactory
    {
    public:
        using _This = CommandFactory&;
    private:
        mccmdlist commands;
        mc_program& context;
        rbc_program& rbc_compiler;
        inline void add(mc_command& c)
        { 
            make(c);
            commands.push_back(c);
        }
        
        _This op_reg_math(rbc_register& reg, rbc_value& val, bst_operation_type t);
        inline _This nop_reg_math(rbc_register& reg, rbc_value& val, bst_operation_type t)
        {
            WARN("Non operable register math is not supported.");
            return THIS;
        }
    public:
        CommandFactory(mc_program& _context, rbc_program& _rbc_compiler) : context(_context), rbc_compiler(_rbc_compiler)
        {}
        template<typename... Tys>
        inline void create_and_push(Tys&&... t)
        {
            mc_command c(false, std::forward<Tys>(t)...);
            make(c);
            add(c);
        }
        inline mccmdlist& package()
        {
            for(auto& cmd : commands)
                cmd.addroot();
            return commands;
        }
        inline void pop_back()
        {
            commands.pop_back();
        }
        void make(mc_command& in);

        _This copyStorage   (const std::string& dest, const std::string& src);
        _This appendStorage (const std::string& dest, const std::string& _const);
        _This createVariable(rs_variable& var);
        _This createVariable(rs_variable& var, rbc_value& val);
        _This math          (rbc_value& lhs, rbc_value& rhs, bst_operation_type t);
        _This pushParameter (rbc_value& val);
        _This popParameter  ();
        _This invoke        (rbc_function& func);

        
        static mc_command getVariableValue(rs_variable& var);
        static mc_command getRegisterValue(rbc_register& reg);
        _This             setRegisterValue(rbc_register& reg, rbc_value& c);
        _This             setVariableValue(rs_variable& var, rbc_value& val);
    };
}


mc_program tomc(rbc_program&, std::string&);
