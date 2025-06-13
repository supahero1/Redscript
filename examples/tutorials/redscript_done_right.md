# Redscript done right - Writing optimal redscript code

If you were given two programs, and had to decide which one was more performant by just looking at them, how would you do it? The right way is to know how the language they are written in operates.

With redscript, you are confined to work with Minecraft, which doesn't really like the idea of a programming language running inside it. 

To fix this, let's go over some fundementals of how redscript accomplishes its high level features, and the subsequent **dos and don'ts** of these features.


## Functional programming

Redscript provides function support in the form of individual files. Given a function `do_stuff`, its output code would be written to a file named `do_stuff.mcfunction`.

In redscript, you should write functions for **if/elif/else statement blocks** if the block exceeds around 5 lines of code or the block is nested in another **if/elif/else** block. I know, stay in your seat. Let me explain.

To accomplish conditional logic with minecraft commands, the `execute if/unless` command is used.

To compute `if (x == 2)`, redscript will:

1. Store the value of x into a scoreboard register (A)
2. Store the value `2` into another scoreboard register (B)
3. Store a result of `1` (success) into a free comparison register if A and B equal. (C)

Then for the if statement's contents, redscript will prepend an `execute if/unless` clause to all commands, checking the result of C.

This is common knowledge to anyone who has written anything in minecraft commands before. However, all of these comparisons add up. Functions are better instead of long if statements as they don't perform this comparison for every operation done.

To aid the development process of functional programming in redscript, you can define nested functions.

> Nested functions can access variables defined before it.


```c++
method: void|int dostuff(x: int!)
{
    if (x % 2 == 0)
    {
        msg(@r, "Hello! The number you entered is odd."); // comparison 1
        if (x / 2 == 4) // comparison 2
        {
            msg(@r, "Hello again! Your number is 8."); // comparison 3
            return 8; // comparison 4
        }
        msg(@r, "Ok all done here!"); // comparison 5
    }
}
```
now lets try the same with functions.
```c++
method: void|int dostuff(x: int!)
{
    method: void|int do_more_stuff()
    {
        msg(@r, "Hello! The number you entered is odd.");
        if (x / 2 == 4)
        {
            msg(@r, "Hello again! Your number is 8.");  // comparison 2
            return 8; // comparison 3
        }
        msg(@r, "Ok all done here!");
    }
    if (x % 2 == 0)
    {
        return do_more_stuff(); // comparison 1
    }
}

```

As this scales, I think we can see the winner here. It's important to note that nested ifs use >=2 comparisons to run their command without functional seperation.