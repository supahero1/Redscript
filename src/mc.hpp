#pragma once
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "error.hpp"
#include "bst.hpp"
#include "mchelpers.hpp"

#define MC_DATA_CMD_ID 0
#define MC_EXEC_CMD_ID 1
#define MC_FUNCTION_CMD_ID 2
#define MC_SCOREBOARD_CMD_ID 3
#define MC_TELLRAW_CMD_ID 4

#define THIS *this;

typedef unsigned int uint;

struct mc_command
{
    using _This = mc_command&;

    bool        macro;
    uint        cmd;
    std::string body;

    constexpr inline const bool isexec()
    { return cmd == MC_EXEC_CMD_ID; }

    _This addroot();

    _This store(bool result, const std::string& where);
    _This storeResult(const std::string& where, const std::string& dataType, int scale);
    _This storeResult(const std::string& where);
    _This storeSuccess(const std::string& where);

    _This ifcmpreg(comparison_operation_type t);
    _This ifint   (const std::string& lhs, comparison_operation_type t, const std::string& rhs, bool negate = false);
};

typedef std::vector<mc_command> mccmdlist;
struct mc_function
{
    std::vector<mc_command> commands;
};

struct mc_program
{
    uint varStackCount = 0;
    uint ifStatementDepth = 0;
    mc_command* lastIfStatement;
    std::unordered_map<std::string, mc_function> functions;
    mc_function* currentFunction = nullptr;
    mc_function globalFunction;
};
const std::filesystem::path makeDatapack(const std::filesystem::path&);
void writemc(mc_program&, const std::string&, const std::string&, std::string&);