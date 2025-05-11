# **The Program**


## Storage

In Minecraft, we can store data in 2 ways:

1. VIA storage
2. VIA scoreboard

### Minecraft storage

redscript stores all program variables in the `redscript:program_data` data block.

All data to do with the running of your program is housed here, including `variables`.

Variables is a json dict of a variable, given the form:
```jsx
<name>: 
{
    data: <data> || 0,
    type: <type_id> || 0 (any)
}
```

Variables can be added using the `data merge` command.
Variables can be removed using the `data remove` command.

### The Minecraft Scoreboard

The scoreboard allows us to do arithmatic on integers, 
poll events for some things as well as create registers using the command `scoreboard objectives add <register>`.

Through this, we can create a dummy player called **`_CPU`**, and add all register objectives to him such as `eax, ebx, ecx, edx, esi`, etc. As well as variables such as a program counter, we technically already have assembly made in minecraft.

The scoreboard can increment, decrement, multiply, divide, and even perform modulus operations on two given integers.

This means we can take an integer variables data from memory (data), set it to the value of the register, and operate on when required.

We can also make a register for whether or not an error has been thrown, and halt any program's execution if that bit is set, instead of constantly querying memory to see if an error has landed there.

## Functions

Given that we can store anything we want, we can also store parameters to a function, located somewhere like `redscript:program_data/parameters`. Parameters will be an array, and can be indexed to find each parameter, the first starting at 0.
Functions can automatically pop their parameters after returning or finishing execution from the parameter array.

Although it is important to note that macros do exist for functions, i.e. we can pass whatever we want to functions, including variables using the `with` keyword. And we should go with this instead, as it makes writing inbuilt functions easier.

For compiling functions, we can seperate each function into their own seperate `.mcfunction` file. Although costly in big programs (we can advise lambdas later on possibly), this makes it easy to modularise the outputted program. Functions can be called using the `function` command.



## Inbuilt functions

Inbuilt functions will have to be crafted by hand, ensuring they are performant over load and will produce expected error results.

For example, `error.mcfunction` should create an error object located at `redscript:program_data/error` and set the error register to `1`:

```
$data modify storage redscript:program_data error set value $(_err)
scoreboard players set _CPU err 1
```
This can be called in redscript as:
```
throw "Unexpected Error occured"
```
Which translates to `rs_byte_code` as:
```
ERROR "Unexpected Error occured"
```

> It is important to note that inbuilt errors should provide error data as to where the runtime error occured, which will need extra registers to keep track of where we are. This is needed with a debugger

## Conditional statements

Things can be done conditionally in minecraft using the execute command `execute ... if|unless ...|data storage <path> <value>`.

However, this becomes annoying when having an if statement with hundreds of lines of code within it. What we can do is store the result of a comparison in a queue, located in program storage (`redscript:program_data/comparisons`). This will reduce runtime for large comparisons, as we can just execute if the stored result is the desired result, 1 or 0 (if | unless).

This would translate to `rs_byte_code` as:

```
EQ|NEQ|GT|LT eax, ebx, 1|0(for_if_flag), 1|0(GTE|LTE) ; stored in cpt (compare top (comparisons[0]))
IF
...
EIF ; pops 
```