
#include "smartpointerbase.h"

SmartPointerBase::SmartPointerBase() noexcept
    : m_object(nullptr)
{
}

SmartPointerBase::SmartPointerBase(ObjectBase* r)
    : m_object(r)
{
    this->Register();
}

SmartPointerBase::SmartPointerBase(ObjectBase* r, const NoReference&)
    : m_object(r)
{
}

SmartPointerBase::SmartPointerBase(const SmartPointerBase& r)
    : m_object(r.m_object)
{
    this->Register();
}

SmartPointerBase::~SmartPointerBase()
{
    ObjectBase* object = this->m_object;
    if (object)
    {
        this->m_object = nullptr;
        object->UnRegister(nullptr);
    }
}

SmartPointerBase& SmartPointerBase::operator=(ObjectBase* r)
{
    if (r != this->m_object)
    {
        SmartPointerBase(r).Swap(*this);
    }
    return *this;
}

SmartPointerBase& SmartPointerBase::operator=(const SmartPointerBase& r)
{
    if (&r != this && r.m_object != this->m_object)
    {
        SmartPointerBase(r).Swap(*this);
    }
    return *this;
}

void SmartPointerBase::Swap(SmartPointerBase& r) noexcept
{
    ObjectBase* temp = r.m_object;
    r.m_object       = this->m_object;
    this->m_object   = temp;
}

void SmartPointerBase::Register()
{
    if (this->m_object)
    {
        this->m_object->Register(nullptr);
    }
}
