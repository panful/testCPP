#include "temp.h"

template<typename T>
T AddFunc(T&& t1, T&& t2)
{
    return t1 + t2;
}