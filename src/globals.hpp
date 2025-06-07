#pragma once
#include "config.hpp"

#define RS_CONFIG_LOCATION "./rs.config"

#define RS_STORAGE_NAME "redscript"
#define RS_PROGRAM_STORAGE RS_STORAGE_NAME ":_program"

#define RS_PROGRAM_STACK     "stack"
#define RS_PROGRAM_DATA      "_internal"
#define RS_PROGRAM_VARIABLES "variables"
#define RS_PROGRAM_REGISTERS "registers"
#define RBC_REGISTER_PLAYER "_CPU"
#define RBC_REGISTER_PLAYER_OBJ "alu"
#define RBC_COMPARISON_RESULT_REGISTER "cmp"


inline rs_config RS_CONFIG;