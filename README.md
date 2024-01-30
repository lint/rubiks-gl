
# rubiks-gl

Rubiks-gl is an OpenGL application written in C that presents a virtual interactable Rubik's Cube. It features several controls for the cube such as rotating each of its faces or automatically generating and applying a scramble. Additionally, there are controls to move the camera, light source, and background plane. 

### Building and Running

1. Install the required development tools, libraries, and OpenGL packages
    `sudo apt install build-essential libglew-dev freeglut3-dev libxi-dev libxmu-dev`
2. In the project's root directory, build rubiks-gl using the following command:
    `make`
3. The final binary is located at `bin/rubiks-gl`, run it with:
    `make run` or `./bin/rubiks-gl`

### Key Commands

#### Rubik's Cube Turns

Key | Result 
---|---
**s**, **S** | Scramble Cube
**f** | F Turn (CW)
**u** | U Turn (CW)
**r** | R Turn (CW)
**l** | L Turn (CW)
**b** | B Turn (CW)
**d** | D Turn (CW)
**F** (shift+f) | F' Turn (CCW)
**U** (shift+u) | U' Turn (CCW)
**R** (shift+r) | R' Turn (CCW)
**L** (shift+l) | L' Turn (CCW)
**B** (shift+b) | B' Turn (CCW)
**D** (shift+d) | D' Turn (CCW)

#### Viewing Movement

Key | Result 
---|---
**\-**,  **_** | Zoom Out
**\+**, **=** | Zoom In
**Up Arrow** | Rotate Up
**Down Arrow** | Rotate Down
**Left Arrow** | Rotate Left
**Right Arrow** | Rotate Right

#### Light Movement

Key | Result 
---|---
**x** | Move in -X Direction
**X** (shift+x) | Move in +X Direction
**y** | Move in -Y Direction
**Y** (shift+y) | Move in +Y Direction
**z** | Move in -Z Direction
**Z** (shift+z) | Move in +Z Direction

#### Plane Movement

Key | Result 
---|---
**e**, **E** | Move Plane Up
**w**, **W** | Move Plane Down

#### Other

Key | Result 
---|---
**q**, **Q** | Quit
