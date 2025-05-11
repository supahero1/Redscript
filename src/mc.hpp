#pragma once
#include <unordered_map>
#include <vector>
#include "error.hpp"

struct mc_command
{
    bool        macro;
    std::string cmd;
    std::string body;
};

struct mc_function
{
    std::vector<mc_command> commands;
};


typedef std::unordered_map<std::string, mc_function> mc_program;
 
void writemc(mc_program&, const std::string&, rs_error*);