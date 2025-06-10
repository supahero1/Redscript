use lang;

method: void dostuff(x: int)
{
    y = x * 2;

    z: int! = y * 400;

    msg(@r, z);
}
msg(@r, "§4You are stupid!");
dostuff(44);
msg(@r, "DONE");