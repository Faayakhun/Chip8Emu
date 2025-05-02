# CHIP-8 Emulator

A simple CHIP-8 emulator built with C++ and SDL2, designed to run classic CHIP-8 games from the 1970s. CHIP-8 is a virtual machine developed for playing games on early microcomputers, and this emulator recreates that functionality.

---

## 🕹️ What is CHIP-8?

CHIP-8 is a simple, interpreted programming language from the 1970s used to create video games on 8-bit systems. It has:

- 64x32 monochrome display
- 16 8-bit registers
- A 16-key hexadecimal keypad
- Stack-based subroutine handling
- Timers and basic sound

This emulator can run most standard CHIP-8 ROMs, including games like:
- Pong
- Tetris
- Space Invaders
- Brix

---

## 🛠️ Installation (Visual Studio 2022 on Windows)

### 🔧 Prerequisites
- **Visual Studio 2022 Community Edition**
- **Desktop development with C++** workload installed
- [SDL2 Development Library for Visual C++ (SDL2)](https://github.com/libsdl-org/SDL/releases) (e.g., `SDL2-devel-2.28.5-VC.zip`)

### 📦 Setup Instructions

1. **Extract SDL2**
   - Extract `SDL2` to a folder like `C:\libs\SDL2`

2. **Create a new project**
   - Open **Visual Studio 2022**
   - Create a new **Empty C++ Project**
   - Name it `Chip8Emulator`

3. **Add source files**
   - Add your `main.cpp`, `chip8.cpp`, and `chip8.h` to the project.

4. **Add SDL2 includes and libs**
   - Right-click the project > **Properties**
   - Under **C/C++ > General > Additional Include Directories**:
     ```
     C:\libs\SDL2\include
     ```
   - Under **Linker > General > Additional Library Directories**:
     ```
     C:\libs\SDL2\lib\x64
     ```
   - Under **Linker > Input > Additional Dependencies**:
     ```
     SDL2.lib; SDL2main.lib;
     ```

5. **Copy SDL2.dll**
   - Copy `SDL2.dll` from `C:\libs\SDL2\lib\x64` into your project’s output directory (`Debug` or `Release` folder).

---

## 🚀 Running the Emulator

1. **Build the project**
   - Press `Ctrl+Shift+B` or choose **Build > Build Solution**

2. **Put your ROM file in the same folder as the `.exe`**
   - Example: `Debug\pong.rom`

3. **Run from the command line**
   - Open **Command Prompt** and navigate to the build output folder (e.g., `Debug`)
   - Run:
     ```
     Chip8Emulator.exe "pong.rom"
     ```

   *(Quotes are needed if the ROM filename has spaces)*

---
