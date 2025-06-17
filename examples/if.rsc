use lang;

method: void tell_player_is_odd_or_even(num: int)
{
    if (num % 2 == 0)
    {
        return "X is even";
    }
    else
    {
        return "X is odd";
    }
}
y: string! = "The value of x is:";
x: string! = tell_player_is_odd_or_even(45);
msg(@r, "Running program..");
msg(@r, y);
msg(@r, x);