//
// Created by insberr on 5/21/24.
//

#include "../System.h"

class triangl_render : public System {
public:
    triangl_render();
    ~triangl_render();

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;
};
