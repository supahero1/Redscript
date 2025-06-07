// flag strict; disables invalid typing such as: x:int = "Hello!";
use lang;

object player
{
    required health: int!;
    required name  : string!;

}

// causes seg fault
// method: void msg(i: int)
// {
//    msg();
// }

minimal_player = (player) getPlayer("Gregory");

y: int? = 4;
x: player! = {health:y, name: "kristian"};

msg(@p, x);