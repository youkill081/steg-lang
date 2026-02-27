//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include "ByteBuffer.h"
#include "runtime/Runtime.h"

class Loader
{
private:
    static void init_variables(ByteBuffer &buffer, Runtime &runtime);
    static void init_files(ByteBuffer &buffer, Runtime &runtime);
    static void init_subtextures(ByteBuffer &buffer, Runtime &runtime);
    static void init_instructions(ByteBuffer &buffer, Runtime &runtime);
public:
    static Runtime load(ByteBuffer &buffer);
};
