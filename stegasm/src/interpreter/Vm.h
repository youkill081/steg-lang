//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include "ByteBuffer.h"
#include "runtime/Runtime.h"

class Vm
{
private:
public:
    static uint32_t run(ByteBuffer &buffer);
};
