#pragma once

#include "objectbase.h"

class Object : public ObjectBase
{
public:
    static Object* New();

protected:
    Object();
    ~Object() override;

private:
    Object(const Object&)         = delete;
    void operator=(const Object&) = delete;
};
