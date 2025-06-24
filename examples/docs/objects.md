# Constructing objects

```c++
object player
{
    required health: float;
    required name: string;
}

me: player = new player(20.0, "TBCMDev");
```
will pseudo compile to:

```
store {"health": 20.0, "name": "TBCMDev"} in variables
```

## Constructing with non-constants

```c++
object player
{
    required health: float;
    required name: string;
}
x: float = 20.0;
name = getPlayer("TBCMDev").name;
me: player = new player(x, name);
```