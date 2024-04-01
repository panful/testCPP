#pragma once

#include "meta.h"
#include <type_traits>

class ObjectBase;

template <class T>
class New
{
    template <typename U>
    friend class New;
    template <typename U>
    friend class SmartPointer;
    template <typename U>
    friend class WeakPointer;

    template <typename U = T>
    static void CheckTypes() noexcept
    {
        static_assert(::detail::IsComplete<T>::value, "New<T>'s T type has not been defined. Missing include?");
        static_assert(::detail::IsComplete<U>::value, "Cannot store an object with undefined type in New. Missing include?");
        static_assert(std::is_base_of<T, U>::value, "Argument type is not compatible with New<T>'s T type.");
        static_assert(std::is_base_of<ObjectBase, T>::value, "New can only be used with subclasses of ObjectBase.");
    }

public:
    New()
        : m_object(T::New())
    {
        New::CheckTypes();
    }

    New(New&& o) noexcept
        : m_object(o.m_object)
    {
        o.m_object = nullptr;
    }

    template <typename U>
    New(New<U>&& o) noexcept
        : m_object(o.m_object)
    {
        New::CheckTypes<U>();

        o.m_object = nullptr;
    }

    ~New()
    {
        this->Reset();
    }

    void Reset()
    {
        T* obj = this->m_object;
        if (obj)
        {
            this->m_object = nullptr;
            obj->Delete();
        }
    }

    T* operator->() const noexcept
    {
        return this->m_object;
    }

    T* GetPointer() const noexcept
    {
        return this->m_object;
    }

    T* Get() const noexcept
    {
        return this->m_object;
    }

    operator T*() const noexcept
    {
        return static_cast<T*>(this->m_object);
    }

    T& operator*() const noexcept
    {
        return *static_cast<T*>(this->m_object);
    }

private:
    New(New<T> const&)            = delete;
    void operator=(New<T> const&) = delete;
    T* m_object;
};
