# Archea
Archea is a 3D Falling Sand Simulation I am developing because I am bored and love falling sand simulations.
I also needed an excuse to learn graphics programming and to make a custom game engine.
This project has been a bit of a challenge so far and has been through multiple rewrites. See [Journey Of Archea](#journey-of-archea)

You can watch my streams of making this project on [YouTube here](https://www.youtube.com/playlist?list=PLegENDHYPkX28wjNpFBd16ikRTgV8ekCf).  
If uou don't feel like watching the streams (I don't blame you, they are long), you can watch my [dev log videos instead](https://www.youtube.com/playlist?list=PLegENDHYPkX0YnBPZnPjTTeSgQtDirN8P)

## Setup
Make sure to follow the specific OS instructions.

For VSCode, there should be a run and debug configuration already provided.

JetBrains CLion will likely automatically set everything up (I have not yet tested that lol).

### Linux
First you need to install `OpenGL` development libraries. This is really easy on linux.
```shell
sudo apt install libglew-dev libglfw3-dev libglm-dev
```

Then configure cmake in your IDE of choice.

### Windows
Install packages with vcpkg (builtin to CLion)
- glew
- glfw3
- glm

Have fun good luck

### MacOS
I have no idea, you are on your own for this. (I will look later I guess)

## Cross Compiling
I have to look up how to do these.

### Compile for Windows on Linux
TODO

### Compile for Linux on Windows
TODO

## Wording
1. Particle - Refers to a voxel which is simulated (i.e. sand, water)
2. Voxel - The cubes you see in the world, duh

## Journey Of Archea
Here is the journey I went through and some history LOL.

(I will make this better later)
Started out trying to make this in rust using [Bevy]().  
Then decided I need to just make my own engine and graphics pipeline.  
I also decided that rust and I are not compatible for this project and decided to switch to c++.  
Started learning DirectX11
Started making a [2d falling sand game in directx11]() and quickly made it 3D because I am impatient.  
Then decided my code was too messy.  
I started looking for ways to render the particles and discovered ray marching. Ray marching seemed interesting
  so I started experimenting with it in shader toys.  
I started learning opengl and messing with ray marching. Once I had a handle on that I came back here (to Archea)
  and started rewriting it in c++.  
3 Days later I have a basic engine working, ray marching working, opengl working, ImGui set up, and a very basic simulation of particles.  


I will continue to update this (hopefully)
