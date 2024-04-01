#include "object.h"
#include <stdexcept>

Object* Object::New()
{
    Object* ret = new Object;
    ret->InitializeObjectBase();
    return ret;
}

Object::Object()
{
}

Object::~Object()
{
    if (this->m_refCount > 0)
    {
        // error
    }
}
