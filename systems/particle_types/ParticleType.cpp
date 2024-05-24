//
// Created by insberr on 5/23/24.
//

struct ParticleType {
    enum State {
        Solid,
        Liquid,
        Gas,
    };

    ParticleType::State state;
    unsigned char color[4];
    float temperature;

};



// sandparticle.cpp
const ParticleType SandParticle = {
    .state = ParticleType::State::Solid,
    .color = {200, 150, 10, 255},
    .temperature = 30.0f
};
