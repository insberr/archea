//
// Created by insberr on 5/27/24.
//

#include "TestSingletonSystem.h"

void TestSystem::InstanceClass::Init() {}
void TestSystem::InstanceClass::Update(float dt) {
    FooBar(1, dt);
}
void TestSystem::InstanceClass::Render() {}
void TestSystem::InstanceClass::Exit() {}

int TestSystem::FooBar(int x, int y) {
    return x + y;
}
