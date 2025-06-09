#include "mc.hpp"
#include "util.hpp"
mc_command::_This mc_command::addroot()
{
    std::string name = "";

    switch (cmd)
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
    case MC_TELLRAW_CMD_ID:
        name = "tellraw";
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
mc_command::_This mc_command::ifint(const std::string &lhs, comparison_operation_type t, const std::string &rhs, bool negate)
{
    using T = comparison_operation_type;
    if (!isexec())
    {
        addroot();
        cmd = MC_EXEC_CMD_ID;
    }

    std::string op = "=";

    switch (t)
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

mc_command::_This mc_command::store(bool result, const std::string &where)
{
    switch (cmd)
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
mc_command::_This mc_command::storeSuccess(const std::string &where)
{
    return store(false, where);
}
mc_command::_This mc_command::storeResult(const std::string &where, const std::string &dataType, int scale)
{
    return store(true, where + " " + dataType + " " + std::to_string(scale));
}
mc_command::_This mc_command::storeResult(const std::string &where)
{
    return store(true, where);
}
const std::filesystem::path makeDatapack(const std::filesystem::path& path)
{
    const std::filesystem::path funcDir = path/"data"/RS_STORAGE_NAME/"function";
    std::filesystem::create_directories(funcDir);

    std::ofstream mcMetaPack(path/MC_MCMETA_FILE_NAME);

    if (!RS_CONFIG.exists("versionid"))
        throw std::runtime_error("'versionid' is not specified in redscript config.");

    const int version = RS_CONFIG.get<int>("versionid");

    mcMetaPack << MC_MCMETA_CONTENT(version);

    mcMetaPack.close();


    return funcDir;
}
#define WRITE_ERROR(ec, what) { *err = rs_error()}
void writemc(mc_program &program, const std::string &name, const std::string &path, std::string &err)
{
    if (!RS_CONFIG.exists("mcpath"))
    {
        err = "'mcpath' doesn't exist in config. Can't write.";
        return;
    }

    auto writeFunction = [&](mc_function &func, const std::filesystem::path &path)
    {
        std::ofstream stream(path);
        for (auto &command : func.commands)
            stream << command.body << '\n';
        stream.close();
        return true;
    };
    try
    {
        std::filesystem::path mcpath(RS_CONFIG.get<std::string>("mcpath"));
        const std::filesystem::path safeName = removeSpecialCharacters(name);

        mcpath = mcpath/path/MC_DATAPACK_FOLDER/(safeName.stem());

        const std::filesystem::path funcPath = makeDatapack(mcpath);

        std::filesystem::path to = funcPath/safeName;
        if (!writeFunction(program.globalFunction, to))
        {
            ERROR("Could not write function to '%s'.", to.string().c_str());
            return;
        }
        // TODO: other functions
    }catch(std::exception& error)
    {
        err = error.what();
    }
}
