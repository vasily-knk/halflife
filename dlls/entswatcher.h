#pragma once

#include "../common/const.h"

class CBaseEntity;


struct EntsWatcher
{
    virtual ~EntsWatcher() {};

    virtual void AddEnt(CBaseEntity* ent) = 0;
    virtual void Update() = 0;

    virtual void Reset() = 0;

    static EntsWatcher &Instance();
};


