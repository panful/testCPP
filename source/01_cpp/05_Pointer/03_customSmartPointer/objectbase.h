#pragma once

#include <atomic>

class ObjectBase
{
public:
    virtual void Delete();

    static ObjectBase* New()
    {
        ObjectBase* o = new ObjectBase;
        o->InitializeObjectBase();
        return o;
    }

    void InitializeObjectBase();

    virtual void Register(ObjectBase* o);

    virtual void UnRegister(ObjectBase* o);

    int GetReferenceCount()
    {
        return this->m_refCount;
    }

    void SetReferenceCount(int);

protected:
    ObjectBase();
    virtual ~ObjectBase();

    std::atomic<int32_t> m_refCount;

protected:
    ObjectBase(const ObjectBase&)
    {
    }

    void operator=(const ObjectBase&)
    {
    }
};
