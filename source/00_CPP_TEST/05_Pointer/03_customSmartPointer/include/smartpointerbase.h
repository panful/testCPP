#pragma once

#include "objectbase.h"

class SmartPointerBase
{
public:
    SmartPointerBase() noexcept;
    SmartPointerBase(ObjectBase* r);
    SmartPointerBase(const SmartPointerBase& r);

    SmartPointerBase(SmartPointerBase&& r) noexcept
        : m_object(r.m_object)
    {
        r.m_object = nullptr;
    }

    ~SmartPointerBase();

    SmartPointerBase& operator=(ObjectBase* r);
    SmartPointerBase& operator=(const SmartPointerBase& r);

    ObjectBase* GetPointer() const noexcept
    {
        return this->m_object;
    }

protected:
    class NoReference
    {
    };

    SmartPointerBase(ObjectBase* r, const NoReference&);

    ObjectBase* m_object;

private:
    void Swap(SmartPointerBase& r) noexcept;
    void Register();
};

//----------------------------------------------------------------------------
#define _SMART_POINTER_BASE_DEFINE_OPERATOR(op)                                            \
    inline bool operator op(const SmartPointerBase& l, const SmartPointerBase& r)          \
    {                                                                                      \
        return (static_cast<void*>(l.GetPointer()) op static_cast<void*>(r.GetPointer())); \
    }                                                                                      \
    inline bool operator op(ObjectBase* l, const SmartPointerBase& r)                      \
    {                                                                                      \
        return (static_cast<void*>(l) op static_cast<void*>(r.GetPointer()));              \
    }                                                                                      \
    inline bool operator op(const SmartPointerBase& l, ObjectBase* r)                      \
    {                                                                                      \
        return (static_cast<void*>(l.GetPointer()) op static_cast<void*>(r));              \
    }
/**
 * Compare smart pointer values.
 */
_SMART_POINTER_BASE_DEFINE_OPERATOR(==)
_SMART_POINTER_BASE_DEFINE_OPERATOR(!=)
_SMART_POINTER_BASE_DEFINE_OPERATOR(<)
_SMART_POINTER_BASE_DEFINE_OPERATOR(<=)
_SMART_POINTER_BASE_DEFINE_OPERATOR(>)
_SMART_POINTER_BASE_DEFINE_OPERATOR(>=)

#undef _SMART_POINTER_BASE_DEFINE_OPERATOR
