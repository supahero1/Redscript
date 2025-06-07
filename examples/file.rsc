use lang
s;
~
{
    noteBlockSounds ~= readFile("noteblock.txt");

    s = noteBlockSounds
}

object player
{
    health; name;
}
method normalize(vec2)

for (i = 0; i < 10; i+=1) // look into ticks!!
{
    // get health and name of player, store in player variable.
    player = (minimal_player)getPlayer(@r);
    
    player.health = 0;
    // data set 

    sleep(1);
    stopSound(@r);
}

