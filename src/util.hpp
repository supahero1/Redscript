#pragma once

template<typename T>
inline T& unmove(T&& x)
{
    return x;
}


namespace util
{
    template<typename T>
    constexpr T copy(const T& t)
    {
        return T(t);
    }
}

template<typename _T1, typename _T2>
struct commutative
{
    
};