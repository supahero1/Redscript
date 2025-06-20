use lang;

module lists
{
    method: void append(l: list|string, val: any)
    {
        val = 2;
    }
    module extended
    {
        method: void delete()
        {
            const x;
            x = 4;
            x = 2;
            msg(@r, "Not implemented");
        }
    }
}
// to access:
l: list = 4;

lists::append(l, 4);
// not the same as : minecraft:redscript // this is an nbt path
lists::extended::delete();