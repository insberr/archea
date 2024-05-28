//
// Created by insberr on 5/27/24.
//

#pragma once

//class DemoSystem {
//public:
//    virtual void Init() = 0;
//    virtual void Update(float dt) = 0;
//    virtual void Render() = 0;
//    virtual void Exit() = 0;
//};
//
//#define Single(n) \
//static n& get() { \
//    static n instance; \
//    return instance;                     \
//} \
//private: \
//TestSingletonSystem() = default; \
//~TestSingletonSystem() = default; \
//public:
//
//#define DemoSystemVirtualFuncs() \
//void Init() override {}; \
//void Update(float dt) override {};\
//void Render() override {}; \
//void Exit() override {};
//
//class TestSingletonSystem : public DemoSystem {
//public:
//    Single(TestSingletonSystem)
//    DemoSystemVirtualFuncs()
//};
//
//namespace TestSingletonSys {
//    TestSingletonSystem& s = TestSingletonSystem::get();
//    // Provide System functions that all systems have and other functions you wish to implement
//    void Init() { s.Init(); };
//    void Update(float dt) { s.Update(dt); };
//    void Render() { s.Render(); };
//    void Exit() { s.Exit(); };
//    // Add other implemented functions
//}
//
//int test() {
//    // Imagine this
//    // app.AddSystem()
//
//    // Now imagine being somewhere else in the engine
//    TestSingletonSys::Init();
//
//    return 0;
//}

/* Attempt 2 */

class DemoSystem {
public:
    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Exit() = 0;
};

namespace TestSystem {
    class InstanceClass : public DemoSystem {
    public:
        InstanceClass() = default;
        ~InstanceClass() = default;

        void Init() override;
        void Update(float dt) override;
        void Render() override;
        void Exit() override;
    };

    InstanceClass self{};

    InstanceClass& Instance() { return self; };
    DemoSystem* AsSystem() { return reinterpret_cast<DemoSystem*>(&self); };

    // Actual system implementation
    int FooBar(int x,int y);
}

// demo app just for concept
struct a{ void AddSystem(void* v) { v; }; };

int test() {

    a app{};

    app.AddSystem(TestSystem::AsSystem());

    // Example of calling system function outside the system for whatever reason.
    TestSystem::Instance().Update(0.0f);

    // Calling function for the system;
    TestSystem::FooBar(1, 1);

    return 0;
}
