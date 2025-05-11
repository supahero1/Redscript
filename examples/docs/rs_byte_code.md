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
DEL  <src>                            | n         | y       | y        | y          | y       |
EQ   <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
NEQ  <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
GT   <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
LT   <val1>, <val2>, <for_if_flag=1>  | y,y,y     | n,n,n   | y,y,y    | y,y,y      | y,y,n   |
IF                                    |           |         |          |            |         |
NIF                                   |           |         |          |            |         |
RET <val=0>                           | y         | y       | y        | y          | y       |

```