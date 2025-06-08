# Internals of Redscript

## Variables

We can use the data storage command to store any kind of data we want.

`data storage <set|modify|merge|remove|...>`

Variables will be located at `RS_STORAGE_NAME:RS_PROGRAM_DATA RS_PROGRAM_VARIABLES` defined in `globals.hpp`.

### Program Variables

Program variables are things like the program or depth counter.
They can help write inbuilt functions like error throwing.
They can be accessed via redscript using the `$` operator:
```py
msg(@a, $scope)
$scope becomes RS_STORAGE_NAME:RS_PROGRAM_DATA scope
```
## Functions

### User Defined functions

In Redscript, user defined functions help a user to modulate their code.
as each command is a `.mcfunction` file, we can just make a new one for their function. Despite macros being introduced into minecraft commands, we believe they are quite slow. until we figure out if that is the case, we will result to the normal way of creating functions.

Parameters will be stored in `RS_STORAGE_NAME:RS_PROGRAM_DATA RS_PROGRAM_PARAMETERS`, and will be an array of items with stack functionality.

They will be pushed before function execution and popped after.

We can execute the function by using the function command.

Due to limitations the max recursion depth is 35665.

## Inbuilt functions

Inbuilt functions that are not precompiled will have to be written using redscript and compiled with the users code or as pre packaged `.mcfunction`s
regardless, they will end up as that anyway.

Inbuilt functions wont technically be inbuilt, as you will have to include them from the `lang.rsc` source file using `use lang;` at the top of your file.

Example:
```c++
method type() __inbuilt__; // declaration of compiled & implemented inbuilt function

// declaration of inbuit function with an uncompiled body
method type(__v: any&) __inbuilt__, __single__ // denoted as single instruction function and gets compiled to the function call instead
{
    // bad practice! type should be inbuilt as it is a one liner in terms of complexity
    // and should be implemented

    // this code works as parameters will most likely be stored with a .value and .type just like
    // variables. Therefore we can just return its type and the compiler has already.
    // Therefore Redscript does attain the power to determine what is an rvalue and lvalue.
    // 4 -> r value, myVar -> lVal, __v -> lVal.
    // we dont know if lvalues | rvalues will be implemented however.
    return getattr(__v, "type");
}
```


# Objects

Redscript stores everything as objects in minecraft storage. Here are how we use them effectively.

## Objects are huge!

A player's NBT can be really big. We don't want to be copying over huge amounts of json just for one or two functions calls. Thats where **`dynamic-object-parsing`** (DOP) comes into play.

### DOP -> explicit usage

We can use DOP to explicitly define a structured variant of an object:

```python
object minimal_player
{
    health: float; name: str; deaths: int;
}

player: minimal_player = (minimal_player)getPlayer(@p);
// OR (denoting of type is optional)
player: minimal_player = getPlayer(@p) as minimal_player;
```

This will tell the compiler that we should only retrieve certain attributes from the players data.

### DOP -> implicit & inline usage

Although sometimes this can be lengthy and use a lot of objects.

DOP also allows it to be defined as an inline object.

```py
player = getPlayer(@p) as {health:float, name, deaths:int};
```

# Type Enforcing

For the redscript inbuilt library, type enforcing is necessary to not allow users to mess up their code.

```c++
method msg(__message: string|object!, __who: nbt) __inbuilt__;

```

`string|object` tells the compiler `__message` can be a string or object. the exclamation mark (`!`) tells the compiler that message can be a string or an object, but not anything else.

It reads semantically as `string OR MUST BE object`. As in, if message isn't a string, it has to be an object, or else the compiler will throw an error.
> It's worth noting that: `string|object!` == `string!|object!` == `string!|object`

The counter part to the enforcing operator is the **voluntary/optional operator**.

```
method returnSomething(v: int|str?)
{
    return v;
}
```

## Selectors

Selectors are annoying. Say for example:

```rsc
method: void test(x: selector!)
{
    msg(x, "Hello world!");
}
```

this would compile to:


```
test:
    PUSH x
    PUSH "Hello world!"
    CALL msg
    POP
PUSH @p
CALL test
POP
```

Although this looks like it works, there are some problems.

For one, It would be nice to have variadic selectors, i.e:

```
x = "TBCMDev";
@p[name=x]
```

And this can be accomplished through macros. Although a selector is not a function call, and macros are used in the whole mcfunction file.
**they arent performant at all.**

As well as this, passing @p to a user made function is a variable in memory. This cannot be applied to a function call like:
```
x = @a
tellraw x
```

It would have to be known at compile time that it is indeed a constant, prepended with an @ in the mcfunction, and substituted using macros.
```
$tellraw @$(x)
```

The way to do this would be:

1. During compile time, each function is given their own list of macros:
    ```c++
        std::unordered_map<rbc_value, std::string>
    ```
2. Then the compiler locates an rbc_value's (variable, constant, register, or object) usage and pastes the macro syntax + specific macro name in to where it should go in the command.

3. Finally, if a function had any macros, we would call it with:
    ```
        function <name> with {"macro1": 12, "macro2": {...}}
    ```
