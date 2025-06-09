use lang;

method: void doStuff(x: int)
{
    y = x * 2;

    z: int! = y * 10;

    msg(@r, z);
}

doStuff(44);