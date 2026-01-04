# SandSim

A simple falling sand simulation in C using SDL2.

## Features

- Left-click to place sand.
- Hold **Shift** while clicking to place walls.
- Hold **Command/Windows** while clicking to place water.
- Hold **Alt** while clicking to erase.
- Real-time sand and water physics.

## Build & Run

1. Install SDL2 development libraries.
2. Clone this repository.
3. Compile:
   ```
   cd build/ && cmake .. && cmake --build .
   ```
4. Run:
   ```
   ./falling_sand
   ```

## Controls

- **Left Mouse Drag**: Draw sand
- **Shift + Drag**: Draw wall
- **Command/Win + Drag**: Draw water
- **Alt + Drag**: Erase

Enjoy playing with the falling sand!
