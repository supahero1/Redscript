// ----- keywords -----          //

use msg from lang;
use * from lang;
use lang; // same as above line
use lang as inbuilt; // inbuilt.msg("stuff");


// ----- keywords -----          //

// boolean
true
false

// types
int,
float,
bool,
string,
list[T], // list[T] where T is a type. For more than 1 dimension: list[T][T]. Note T is not necessary, and the any type will be used if not specified.
object,
any

// functions
return
method

// decorators
const

// loops
for
while
break
continue

// precompilation
#macro
#ifdef
#endif

// blocks
asm {...}

// etc
null

// ----- variables -----         //

x; || x: any; || x = null; || x: any = null; || const x: any = null;

// ----- operators -----         //

x + y // int & string & float
x - y // int & float
x / y // int & float
x * y // int & float
x ^ y // int & float
x % y // int

x <op>= y // ex: x += y

// ----- functions -----         //

method functionName(x, y:any = null) (; || {...})

functionName(44, "yes!"); // type(x) == int, type(y) == string

// ----- inbuilt functions ----- //

parseInt
parseFloat
exec

... (see lang.rsc)

// ----- precompiled functions ----- //

// precompiled functions can only be executed in global scope or
// inside other precompiled functions.

// main.rsc:

    method ~readFile(path: string)
    {
        return ~inbuilt_readFileImpl(path);
    }

    const SPAWN_FILE_CONTENT = ~readFile("spawns.txt");
    