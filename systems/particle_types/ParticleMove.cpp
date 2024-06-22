//
// Created by jonah on 6/22/2024.
//

#include "ParticleMove.h"

void ParticleMove::predefined::none(MoveState& lastMoveState) {
    lastMoveState.done = true;
}

void ParticleMove::predefined::up(ParticleMove::MoveState &lastMoveState)  {
    if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Up;
}

void ParticleMove::predefined::down(ParticleMove::MoveState &lastMoveState)  {
    if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Down;
}

void ParticleMove::predefined::randomLR(MoveState& lastMoveState) {
    if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Left;
}

void ParticleMove::predefined::randomFB(MoveState& lastMoveState) {
    if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Forward;
}

void ParticleMove::predefined::randomLRFB(MoveState& lastMoveState) {
    if (lastMoveState.step >= 1) lastMoveState.done = true;
    lastMoveState.step++;
    lastMoveState.positionToTry = Left + Forward;
}
