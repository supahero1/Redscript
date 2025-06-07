
// note : objects denoted as const have all members set to const.
object player
{
    // static var
    global const maxHealth: float! = 20.0;
    required|optional|seperate health: float! = 20.0;
};

// players health = 10.5

// REQUIRED:
// myPlayer = {health=10.5}
const myPlayer = (player) getPlayer("tbcmwastaken");

// REQUIRED:
// runtime error: required field not set (return value was null)
const myPlayer = (player) getPlayer("thisplayerdoesntexist");

// OPTIONAL (default):
// myPlayer = {health=20.0}
const myPlayer = (player) getPlayer("thisplayerdoesntexist");

// OPTIONAL (default):
// myPlayer = {health=10.5}
const myPlayer = (player) getPlayer("tbcmwastaken");

// SEPERATE:
// myPlayer = {health=20.0}
const myPlayer = (player) getPlayer("tbcmwastaken");

const object animal
{
    name: string!;
};
const object fox(animal)
{
    name = "fox"; // after initialisation, name can never be changed.
}