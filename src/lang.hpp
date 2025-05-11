#pragma once
#include <cstdint>
#include <string>

struct rbc_program;

#include "bst.hpp"
#include "token.hpp"
struct rs_type_info
{
    uint32_t type_id     = 0;
    uint32_t array_count = 0;

};

class rs_variable
{
public:
    token&       from;
    std::string  name;
    uint32_t     scope;
    rs_type_info type_info, real_type_info;
    bool global; bool _const;

    rs_variable(token& _from, uint32_t _scope = 0, bool _global = false)
        : from(_from), name(_from.repr), scope(_scope), global(_global)
    {}
    rs_variable(token& _from, rs_type_info realInfo, uint32_t _scope = 0, bool _global = false)
        : from(_from), name(_from.repr), scope(_scope), real_type_info(realInfo), global(_global)
    {}
    rs_variable(token& _from, rs_type_info explicitInfo, rs_type_info realInfo, uint32_t _scope = 0, bool _global = false)
        : from(_from), name(_from.repr), scope(_scope), type_info(explicitInfo), real_type_info(realInfo), global(_global)
    {}

    void rbc_create(rbc_program&, rs_error&);
    // void rbc_assign(rs_scope&)
};

struct rs_expression
{
    bst_operation<token> operation;

    void rbc_evaluate(rbc_program&, rs_error&);
};

bst_operation<token> make_bst(rbc_program& program, token_list& tlist, size_t& start, rs_error* err, bool br = false, bool oneNode = false);
rs_expression expreval(rbc_program& program, token_list& tlist, size_t& start, rs_error* err);