use lang;

method: void tell_player_is_odd_or_even(num: int)
{
    if (num % 3 == 0)
    {
        return "X is even";
    }
    else
    {
        return "X is odd";
    }
}
x: string! = tell_player_is_odd_or_even(45);


msg(@r, x);