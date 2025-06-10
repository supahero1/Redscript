use math;

type path_selector: block;

method: void msg   (_p: selector!, _msg: string!)      __inbuilt__ __cpp__;
method: void kill  (_p: selector!)                     __inbuilt__ __cpp__;
method: void place (_pos: position!, _what: block|int) __inbuilt__ __cpp__;

// method: player! getPlayer(_name: string!) __single__
// { return __cpp__(::playerGet); }