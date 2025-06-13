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
            return x;
        }
        else
        {
            return;
        }
    }
    if (x * 4 == 4 * 4)
    {
        _do_stuff();
    }
    // TODO: every function should terminate with a return 0
    return;
}

return_if_even(12);