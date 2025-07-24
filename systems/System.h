/*
 * File System.h
 * Provides an interface for systems to be created and used
 *
 * Created by insberr on 5/21/24.
 * Archea
*/

#pragma once

struct System {
    // Setup is called when the system is added to the app
    // Setup must return an int to state success or failure
    int (*Setup)() = nullptr;
    // Init is called when app.Run() is called,
    //   and before the game loop starts
    void (*Init)() = nullptr;

    // This is a pre-update function. Called before glPollEvents
    void (*PrePollEvents)() = nullptr;
    // Update is called every game loop
    void (*Update)(float dt) = nullptr;

    void (*PreRender)() = nullptr;
    // Render is called every game loop
    void (*Render)() = nullptr;
    void (*PostRender)() = nullptr;

    // Exit is called when the game loop is done
    void (*Exit)() = nullptr;
    // Done is called when the app's destructor is called
    void (*Done)() = nullptr;
};
