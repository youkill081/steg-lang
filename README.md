# Steg

<p align="center">
    <img src="./doc/img/logo_steg.png" alt="Steg Logo" width="400">
</p>

<div align="center">

**Hide code in images. Run them.**

[![C++](https://img.shields.io/badge/C++-23-00599C?style=flat-square&logo=c%2B%2B)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-4.0+-064F8C?style=flat-square&logo=cmake)](https://cmake.org/)
[![Plaforms](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)]()
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)]()
[![fr](https://img.shields.io/badge/lang-fr-red.svg)](https://github.com/youkill081/steg-lang/blob/main/README.fr.md)

</div>

---

## Overview

**Steg** is much more than a steganography tool; it's a complete compilation toolchain whose target code isn't binary, but a **PNG image** !

Write code in **Steg** (high level language), compile it, and inject it silently into the pixels of an image. The resulting image is visually almost identical to the original, but executable with the StegVM interpreter !

```
   Steg Code (.steg) <- Your code !
        │
        ▼
  ┌─────────────────────────────────────────────────────┐
  │              Steg Compiler                          │
  │   Lexer → Parser → AST → Semantics → IR → Backend   │
  └─────────────────────────────────────────────────────┘
        │
        ▼
  StegASM Code  (.stegasm) <- You can also write directly in StegASM !
        │
        ▼
  ┌─────────────────────────────────────────────────────┐
  │                StegASM Assembly                     │
  └─────────────────────────────────────────────────────┘
        │
        ▼
  StegASM Bytecode
        │
        ▼
  ┌─────────────────┐        ┌──────────────┐
  │  Steganography  │ <----  │  PNG Image   │
  │  (injection)    │        │  (host)      │
  └─────────────────┘        └──────────────┘
        │
        ▼
  PNG Image  (executable) ← hidden program here
        │
        ▼
  ┌─────────────────┐
  │  Steg VM        │ <---- Interpreter
  └─────────────────┘
```

## Demo

<p align="center">
    <img src="./doc/demo-wolfensteg.gif">
    <br>
    <b>Load WolfenSteg from the image and play it.</b>
</p>

<p align="center">
    <img src="./doc/demo-tetris.gif">
    <br>
    <b>Demo of Tetris (game created directly in StegASM)</b>
</p>

---

## Table of contents

<!-- TOC -->
* [Steg](#steg)
  * [Overview](#overview)
  * [Demo](#demo)
  * [Table of contents](#table-of-contents)
  * [Project component](#project-component)
  * [Compilation](#compilation)
    * [Compilation options](#compilation-options)
      * [Instruction counter](#instruction-counter)
  * [Usage - Steg](#usage---steg)
    * [Run a program](#run-a-program)
    * [Build an executable image](#build-an-executable-image)
    * [run an image](#run-an-image)
    * [debug mode](#debug-mode)
  * [The steg language](#the-steg-language)
  * [StegASM the low-level language](#stegasm-the-low-level-language)
  * [StegLSP - the language server](#steglsp---the-language-server)
  * [Examples](#examples)
    * [WolfenSteg - Steg](#wolfensteg---steg)
      * [Performance and capacity](#performance-and-capacity)
      * [Play](#play)
    * [Tetris - StegASM](#tetris---stegasm)
  * [Thanks](#thanks)
<!-- TOC -->

---

## Project component

| Binary      | Description                                                                                                                      |
|-------------|----------------------------------------------------------------------------------------------------------------------------------|
| **Steg**    | The unified entry point for the project allows launching Steg/StegASM, executing within images, and also building within images! |
| **StegLSP** | LSP server for .steg files, directly connected to the steg compiler!                                                             |

Although primarily built from scratch, the project incorporates a few external libraries as submodules :  
- [**stb**](https://github.com/nothings/stb) - Read/Write pixels for steganography, also handle multiple image types
- [**raylib**](https://www.raylib.com/) - Graphics engine, windows, inputs...
- [**lsp-framework**](https://github.com/leon-bckl/lsp-framework) - LSP server base framework
- [**googletest**](https://github.com/google/googletest) - Testing framework

---

## Compilation

> [CMake 4.0+](https://cmake.org/download/) is required.

The program was designed for **Windows** and **Linux** and is cross-platform (created images are compatible with both platforms).  
On Window it is recommended to use [MinGW](https://www.mingw-w64.org/) to have the best performance in the interpreter.


````shell
git submodule update --init --recursive
cmake -B build -DENABLE_INSTRUCTION_COUNTER=OFF
cmake --build build --config Release
````
### Compilation options

#### Instruction counter

An instructions-per-second (IPS) counter is available. It displays every second, the number of instructions that have been executed.    
It was designed to add as little overhead as possible, but it is disabled by default.    
To enable it, just add the flag `-DENABLE_INSTRUCTION_COUNTER=ON`.  

---

## Usage - Steg

`Steg` is the main binary of the project. It plays a multitude of roles such as compiling or launching images.

````
USAGE:
  steg run <file.stegasm|file.steg> [-d]
  steg build <file.stegasm|file.steg> <input.png> <output.png>
  steg run_img <img.png>

OPTIONS:
  -d    Enable debug mode for compiler (Display, reg allocation, IR and AST code)
````

### Run a program

Compiles and executes a source file directly, without using an image: 

````shell
steg run mon_programme.steg
steg run mon_programme.stegasm
````

The launch is directly compatible with **.steg** and **.stegasm** files.

### Build an executable image

Compiles a program (Steg or StegASM) and injects it into a PNG image:
 
````shell
steg build mon_programme.steg input.png output.png
steg build mon_programme.stegam input.png output.png
````

> The input image does not have to be a .png but the output image **must** be a **.png**.

### run an image

Extract and execute a program from an image : 
````shell
steg run_img img.png
steg img.png # Short version
````

> On Windows, it is possible to select "Run with" on the image to launch the program.

### debug mode

the flag `-d` activates a full display of the compilation pipeline:

```shell
steg run mon_programme.steg -d
steg build mon_programme.steg input.png output.png -d
```

The program will display the following information 
- The generated IR
- The register allocation
- The assembler

Warning: Displaying this information can become very resource-intensive with large programs!

---

## The steg language

**Steg** is the high-level language of the project. It is compiled into StegASM via a full compilation pipeline :
````shell
Lexer -> Parseur -> AST -> semantics analysis -> IR -> StegASM Backend 
````

Sources files use `.steg` extension.

> A steg documentation is available [here](./doc/steg.md).

--- 

## StegASM the low-level language

**StegASM** is the low-level language of the project. It is represents the instruction set available in the StegVM VM.   
Heavily inspired by the "standard" assembler, it exposes the registers and instructions of StegVM.

Sources file use `.stegasm` extension.

> A StegASM documentation is available [here](./doc/stegasm.md).

---

## StegLSP - the language server

`StegLSP` is a **LSP** server for **.steg** files.  
Thanks to this LSP server, you can have the following on most modern IDEs: 

- **Syntax Coloring**
- **Semantics Analysis** -> flow control, types, parameters, etc...
- **Warnings** : Like implicit conversions

--- 

## Examples

### WolfenSteg - Steg

wolfenSteg is a Ray-casting 3D game engine entirely written in Steg.  
This is the technical showcase of the project, proving that the VM and the compilation chain can handle intensive mathematical calculations in real time.

![WolfenSteg](./examples/wolfensteg/wolfensteg.png)
**This image contains the game**

#### Performance and capacity

- Real-time ray-casting: The engine runs smoothly at 60 FPS.
- Raw power: The StegVM VM achieves peak performance of 250 million instructions per second (IPS).

Gameplay : 
- Select a random level at game launch
- Colision system and movement handling
- Can open doors and go through them
- Basic fighting system to kill enemies (you can switch weapons with the **1** - **2** - **3** - **4** keys)

#### Play

You can either launch the WolfenSteg.png image directly :  
````shell
steg run_img ./examples/wolfensteg/wolfensteg.png
````

Or you can compile it yourself ! : 
````shell
steg run ./examples/wolfensteg/WolfenSteg.steg
````

### Tetris - StegASM

Tetris is the demonstration of **StegASM**.

![Tetris](./examples/tetris/tetris.png)  

The code **StegASM** of the Tetris is available here : [./examples/tetris/tetris.stegasm](./examples/tetris/tetris.stegasm)

You can therefore launch the image directly :
````shell
steg run_img ./examples/tetris/tetris.png
````

Or compile it yourself :
````shell
steg run ./examples/tetris/tetris.stegasm
````

> ⚠️ Run the "run" commands from the **repo root**, otherwise the `.stegasm` imports will be invalid.

## Thanks

Many thanks to Ewan Clein for the Steg logo design -> https://www.linkedin.com/in/ewan-clein-architecture/