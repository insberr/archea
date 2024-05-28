//
// Created by insberr on 5/21/24.
//

#pragma once

#define SystemBaseHeader(namespaceName) \
namespace namespaceName { \
    class InstanceClass : public System { \
    public: \
        InstanceClass() = default; \
        ~InstanceClass() override = default; \
        int Setup() override; \
        void Init() override; \
        void Update(float dt) override; \
        void Render() override; \
        void Exit() override; \
        void Done() override; \
    }; \
    InstanceClass& Instance(); \
    System* AsSystem(); }

#define SystemBaseImpl(namespaceName) \
namespace namespaceName { InstanceClass self {}; } \
namespaceName::InstanceClass& namespaceName::Instance() { return self; } \
System *namespaceName::AsSystem() { return reinterpret_cast<System*>(&self); }

class System {
public:
    System() = default;
    virtual ~System() = default;

    // Setup is called when the system is added to the app
    // Setup must return an int to state success or failure
    virtual int Setup() = 0;

    // Init is called when app.Run() is called,
    //   and before the game loop starts
    virtual void Init() = 0;

    // Update is called every game loop
    virtual void Update(float dt) = 0;

    // Render is called every game loop
    virtual void Render() = 0;

    // Exit is called when the game loop is done
    virtual void Exit() = 0;

    // Done is called when the app's destructor is called
    virtual void Done() = 0;
};
