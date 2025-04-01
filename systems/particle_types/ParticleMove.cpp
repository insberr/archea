//
// Created by jonah on 6/22/2024.
//

#include "ParticleMove.h"

#include <chrono>
#include <iostream>
#include <random>

int geRandomInt(int rangeStart, int rangeEnd) {
    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    // generator.seed();
    std::uniform_int_distribution<int> distribution(rangeStart,rangeEnd);

    return distribution(generator);
}

void ParticleMove::predefined::none(MoveState& lastMoveState) {
    lastMoveState.done = true;
}

void ParticleMove::predefined::up(ParticleMove::MoveState &lastMoveState)  {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Up;
}

void ParticleMove::predefined::down(ParticleMove::MoveState &lastMoveState)  {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Down;
}

void ParticleMove::predefined::randomLR(MoveState& lastMoveState) {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Right;
}

void ParticleMove::predefined::randomFB(MoveState& lastMoveState) {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Forward;
}

void ParticleMove::predefined::randomLRFB(MoveState& lastMoveState) {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;

    vec3 newPos = None;

    switch (geRandomInt(0, 1))
    {
    case 0: newPos += Left; break;
    case 1: newPos += Right; break;
    default: break;
    }

    switch (geRandomInt(0, 1))
    {
    case 0: newPos += Forward; break;
    case 1: newPos += Backward; break;
    default: break;
    }

    lastMoveState.positionToTry = newPos;
}

void ParticleMove::predefined::randomLR_Down(MoveState& lastMoveState) {
    lastMoveState.step++;

    switch (geRandomInt(0, 3))
    {
    case 0: lastMoveState.positionToTry = Left + Down; break;
    case 1: lastMoveState.positionToTry = Right + Down; break;
    case 2: lastMoveState.positionToTry = Forward + Down; break;
    case 3: lastMoveState.positionToTry = Backward + Down; break;
    default: lastMoveState.positionToTry = None; break;
    }
};
