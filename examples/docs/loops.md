# Loops in redscript

## For loops

For loops can iterate over objects, lists, or ranges only.

### Iterating over objects

```c++
type pair; // defined in lang
obj = {hello: "goodbyte", goodbye: "hello"}

for (x: pair in obj)
{
    // could be this
    msg(@r, x[0]); msg(@r, x[1]);
    // / or this
    msg(@r, x.i1); msg(@r, x.i2);
}

```

The `pair` type, defined in `lang.rsc`, holds `i1` & `i2`, two members. It may be more performant however to go with an array approach.

### Iterating over lists

```py

for(item: int in [1,2,3,4])
{
    msg(@r, item);
}

```

### Iterating over ranges

```py

for (i in range(100))
{
    msg(@r, i);
}
for (i in range(0, -100, -1))
{
    msg(@r, i);
}

```
