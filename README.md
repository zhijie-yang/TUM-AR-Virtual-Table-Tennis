# Virtual Tennis
 
> A multiplayer AR tennis game.


Components:

- `app`.
- `libs/libtennis`.
- `libs/libtennis/tests`.
- `libs/librendering`.
- `libs/libvision`.

## Group Member
Mykola Morozov

Yifeng Li

Siyun Liang

Zhijie Yang

## Description
The virtual table tennis game is implemented in **C++** . The game uses several libraries:
1. libvision
   * Using *opencv* functions to do camera calibration and necessary computation in rotation and translation matrix.
   * Using *ArUco* to create a marker board for table and marker for racket. 
2. librendering
   * Loading video capture into *OpenGL*
   * Handling Desktop Window display and user input
   * Rendering the video capture, ball, table and rackets inside the window 
   * Drawing UI and displaying pop-ups
   * Calculating frame processing time
3. libtennis
   * Game Logic
   * Collision detection
4. libnetwork
   * Enable two-player match across local network
5. libframework
   * Define compatible data structures used for different libraries

The game assumes players using HDM head-mountded display to play table tennis game. The players first need to calibrate the camera once for the future usage. The vision manager needs to detect one of the markers for boards and the marker for racket to determine location respectively. After the detection, the table, ball and other objects are rendered. After user getting into the UI, user can start the table tennis game according to the game rule. Alternatively, they can use local network to enable multiplayer gaming.

## Dependencies

- libvision: `Opencv` and `Opencv_contrib` for image tracking
- librendering: `OpenGL`
- libnetwork: `grpc`

## Usage

Install dependencies:
```bash
sudo apt update && sudo apt install -y libgrpc++-dev protobuf-compiler # For Ubuntu/Debian
```

Clone the project:
```bash
git clone https://gitlab.com/mcmikecreations/virtualtennis.git
cd virtualtennis/
git submodule update --init --recursive
```

Create the `build` directory and configure the build system:

```bash
cmake -B build # Or `cmake -D BUILD_TESTING=OFF -B build` to skip tests. 
```

Build the project:

```bash
cmake --build build
```

Inspect the `build` directory to find the application and the tests.

Optionally, run the tests with `ctest` by typing:

```bash
cmake -E chdir build ctest
```
