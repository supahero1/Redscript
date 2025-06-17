use lang;

method: any|void return_if_even(x: any)
{
    method: void _do_stuff()
    {
        if (x % 2 != 0)
        {
            if (x / 2 == 4)
            {
                x = 2;
            }
            msg(@r, x);
            return x;
            msg(@r, "Is odd!");
        }
        msg(@r, "Didnt work!");
    }
    if (x * 4 == 4 * 4)
    {
        msg(@r, "Running do stuff...");
    }
    _do_stuff();

}
return_if_even(3);