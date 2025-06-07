#include "mc.hpp"
mc_command::_This mc_command::addroot()
{
    std::string name = "";

    switch(cmd)
    {
        case MC_EXEC_CMD_ID:
            name = "execute";
            break;
        case MC_FUNCTION_CMD_ID:
            name = "function";
            break;
        case MC_DATA_CMD_ID:
            name = "data";
            break;
        case MC_SCOREBOARD_CMD_ID:
            name = "scoreboard";
            break;
        default:
            WARN("Unknown command.");
            break;
    }
    body = name + ' ' + body;
    return THIS;
}
mc_command::_This mc_command::ifcmpreg(comparison_operation_type t)
{
    if (!isexec())
    {
        addroot();
        cmd = MC_EXEC_CMD_ID;
    }
    if (t != comparison_operation_type::EQ || t != comparison_operation_type::NEQ)
        WARN("Performing undefined operation on comparison register. Defaulting to neq.");
    bool eq = t == comparison_operation_type::EQ;
    std::string s = t == comparison_operation_type::EQ ? "if" : "unless";
    body = s + SEP "score" SEP RBC_REGISTER_PLAYER SEP RBC_COMPARISON_RESULT_REGISTER + body;
    return THIS;
}
mc_command::_This mc_command::ifint(const std::string& lhs, comparison_operation_type t, const std::string& rhs, bool negate)
{
    using T = comparison_operation_type;
    if (!isexec())
    {
        addroot();
        cmd = MC_EXEC_CMD_ID;
    }

    std::string op = "=";

    switch(t)
    {
        case T::EQ:
            break;
        case T::NEQ:
            op = "!=";
            break;
        case T::GT:
            op = ">";
            break;
        case T::GTE:
            op = ">=";
            break;
        case T::LT:
            op = "<";
            break;
        case T::LTE:
            op = "<=";
            break;
        default:
            WARN("Unknown int comparison operator.");
            break;
    }
    body = (negate ? "unless score " : "if score ") + lhs + SEP + op + SEP + rhs + SEP + body;
    return THIS;
}

mc_command::_This mc_command::store(bool result, const std::string& where)
{
    switch(cmd)
    {
        case MC_EXEC_CMD_ID:
        {
            if (result)
                body = "store result " + where + body;
            else
                body = "store success " + where + body;

            break;   
        }
        default:
        {
            addroot();
            if (result)
                body = "store result " + where + " run " + body;
            else
                body = "store success " + where + " run " + body;
            cmd = MC_EXEC_CMD_ID;
            break;
        }
    }
    return THIS;
}
mc_command::_This mc_command::storeSuccess(const std::string& where)
{
    return store(false, where);
}
mc_command::_This mc_command::storeResult(const std::string& where, const std::string& dataType, int scale)
{
    return store(true, where + " " + dataType + " " + std::to_string(scale));
}
mc_command::_This mc_command::storeResult(const std::string& where)
{
    return store(true, where);
}


void writemc(mc_program&, const std::string&, rs_error*)
{
    // TODO;
}