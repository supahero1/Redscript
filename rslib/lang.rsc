use math;

// type path_selector: block;
method: void msg   (_p: selector!, _msg: string!)      extern __cpp__;

// method<T, T1>: void msg   (_p: selector!, [T, T1]...);

method: void kill  (_p: selector!)                     extern __cpp__;
// method: void place (_pos: position!, _what: block|int) __inbuilt__ __cpp__;

// method: player! getPlayer(_name: string!) __single__
// { return __cpp__(::playerGet); }

module lists
{
    method: void append(__l: list, __v: any) __cpp__;
    
}