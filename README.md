# Redscript - The programming language for minecraft.

Redscript is a programming language made entirely for Minecraft. It compiles high level human readable code into Minecraft commands thatc can be executed in any Minecraft world. 

Redscript was made to abstract away the frustrations people have with the Minecraft command system.

> NOTE: There is no website yet. Coming on release.

Read more about it on our website [here](https://redscript.com).

This project is maintained by only 1 developer, me! This means that the development process is slow,
and it is still in the beta stages. Feel free to flag any issues here on github, or contact me directly.

# Example

```c++
use lang;

x = 4;
y: int;
z: int!; // strictly integer
w: int?|string!; // possibly integer or strictly string

method: void myFunction(x: int)
{
    msg(@r, "Parameter: ", x);
}
// the function must be compiled using redscript.
method: void myFunctionInExternalDatapack(x: string) __inbuilt__;
method: void myNamedFunctionInExternalDatapack(x: string) __inbuilt__("nameOfFunction");
// example: //wand
method: void myCommandInExternalDatapack() __external__("/wand");

object minimal_player
{
    // when casting to/constructing minimal_player,
    // health and name must be present.
    required health: int!;
    required name  : string!;

    // This field will be saved if part
    // of the object being casted (see below) OR if given a default value.
    // if not, as the field suggests it's optional and therefore not in the final object.
    optional hunger: float! = 20.0;

    // not part of the players NBT but still kept regardless of default value.
    seperate points: int = 0;
}

// contains all NBT
fullPlayer = getPlayer("name");

// only contains health, player, maybe hunger, and points.
// if getPlayer() is null, the cast returns null.
smallPlayer = (minimal_player) getPlayer("name");

if (fullPlayer == null)
{
    throw("Could not find player.");
}
elif (smallPlayer == null)
{
    throw("Could not find small player");
}

// in theory
msg(@a[hunger<fullPlayer.hunger], "Broadcasting to players hungrier than fullPlayer.");

```

# How?

Heres a [niche youtube video](YT_LINK) I made showcasing the capabilities of the language and my journey making it.

# Installation

To install Redscript, you can clone this repo and build it yourself. As of version 0.1, It does not use any external libraries.

> Redscript uses c++20 or above.

# Documentation

Redscript is still in beta, and the documentation will change rapidly, however you can read the documentation [here](https://redscript.com/docs).

# Share your creations and concerns

Join our Discord! [](DISCORD_LINK).

# Adding Features

Want a feature added or have a suggestion? Request it through my voting system here [](https://redscript.com/roadmap).

# Support

I spend a lot of my time on this project, and if you would like to support my journey then please visit [redscripts website](https://redscript.com/donate) to learn more.

# FAQ

#### What version of minecraft does Redscript work in?

Redscript employs the function features of **minecraft 1.21.5+**, and has not been tested in previous versions. Once released, previous version support can be added.