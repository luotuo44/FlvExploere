//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef HELPER_H
#define HELPER_H

#include<stddef.h>
#include<numeric>


namespace FLV
{

namespace Helper
{


template<typename T, size_t N>
struct __GetNBits
{
    static size_t value(T t, size_t pos)
    {
        constexpr size_t mask = (1<<N) - 1;
        return (t>>pos) & mask;
    }
};


template<typename T>
struct __GetNBits<T, 0>
{
    static size_t value(T , size_t )
    {
        return 0;
    }
};





template<size_t N, typename T>
inline size_t getNBits(T t, size_t pos)
{
    return __GetNBits<T, N>::value(t, pos);
}


inline bool isLittleEndian()
{
    int a = 1;
    return *(reinterpret_cast<char*>(&a));
}



template<typename InputIterator>
inline size_t getSizeValue(InputIterator begin, InputIterator end)
{
    size_t init = 0;
    return std::accumulate(begin, end, init, [](size_t val, typename InputIterator::value_type cur){
        return (val<<8) + cur;
    });
}



template<typename T>
inline size_t getSizeValue(const T *begin, const T *end)
{
    size_t init = 0;
    return std::accumulate(begin, end, init, [](size_t val, T cur){
        return (val<<8) + cur;
    });
}


template<typename T>
inline size_t getSizeValue(const T &t)
{
    return getSizeValue(std::begin(t), std::end(t));
}

}//end of namespace Helper

}//end of namespace FLV

#endif // HELPER_H

