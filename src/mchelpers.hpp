#pragma once
#include "globals.hpp"

#define SEP " "
#define PADL(x) " " #x
#define PADR(x) #x " "
#define PAD(x) PADL(x) " "
#define INS(x) +(x)+
#define INS_L(x) +(x)
#define STR(x) std::to_string(x)
#define ARR_AT(arr, i) arr "[" INS(i) "]"

#define MC_DATA(cmd, where) #cmd SEP RS_PROGRAM_STORAGE SEP where
#pragma region variables

#define MC_VARIABLE_VALUE(id) ARR_AT(RS_PROGRAM_VARIABLES, STR(id)) ".value"
#define MC_VARIABLE_VALUE_FULL(id) RS_PROGRAM_STORAGE SEP MC_VARIABLE_VALUE(id)
#define MC_GET_VARIABLE_VALUE(id) MC_DATA(get storage, MC_VARIABLE_VALUE(id))

#define MC_VARIABLE_JSON(data, scope, type) "{\"value\":" data ",\"scope\":" INS(scope) ",\"type\":" INS(type) "}"
#define MC_VARIABLE_JSON_DEFAULT(scope, type) MC_VARIABLE_JSON("0", scope, type)
#define MC_VARIABLE_JSON_VAL(data, scope, type) MC_VARIABLE_JSON(INS(data), scope, type)
#define MC_VARIABLE_CREATE_DEF(scope, type, ...) MC_DATA(modify storage, RS_PROGRAM_VARIABLES) \
                                                 PADL(set) PAD(value) \
                                                 MC_VARIABLE_JSON_DEFAULT(scope, type) \
                                                 __VA_ARGS__
#define MC_VARIABLE_SET_CONST(id, v) PADR(modify storage) RS_PROGRAM_DATA SEP ARR_AT(RS_PROGRAM_VARIABLES, STR(id)) PAD(.value set value) INS_L(v)
#pragma endregion variables

#pragma region registers
#define MC_OPERABLE_REG_RAW(id) "r" id
#define MC_OPERABLE_REG(id) RBC_REGISTER_PLAYER SEP MC_OPERABLE_REG_RAW(id)
#define MC_CREATE_OPERABLE_REG(id, criteria) PADR(objectives add) MC_OPERABLE_REG_RAW(INS(STR(id))) SEP criteria SEP "\"" INS(STR(id)) "\""
#define MC_COMPARE_REG_GET PADR(players) PADR(get) RBC_REGISTER_PLAYER SEP RBC_COMPARISON_RESULT_REGISTER
#define MC_OPERABLE_REG_SET(id, v) PADR(players) PADR(set) MC_OPERABLE_REG(INS(STR(id))) SEP INS_L(v)
#define MC_OPERABLE_REG_GET(id) PADR(players) PADR(get) MC_OPERABLE_REG(INS_L(STR(id)))
#define MC_NOPERABLE_REG_GET(id) MC_DATA(get storage, ARR_AT(RS_PROGRAM_REGISTERS, STR(id)))
#pragma endregion registers

#pragma region operable_math
#define MC_REG_INCREMENT_CONST(id, x) PADR(players) PADR(add) MC_OPERABLE_REG(INS(STR(id))) SEP INS_L(x)
#define MC_REG_DECREMENT_CONST(id, x) PADR(players) PADR(remove) MC_OPERABLE_REG(INS(STR(id))) SEP INS_L(x)
#define MC_REG_OPERATE(lh_id, op_str, rh_id) PADR(players) MC_OPERABLE_REG(INS(STR(lh_id))) PAD(operation) INS(op_str) SEP MC_OPERABLE_REG(INS_L(STR(rh_id)))
#pragma endregion operable_math

#pragma region stack
#define MC_STACK_PUSH_CONST(x) MC_DATA(modify storage, RS_PROGRAM_STACK) PAD(append value) INS_L(x)
#pragma endregion stack

#pragma region tellraw
#define MC_TELLRAW_CONST(selector, val) '@' INS(selector) SEP INS_L(val)

// DONT USE: not finished
#define MC_TELLRAW_OPERABLE_REGISTER(selector, id) '@' INS(selector) SEP "[{\"score\":{" MC_OPERABLE_REG() ".value}, {\"storage\":\"" RS_PROGRAM_DATA "\"}"
// for tellraw in particular a function needs to be made. Coming in next version.
#define MC_TELLRAW_VARIABLE(selector, id) '@' INS(selector) SEP "[{\"nbt\":\"" ARR_AT(RS_PROGRAM_VARIABLES, STR(id))".value\", \"storage\":\"" RS_PROGRAM_STORAGE "\"}]"
#pragma endregion tellraw

#pragma region mcmeta
#define MC_MCMETA_CONTENT(version) "{\"pack\": {\"pack_format\": " INS(STR(version)) ", \"description\": \"A program created by redscript.\"}}"
#pragma endregion mcmeta