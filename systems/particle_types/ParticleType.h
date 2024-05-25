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
