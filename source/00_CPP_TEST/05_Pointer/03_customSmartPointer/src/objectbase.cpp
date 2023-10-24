
#include "objectbase.h"

#include <cassert>
#include <sstream>

ObjectBase::ObjectBase()
{
    this->m_refCount = 1;
}

ObjectBase::~ObjectBase()
{
    if (this->m_refCount > 0)
    {
        // error
    }
}

void ObjectBase::InitializeObjectBase()
{
}

void ObjectBase::Delete()
{
    this->UnRegister(static_cast<ObjectBase*>(nullptr));
}

void ObjectBase::SetReferenceCount(int ref)
{
    this->m_refCount = ref;
}

void ObjectBase::Register(ObjectBase* o)
{
    this->m_refCount++;
}

void ObjectBase::UnRegister(ObjectBase* o)
{
    if (--this->m_refCount <= 0)
    {
        delete this;
    }
}
