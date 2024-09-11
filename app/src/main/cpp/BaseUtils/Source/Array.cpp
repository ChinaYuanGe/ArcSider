#include "../Header/Array.hpp"

template <typename T>
Array<T>::Array()
{
    this->Length = 0;
    this->Entry = nullptr;
}

template <typename T>
void Array<T>::Push(T *value)
{
    T *NewEntry = (T *)malloc((this->Length + 1) * sizeof(T));
    if (this->Entry != nullptr)
    {
        memcpy((void *)NewEntry, (void *)this->Entry, (this->Length) * sizeof(T));
    }
    NewEntry[this->Length] = *value;
    free(this->Entry);
    this->Entry = NewEntry;
    this->Length++;
}

template <typename T>
void Array<T>::Pop(size_t index)
{
    T *NewEntry = (T *)malloc((this->Length - 1) * sizeof(T));
    memcpy(NewEntry, this->Entry, index * sizeof(T));
    memcpy((void *)((uint64_t)NewEntry + (index * sizeof(T))), (void *)((uint64_t)this->Entry + ((index + 1)) * sizeof(T)), (this->Length - 1 - index) * sizeof(T));
    free(this->Entry);
    this->Entry = NewEntry;
    this->Length--;
}

template <typename T>
void Array<T>::Insert(size_t index, T *value)
{
    T *NewEntry = (T *)malloc((this->Length + 1) * sizeof(T));
    memcpy((void *)NewEntry, (void *)this->Entry, index * sizeof(T));
    NewEntry[index] = *value;
    memcpy((void *)((uint64_t)NewEntry + ((index + 1) * sizeof(T))), (void *)((uint64_t)this->Entry + ((index) * sizeof(T))), (this->Length - index) * sizeof(T));
    free(this->Entry);
    this->Entry = NewEntry;
    this->Length++;
}

template <typename T>
size_t Array<T>::Size()
{
    return this->Length;
}

template <typename T>
T *Array<T>::GetPointer(size_t index)
{
    return &this->Entry[index];
}

template <typename T>
T Array<T>::operator[](size_t index)
{
    return this->Entry[index];
}

template <typename T>
void Array<T>::Clear()
{
    free(this->Entry);
    this->Entry = nullptr;
    this->Length = 0;
}

template <typename T>
T Array<T>::First()
{
    return this->Entry[0];
}

template <typename T>
T Array<T>::Last()
{
    return this->Entry[this->Length - 1];
}