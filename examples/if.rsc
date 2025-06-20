use lang;

method: void tell_player_is_odd_or_even(num: int)
{
    if (num % 2 == 0)
    {
        if (num == 2)
        {
            return "X is just about even!";
        }
        else
        {
            return "X is even, but not 2!";
        }
    }
    else
    {
        return "X is odd";
    }
}
y: string! = "The value of x is:";
x: string! = tell_player_is_odd_or_even(44);
msg(@r, "Running program...");
msg(@r, y);
msg(@r, x);
