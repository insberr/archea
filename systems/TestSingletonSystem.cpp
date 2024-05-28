//
// Created by insberr on 5/27/24.
//

#include "TestSingletonSystem.h"

SystemBaseImpl(TestSystem)

namespace TestSystem {
    /* Private Variables And Functions */
}

int TestSystem::InstanceClass::Setup() { return 0; }
void TestSystem::InstanceClass::Init() {}
void TestSystem::InstanceClass::Update(float dt) {}
void TestSystem::InstanceClass::Render() {}
void TestSystem::InstanceClass::Exit() {}
void TestSystem::InstanceClass::Done() {}

/* Public Function Implementation */


/* Private Function Implementation */
