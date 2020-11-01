Alec Maughan Computer Graphics Assignment

- Source Files -----------------------------------------------------------------

All located in `src/assignment/assignment/`.

assignment.cpp
    Main source code file with most logic and the main function.

assignment.h
    Header file for assignment.cpp. Contains all includes and the Box class.

camera.h
    Camera class, adapted from the one included from LearnOpenGL.

fragment.fs
    Fragment shader.

lampFragment.fs
    Fragment shader for the light source.

lampVertex.vs
    Vertex shader for the light source.

vertex.vs
    Vertex shader.

- Compile ----------------------------------------------------------------------

In directory `build` run the following.

cmake ..
make

- Run --------------------------------------------------------------------------

In directory `build` run the following.

./bin/assignment/assignment__assignment

- Walkthrough ------------------------------------------------------------------

Use the mouse to look around. Move with W,S,A,D. Hold SHIFT to sprint.

Don't let the enemy behind you catch up to you.

Walk up to the torch to pick it up.

Interact with buttons on the wall with E.

1st door: Press the green button to open.

2nd door: Press the button with the texture matching that of the door
(Stone brick textured button).

3rd door: Press the button corresponding to the puzzle on the door
(Button labelled '3').

4th door: Press both buttons to fully open the door.

Walk up to the pedestal on the green platform to win.

Restart at any point with R.

Exit the program with ESC.

- Other Controls ---------------------------------------------------------------

Decrease/Increase light source radius with K and L respectively.

Show the camera movement bounds with B.

Toggle between perspective and parallel projection with P.

Toggle between dim and bright ambient lighting with O.
