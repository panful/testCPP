#pragma once

#include "meta.h"
#include "new.h"
#include "smartpointerbase.h"
#include <type_traits>
#include <utility>

template <class T>
class SmartPointer : public SmartPointerBase
{
    template <typename U = T>
    static void CheckTypes() noexcept
    {
        static_assert(::detail::IsComplete<T>::value, "SmartPointer<T>'s T type has not been defined. Missing include?");
        static_assert(::detail::IsComplete<U>::value, "Cannot store an object with undefined type in SmartPointer. Missing include?");
        static_assert(std::is_base_of<T, U>::value, "Argument type is not compatible with SmartPointer<T>'s T type.");
        static_assert(std::is_base_of<ObjectBase, T>::value, "SmartPointer can only be used with subclasses of ObjectBase.");
    }

public:
    SmartPointer() noexcept
        : SmartPointerBase()
    {
    }

    SmartPointer(const SmartPointer& r)
        : SmartPointerBase(r)
    {
    }

    template <class U>
    SmartPointer(const SmartPointer<U>& r)
        : SmartPointerBase(r)
    {
        SmartPointer::CheckTypes<U>();
    }

    SmartPointer(SmartPointer&& r) noexcept
        : SmartPointerBase(std::move(r))
    {
    }

    template <class U>
    SmartPointer(SmartPointer<U>&& r) noexcept
        : SmartPointerBase(std::move(r))
    {
        SmartPointer::CheckTypes<U>();
    }

    SmartPointer(T* r)
        : SmartPointerBase(r)
    {
        SmartPointer::CheckTypes();
    }

    template <typename U>
    SmartPointer(const New<U>& r)
        : SmartPointerBase(r.m_object)
    {
        SmartPointer::CheckTypes<U>();
    }

    template <typename U>
    SmartPointer(New<U>&& r) noexcept
        : SmartPointerBase(r.m_object, SmartPointerBase::NoReference {})
    {
        SmartPointer::CheckTypes<U>();

        r.m_object = nullptr;
    }

    SmartPointer& operator=(const SmartPointer& r)
    {
        this->SmartPointerBase::operator=(r.GetPointer());
        return *this;
    }

    template <class U>
    SmartPointer& operator=(const SmartPointer<U>& r)
    {
        SmartPointer::CheckTypes<U>();

        this->SmartPointerBase::operator=(r.GetPointer());
        return *this;
    }

    template <typename U>
    SmartPointer& operator=(const New<U>& r)
    {
        SmartPointer::CheckTypes<U>();

        this->SmartPointerBase::operator=(r.m_object);
        return *this;
    }

    template <typename U>
    SmartPointer& operator=(U* r)
    {
        SmartPointer::CheckTypes<U>();

        this->SmartPointerBase::operator=(r);
        return *this;
    }

    T* GetPointer() const noexcept
    {
        return static_cast<T*>(this->m_object);
    }

    T* Get() const noexcept
    {
        return static_cast<T*>(this->m_object);
    }

    operator T*() const noexcept
    {
        return static_cast<T*>(this->m_object);
    }

    T& operator*() const noexcept
    {
        return *static_cast<T*>(this->m_object);
    }

    T* operator->() const noexcept
    {
        return static_cast<T*>(this->m_object);
    }

    static SmartPointer<T> New()
    {
        return SmartPointer<T>(T::New(), NoReference());
    }

protected:
    SmartPointer(T* r, const NoReference& n)
        : SmartPointerBase(r, n)
    {
    }
};

#define _SMART_POINTER_DEFINE_OPERATOR(op)                                      \
    template <class T, class U>                                                 \
    inline bool operator op(const SmartPointer<T>& l, const SmartPointer<U>& r) \
    {                                                                           \
        return (l.GetPointer() op r.GetPointer());                              \
    }                                                                           \
    template <class T, class U>                                                 \
    inline bool operator op(T* l, const SmartPointer<U>& r)                     \
    {                                                                           \
        return (l op r.GetPointer());                                           \
    }                                                                           \
    template <class T, class U>                                                 \
    inline bool operator op(const SmartPointer<T>& l, U* r)                     \
    {                                                                           \
        return (l.GetPointer() op r);                                           \
    }                                                                           \
    template <class T, class U>                                                 \
    inline bool operator op(const New<T>& l, const SmartPointer<U>& r)          \
    {                                                                           \
        return (l.GetPointer() op r.GetPointer());                              \
    }                                                                           \
    template <class T, class U>                                                 \
    inline bool operator op(const SmartPointer<T>& l, const New<U>& r)          \
    {                                                                           \
        return (l.GetPointer() op r.GetPointer);                                \
    }

/**
 * Compare smart pointer values.
 */
_SMART_POINTER_DEFINE_OPERATOR(==)
_SMART_POINTER_DEFINE_OPERATOR(!=)
_SMART_POINTER_DEFINE_OPERATOR(<)
_SMART_POINTER_DEFINE_OPERATOR(<=)
_SMART_POINTER_DEFINE_OPERATOR(>)
_SMART_POINTER_DEFINE_OPERATOR(>=)

#undef _SMART_POINTER_DEFINE_OPERATOR
