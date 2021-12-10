# Computer Graphics Assignment

A small C++ OpenGL video game. The player controls a camera with mouse and
keyboard and must get past a series of simple door puzzles while not getting
caught by the enemy.

---

## Compile & Run

### Environment

Tested in a Ubuntu 20.04 Virtualbox VM. The following commands will install
all dependencies needed for development and running.

```sh
sudo apt update
sudo apt-get update
sudo apt install gcc
sudo apt install cmake
sudo apt install g++
sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
sudo apt-get install libassimp-dev
sudo apt-get install libxinerama-dev
sudo apt-get install libxi-dev
sudo apt-get install libxcursor-dev
sudo apt-get install libxxf86vm-dev
```

### Compile

Run the following.

```sh
# Make the build directory and go into it
# (Only needed the first time)
mkdir build
cd build
# Generate makefile and make it
cmake ..
make
```

### Run

Under `build/`.

```sh
./bin/assignment/assignment__assignment
```

If all goes well, the game window should pop up. If in a VM, you may
need to change the mouse settings for the mouse input to work
correctly, e.g. disable mouse integration in Virtualbox.

---

## How to Play

Use the mouse to look around. Move with `W`,`S`,`A`,`D`. Hold
`SHIFT` to sprint.

### Walkthrough

Don't let the enemy behind you catch up to you.

Walk up to the torch to pick it up.

Interact with buttons on the wall with `E`.

1st door: Press the green button to open.

2nd door: Press the button with the texture matching that of the door
(Stone brick textured button).

3rd door: Press the button corresponding to the puzzle on the door
(Button labelled '3').

4th door: Press both buttons to fully open the door.

Walk up to the pedestal on the green platform to win.

Restart at any point with `R`.

Exit the program with `ESC`.

### Other Controls

Decrease/Increase light source radius with `K` and `L` respectively.

Show the camera movement bounds with `B`.

Toggle between perspective and parallel projection with `P`.

Toggle between dim and bright ambient lighting with `O`.

---

## Project Structure

### Source Files

All located in `src/assignment/assignment/`.

#### `assignment.cpp`

Main source code file with most logic and the main function.

#### `assignment.h`

Header file for assignment.cpp. Contains all includes and the Box class.

#### `camera.h`

Camera class, adapted from the one included from LearnOpenGL.

#### `fragment.fs`

Fragment shader.

#### `lampFragment.fs`

Fragment shader for the light source.

#### `lampVertex.vs`

Vertex shader for the light source.

#### `vertex.vs

Vertex shader.
