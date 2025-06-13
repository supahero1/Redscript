# Features to be implemented

## Wrappers

Wrappers are functions that do not compile to an mcfunction but rather serve as a compile time alias for a runtime function. They are used to assert functionality of a function, to avoid runtime errors that are impossible to debug.

### Syntax



## `compile_assert(...)`

compile_assert will be a keyword (technically a function)
that evaluates a boolean expression at compile time.
If it fails, an error is thrown.

### Examples

```c++
method say(x: selector, s: string) wrapper(impl::say)
{
    compile_assert(type(s) == string, "Invalid argument type.");

    // will compile impl:say(x, s);
}
```