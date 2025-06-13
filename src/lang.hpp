#pragma once
#include <cstdint>
#include <string>
#include <variant>
#include <unordered_map>
struct rbc_program;
struct rbc_register;
struct rbc_constant;

struct rs_variable;
struct rs_object;
#include "bst.hpp"
#include "token.hpp"

struct rs_type_info
{
    int32_t type_id     = -1; // first type id
    uint32_t array_count = 0;
    bool optional = false;
    bool strict   = false;
    std::vector<rs_type_info> otherTypes; // others if specified
    inline std::string tostr()
    {

        std::string typestr = std::to_string(type_id);
        if (optional) typestr.push_back('?');
        if (strict)   typestr.push_back('!');
        for(size_t i = 0; i < otherTypes.size(); i++)
            typestr += '|' + otherTypes.at(i).tostr();

        if (array_count == 0)
            return typestr;
        else
            return typestr + '[' + std::to_string(array_count) + ']';    
    }
};
// holds a bst for pruning and computing,
// or a shared ptr to a raw non operational result such as an object.
// this ptr is not given a value to singleton expressions, such as integers or strings.
// only to values that cannot be operated on.
struct rs_expression
{

    using _ResultT = std::variant<rbc_constant, std::shared_ptr<rbc_register>, std::shared_ptr<rs_variable>, std::shared_ptr<rs_object>>;
    bst_operation<token> operation;
    std::shared_ptr<_ResultT> nonOperationalResult = nullptr;
    _ResultT rbc_evaluate(rbc_program&, rs_error*,
                               bst_operation<token>* = nullptr);
};
struct rs_compilation_info
{
    int varIndex = 0;
};
class rs_variable
{
public:
    token&       from;
    std::string  name;
    uint32_t     scope;
    rs_type_info type_info, real_type_info;
    bool global = false; bool _const = false;

    std::shared_ptr<rs_expression> value = nullptr;
    std::shared_ptr<rs_object> fromObject = nullptr;

    rs_compilation_info comp_info;

    rs_variable(token& _from, uint32_t _scope = 0, bool _global = false)
        : from(_from), name(_from.repr), scope(_scope), global(_global)
    {}
    rs_variable(token& _from, rs_type_info realInfo, uint32_t _scope = 0, bool _global = false)
        : from(_from), name(_from.repr), scope(_scope), real_type_info(realInfo), global(_global)
    {}
    rs_variable(token& _from, rs_type_info explicitInfo, rs_type_info realInfo, uint32_t _scope = 0, bool _global = false)
        : from(_from), name(_from.repr), scope(_scope), type_info(explicitInfo), real_type_info(realInfo), global(_global)
    {}

    inline std::string tostr()
    {
        std::stringstream stream;

        stream << '{' << from.str() << ", name=" << name << ", scope=" << scope << ", typeinf=" << type_info.tostr() << ", (g=" << global << ", c=" << _const << ")}";
        return stream.str();
    }
};
enum class rs_object_member_decorator
{
    OPTIONAL,
    REQUIRED,
    SEPERATE,
};
struct rs_object
{
    const static uint32_t TYPE_CARET_START = 10; // custom types start at 10 and onwards for type id.
    using _MemberT = std::pair<rs_variable, rs_object_member_decorator>;
    // x.name -> 
    std::string name; // empty for inline objects
    uint32_t scope;
    // negative for inline created objects
    int32_t typeID = -1;
    std::unordered_map<std::string, _MemberT> members;

    inline std::string tostr()
    {
        std::stringstream stream;

        stream << "(obj)" << (name.empty() ? "{inline=1" : "{name=" + name) << ", members={";
        int i = 0;
        for(auto& member : members)
        {
            if (i != 0)
                stream << ',';
            stream << member.second.first.name;
            i++;
        }
        stream << "}}";
        return stream.str();
    }
};

void prune_expr(rbc_program&, bst_operation<token>&, rs_error*);
bst_operation<token> make_bst(rbc_program& program, token_list& tlist, long& start, rs_error* err, bool br = false, bool oneNode = false, bool obj = false);
rs_expression expreval(rbc_program& program, token_list& tlist, long& start, rs_error* err, bool = false, bool = true, bool obj = false, bool prune = true);
std::shared_ptr<rs_object> parseInlineObject(rbc_program& program, token_list& tlist, long& start, rs_error* err);