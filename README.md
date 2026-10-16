# Retro Snake

A desktop Snake game written in C++ with [raylib](https://www.raylib.com/). The project demonstrates object-oriented programming concepts including encapsulation, inheritance, and polymorphism.

## Gameplay

Use the arrow keys to steer the snake and eat the food. Avoid the walls and the snake's tail. A collision resets the round; press an arrow key to start moving again.

## Requirements

- C++14-compatible compiler
- GNU Make
- raylib 4.2.0 (the version specified by the Makefile)

The Windows build command below assumes raylib and w64devkit are installed under `C:/raylib`. If raylib is installed elsewhere, pass its location with `RAYLIB_PATH` when building.

## Build and Run on Windows

From the project directory, build the game with:

```powershell
& "C:/raylib/w64devkit/bin/mingw32-make.exe" RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=main OBJS=main.cpp BUILD_MODE=RELEASE
```

For a debug build, change `BUILD_MODE=RELEASE` to `BUILD_MODE=DEBUG`. Run the resulting executable from the project directory so the relative asset paths resolve:

```powershell
.\main.exe
```

## Project Files

- `main.cpp` - game logic and rendering
- `Graphics/food.png` - food image asset (the current game draws food as a circle)
- `Sounds/eat.mp3` and `Sounds/wall.mp3` - sound effects
- `Makefile` - raylib desktop build configuration




