//
// Created by jonah on 6/22/2024.
//

#pragma once
#include <glm/vec3.hpp>

namespace ParticleMove {
    using namespace glm;

    // no move
    const vec3 None = vec3(0);
    // y -1
    const vec3 Down = vec3(0, -1, 0);
    // y +1
    const vec3 Up = vec3(0, 1, 0);
    // x -1
    const vec3 Left = vec3(-1, 0, 0);
    // x +1
    const vec3 Right = vec3(1, 0, 0);
    // z -1
    const vec3 Forward = vec3(0, 0, -1);
    // z +1
    const vec3 Backward = vec3(0, 0, 1);

    struct MoveState {
        bool done = false;
        int step = 0;
        glm::vec3 positionToTry = None;
    };

    namespace predefined {
        void none(MoveState& lastMoveState);

        void up(MoveState& lastMoveState);

        void down(MoveState& lastMoveState);

        void randomLR(MoveState& lastMoveState);

        void randomFB(MoveState& lastMoveState);

        void randomLRFB(MoveState& lastMoveState);
    }
}
