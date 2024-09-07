//
// Created by jonah on 6/22/2024.
//

#include "ParticleMove.h"
#include <random>

int geRandomInt(int rangeStart, int rangeEnd) {
    std::default_random_engine generator;
    generator.seed(0);
    std::uniform_int_distribution<int> distribution(rangeStart,rangeEnd);//note the min and max parameters are inclusive here

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
    lastMoveState.positionToTry = Left;
}

void ParticleMove::predefined::randomFB(MoveState& lastMoveState) {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Forward;
}

void ParticleMove::predefined::randomLRFB(MoveState& lastMoveState) {
    // if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Left + Forward;
}

void ParticleMove::predefined::randomLR_Down(MoveState& lastMoveState) {
    lastMoveState.step++;

    switch (geRandomInt(0, 1))
    {
    case 0: lastMoveState.positionToTry = Left + Down; break;
    case 1: lastMoveState.positionToTry = Right + Down; break;
    default: lastMoveState.positionToTry = None; break;
    }
};
