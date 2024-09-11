#include <iostream>
#include <cstdlib>
#include <cstring>

#ifndef __ARRAY_H_
#define __ARRAY_H_

template <typename T>
class Array
{
public:
    Array();
    void Push(T *value);
    void Pop(size_t index);
    void Insert(size_t index, T *value);
    size_t Size();
    T *GetPointer(size_t index);
    T operator[](size_t index);
    T First();
    T Last();
    void Clear(); // If Array no need to use, Clear it or >memory leaks<.

private:
    size_t Length;
    T *Entry;
};

#include "../Source/Array.cpp"
#endif