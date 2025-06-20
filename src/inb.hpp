#pragma once
#include <unordered_map>
#include <vector>
#include <variant>
#include <memory>

struct rbc_constant;
struct rbc_register;
struct rs_variable;
struct rs_object;

namespace conversion
{
    class CommandFactory;
};
typedef std::variant<rbc_constant, std::shared_ptr<rbc_register>, std::shared_ptr<rs_variable>, std::shared_ptr<rs_object>, std::shared_ptr<void>> rbc_value;
struct rbc_program;
#define INB_IMPL_PARAMETERS rbc_program& program, conversion::CommandFactory& factory, std::vector<rbc_value>& parameters, std::string& err 

namespace inb_impls
{
    void msg(INB_IMPL_PARAMETERS);
    void kill(INB_IMPL_PARAMETERS);

    inline std::unordered_map<std::string, void(*)(INB_IMPL_PARAMETERS)> INB_IMPLS_MAP = 
    {
        {"msg", msg},
        {"kill", kill}
    };
};