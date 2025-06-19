use lang;

object player
{
    health: int! = 4;

    method compare(health, uuid);
}

// OR, AND
// object accessing: x.y.z.w
// inb library 

method: void dostuff(x: int)
{
    y = x * 2;

    z: int! = y * 400;

    msg(@r, z);
}
msg(@r, "§4You are stupid!");
dostuff(44);
msg(@r, "DONE");