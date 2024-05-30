//
// Created by insberr on 5/27/24.
//

#include "TestSingletonSystem.h"

namespace TestSystem {
    /* System Function Declarations */
    int Setup();
    void Init();
    void Update(float dt);
    void Render();
    void Exit();
    void Done();
    System AsSystem() {
        return {
            Setup,
            Init,
            Update,
            Render,
            Exit,
            Done
        };
    }

    /* Private Variables And Functions */
}

int TestSystem::Setup() { return 0; }
void TestSystem::Init() {}
void TestSystem::Update(float dt) {}
void TestSystem::Render() {}
void TestSystem::Exit() {}
void TestSystem::Done() {}

/* Public Function Implementation */


/* Private Function Implementation */
