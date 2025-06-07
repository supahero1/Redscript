# Redscript Byte Code

Redscript Byte code is the intermediate form between human redeable redscript code and the compiled minecraft commands outputted by redscript. It is only used inside the compiler to simplify its job.

# Data types in RBC

A paramteter passed to an instruction can be a pointer to a:

1. Raw Constant (1|0|"hi"|'hi'|{'t': 'x'})
2. An NBT source tag (a:b.c)
3. A register (operable or not)
4. An object
## All instructions

All instructions receive an object via C++, allowing their parameters to be distinguished between the different data types.

```
                                      |   const   |   nbt   |   oreg   |   n-oreg   |   obj   |
--------------------------------------------
CALL <id>, <inbuilt=0>                | y,y       | y,n     | y,y      | y,n        | n,n     |
SAVE <dest>, <src>                    | y,n       | y,n     | y,y      | y,n        | n,n     |
MATH <src>, <value>, <op>             | n,y,y     | n,n,n   | y,y,n    | y?,y?,n    | n,n,n
DEL  <src>                            | n         | y       | y        | y          | y       |
EQ   <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
NEQ  <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
GT   <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
LT   <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
IF                                    |           |         |          |            |         |
NIF                                   |           |         |          |            |         |
RET <val=0>                           | y         | y       | y        | y          | y       |

```


# Redscript byte code => minecraft commands

All content below uses macros defined in globals.hpp:
```c++
#define RS_CONFIG_LOCATION "./rs.config"

#define RS_STORAGE_NAME "redscript"
#define RS_PROGRAM_STORAGE (RS_STORAGE_NAME ":_program")

#define RS_PROGRAM_DATA      "_internal"
#define RS_PROGRAM_VARIABLES "variables"
#define RS_PROGRAM_REGISTERS "registers"
#define RBC_REGISTER_PLAYER "_CPU"
```
## Variables

> Note: variables access themselves via their index in an array, as some functions could create indentically named variables to the ones a user defines.

- x = (const)y => 
    ```
        data modify RS_PROGRAM_STORAGE RS_PROGRAM_VARIABLES[X_INDEX].value set value y
    ```
- x = (var)y =>
    ```
        get y.value store in x.value
        =>
        execute store result storage RS_PROGRAM_STORAGE RS_PROGRAM_VARIABLES[X_INDEX].value run data get RS_PROGRAM_STORAGE RS_PROGRAM_VARIABLES[Y_INDEX].value 
    ```

- x +=/-=/*=//=/%=/^= (const int)y (throw otherwise) =>
    ```
        put x val and y val in register, operate, store result in x
        =>
        // is it mandatory to quote the name of the objective?
        scoreboard players set RBC_REGISTER_PLAYER "0" x
        scoreboard players set RBC_REGISTER_PLAYER "1" y
        execute store result storage RS_PROGRAM_STORAGE RS_PROGRAM_VARIABLES[X_INDEX].value run scoreboard players operation RBC_REGISTER_PLAYER "0" +=/-=/*=/... RBC_REGISTER_PLAYER "1"
    ```
    > Note other forms of arithmatic such as strings or floats will be most likelyhandled by inbuilt functions. 

## Functions

Given a function $f$, it can be either inbuilt of user defined. 
- If $f$ is a minecraft native function, it will most likely need some boiler plate before being executed.
- $f$ can be given the decorator `__cpp__`, which will tell the compiler to consume and remove all `push` and subsequent `pop` operations from the byte code, and convert the function call into a 1 line, native command. 
    if the conversion doesn't exist, an error is thrown. An example of this is the tellraw command.
### Inbuilt functions

- Given a function $f$ that is inbuilt, written in the form:

    ```rsc
    method: <T>? f(...) __inbuilt__;
    ```

    $f$ compiles to simple push instructions paired with a call.

    $f$ should already be defined in a datapack.

    > The `__inbuilt__` decorator can be invoked to pass the name of the datapack: `__inbuilt__(datapackName)`

- Given a function $f$ that is precompiled yet still runs at runtime (with `__cpp__` decorator) such as tellraw:
    ```
    method: void tellraw(who: selector!, msg: string!, args...: any[]) __cpp__;
    ```

    $f$ will consume the parameters pushed onto the stack and use their predicted values in place of the commands normal arguments.

    > The `__cpp__` decorator can be invoked, telling the compiler that it is a special internal implementation: `__cpp__(tellraw)`

- Given a function $f$ that is defined by the user:
    The function will be given its own file, and converted appropriately to access the stack for its parameters.
