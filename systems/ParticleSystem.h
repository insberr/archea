//
// Created by insberr on 5/21/24.
//

#pragma once
#include "System.h"

namespace ParticleSystem {
    System AsSystem();

    /* Implementation Here */
    static float particleScale { 0.5f };
    static unsigned int chunkSize { 64 };
}
