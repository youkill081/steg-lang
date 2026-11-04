# StegASM

StegASM is the assembly language of the **StegVM**. It mixes low-level instructions (mov, add, sub...)
and high-level instructions (window, file, vector...).

StegVM consumes StegASM directly as bytecode. The transformation process 
is explained in the [Developer - Compilation](#compilation) section.

---

## Table of content

<!-- TOC -->
* [StegASM](#stegasm)
  * [Table of content](#table-of-content)
  * [User documentation](#user-documentation)
    * [Structure of a .stegasm file](#structure-of-a-stegasm-file)
      * [.text Section](#text-section-)
      * [.data Section](#data-section)
        * [Variables types](#variables-types)
        * [Using variables](#using-variables)
      * [.file Section](#file-section)
      * [.sub_textures Section](#sub_textures-section)
      * [.import Section](#import-section)
    * [Registries](#registries)
    * [Labels and jumps](#labels-and-jumps)
    * [Comments](#comments)
    * [List of operands](#list-of-operands)
  * [Developer documentation](#developer-documentation)
    * [Implementation](#implementation)
    * [Compilation](#compilation)
      * [Binary file format](#binary-file-format)
        * [Variables](#variables)
        * [Files](#files)
        * [Sub_textures](#sub_textures)
        * [Instructions](#instructions)
    * [List of StegASM operands](#list-of-stegasm-operands)
      * [Operand types](#operand-types)
      * [Load / Store](#load--store)
      * [Integer Arithmetic](#integer-arithmetic)
      * [Floating-point arithmetic](#floating-point-arithmetic)
        * [Integer <-> Floating Point Conversion](#integer---floating-point-conversion)
      * [Comparison and conditional jumps](#comparison-and-conditional-jumps)
      * [Display and debug](#display-and-debug)
      * [Window](#window)
        * [Creation and configuration](#creation-and-configuration)
        * [Drawing](#drawing)
        * [Framebuffer](#framebuffer)
        * [Keyboard inputs](#keyboard-inputs-)
        * [Mouse inputs](#mouse-inputs)
      * [Image](#image)
      * [Files](#files-1)
        * [Files handling](#files-handling)
        * [Cursor](#cursor)
        * [Reading](#reading)
        * [Writing and adding](#writing-and-adding)
      * [Clocks](#clocks)
<!-- TOC -->

---

## User documentation

> ⚠️ Coding in StegASM could be tedious but entirely possible (cf: [Tetris](../examples/tetris/tetris.stegasm))).  
> It is strongly recommended to develop in **Steg** (the project's high-level language), which compiles directly to StegASM.  
> If you still wish to code in StegASM, carefully read this documentation and take inspiration from the existing Tetris.

---

### Structure of a .stegasm file

A `.stegasm` file can be composed of **5 sections** different, each optional :

| Section         | Rôle                                 |
|-----------------|--------------------------------------|
| `.text`         | Program code (instructions + labels) |
| `.data`         | Initialised variables                |
| `.file`         | Encoded file in the binary           |
| `.sub_textures` | Sub-textures from spritesheets       |
| `.import`       | Importing other `.stegasm` files     |

---

#### .text Section 


This is the **heart** of your program. Execution begins with the **first instruction** of this section.      
It consists of a series of **StegASM instructions** and **labels**.

```asm
section .text
    LOAD_8 R1, 10   ; Load 10 in R1
    LOAD_8 R2, 20   ; Load 20 in R2
    ADD R3, R1, R2  ; R3 = R1 + R2 (= 30)
    DISPLAY_N R3    ; Display 30
    HALT            ; Stop the program cleanly
```

---

#### .data Section

This section contains all the **initialized variables** of your program.  
Each variable is a list of **words** terminated automatically by a `'\0'` (null terminator).

```asm
section .data
    ma_variable DB 5        ; 8 bits variable initialised at 5
    ma_string DB "coucou"   ; String
    mix DB "co" 23 47 "a"   ; Mix of both
```

In memory, this gives :

| Variable      | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|---------------|--------|--------|--------|--------|--------|--------|--------|
| `ma_variable` | 5      | `\0`   |        |        |        |        |        |
| `ma_string`   | `'c'`  | `'o'`  | `'u'`  | `'c'`  | `'o'`  | `'u'`  | `'\0'` |
| `mix`         | `'c'`  | `'o'`  | 23     | 47     | `'a'`  | `'\0'` |        |

##### Variables types

| Type | Size of a word | Declaration        |
|------|----------------|--------------------|
| `DB` | 8 bits         | `ma_var DB 42`     |
| `DW` | 16 bits        | `ma_var DW 1000`   |
| `DD` | 32 bits        | `ma_var DD 100000` |

The type changes the **size of each word** stored in memory. It is important to choose the correct type depending on the range of values to be stored.

##### Using variables

Variables are used via their **address**. Two syntaxes exist :

- `[ma_variable]` : Load the **value** contains at `ma_variable` address.
- `ma_variable` : Load the **address** of `ma_variable` her-self (useful for strings and arrays).

```asm
section .data
    compteur DW 10
    message DB "hello"

section .text
    LOAD_16 R1, [compteur]  ; R1 = 10 (value at the address)
    DISPLAY_N R1            ; Display 10

    LOAD_32 R1, message     ; R1 = address of "message"
    ADD R1, 1               ; R1 points to the 2nd character
    DISPLAY_C R1            ; Display 'e'
```

---

#### .file Section

The `.file` section is used to **encode files directly in the binary**.  
Each file is associated with a **descriptor** (symbolical identifier) used by the VM instructions (textures, fonts...).

```asm
section .files
    icon "examples/tetris/ressources/icon.png" // Icon of an application
    tetrisNesFont "examples/tetris/ressources/tetris-nes-front.ttf" // A font
    backgroundImage "examples/tetris/ressources/background.png" // Background image
```

These descriptors can then be passed directly to certain instructions:

```asm
section .text
    WINDOW_SET_ICON icon
    WINDOW_SET_FONT tetrisNesFont
    WINDOW_DRAW_TEXTURE R0 R1 backgroundImage ; (here R0 and R1 are the coordinates)
```

---

#### .sub_textures Section

The `.sub_textures` section is used to **natively encode spritesheets** in the binary.  
Each sub_texture is defined by a **source file** and a **rectangle** `(X, Y, W, H)`.

```asm
section .files
    spritesheet "./mon_spritesheet.png" ; Image of the spritesheet

section .sub_textures
    player_idle spritesheet 0 0 16 16
    player_run spritesheet 16 0 16 16
    enemy spritesheet 0 16 16 16
```

Each sub-texture receives an independent **descriptor** and can be used as a texture in its own right in `WINDOW_DRAW_TEXTURE` instructions, without an additional image being stored in the binary.

---

#### .import Section

The `.import` section is used to **split a program into multiple files** and **regroup them** at compilation time.

```asm
section .import
    "examples/tetris/utils/number_in_string.stegasm"
    "examples/tetris/game/lines.stegasm"
    "examples/tetris/game/score.stegasm"
```

> ⚠️ Import paths are **relative to the execution position of the compilation command**.

---

### Registries

StegVM exposes **32 general registers**, named from `R0` to `R31`.  
They are interchangeable: no register has a reserved role imposed by the VM.  
By convention, it is advisable to use the registers in this way (but this is not mandatory):   
- R0 -> Return value of a function  
- R1-R6 -> Function parameter  

> The Steg compiler uses these registers in a very specific way :)

```asm
LOAD_8 R0, 42       ; R0 = 42
LOAD_16 R1, 1000    ; R1 = 1000
ADD R2, R0, R1      ; R2 = R0 + R1
```

> ⚠️ Registers are not saved automatically during a `CALL`. If a function modifies a register you are using, it is up to you to save it (with a PUSH/POP, for example).

---

### Labels and jumps

A **label** is an identifier followed by `:` which marks a position in the code.
It serves as the target for jump instructions (`JMP`, `JE`, `JNE`, `JB`...).

```asm
section .text
    LOAD_8 R0, 0

loop:
    ADD R0, 1
    CMP R0, 10
    JB loop       ; Jumps to "loop" while R0 < 10

    DISPLAY_N R0    ; Display 10
    HALT
```

Labels also work with `CALL` / `RET` to structure code into **functions** :

```asm
section .data
    msg DB "Hello world !"

section .text
    CALL display_hello   ; -> Jump to "display_hello"
    HALT

display_hello:
    LOAD_32 R0, msg
    DISPLAY_STR R0
    RET                     ; Return to the instruction jump after "CALL display_hello"
```

---

### Comments

The comments start with `;` and extends to the end of the line.

```asm
LOAD_8 R0, 5 ; Ceci est un commentaire
; Ligne entière en commentaire
```

---

### List of operands

The complete list of available operands (operands, accepted types, behaviors) is available in the **[developer documentation - List of operands](#liste-des-opérandes-stegasm)**.

---

## Developer documentation

---

### Implementation

StegASM is entirely developed in **C++ from scratch**. The only external library used is [raylib](https://github.com/raysan5/raylib) for window management.    
The implementation is located in the [stegasm/src](../stegasm/src) folder.


Important files and folders :  

| File / Folder                                                       | Role                                                  |
|---------------------------------------------------------------------|-------------------------------------------------------|
| [`instructions.h`](../stegasm/src/instructions.h)                   | Definition of **all** StegASM instructions            |
| [`interpreter/`](../stegasm/src/interpreter)                        | StegVM implémentation                                 |
| [`interpreter/Vm.cpp`](../stegasm/src/interpreter/Vm.cpp)           | Main loop of the VM                                   |
| [`assembler/`](../stegasm/src/assembler)                            | Implementation of the assembler (StegASM -> ByteCode) |
| [`assembler/Assembler.cpp`](../stegasm/src/assembler/Assembler.cpp) | Base of the assembly                                  |

---

### Compilation

#### Binary file format

The binary file generated by the assembler is composed of several sections written **in the following order**:  

1. Variables (`.data`)
2. Files (`.file`)
3. Sub-Textures (`.sub_textures`)
4. Instructions (`.text`)

---

##### Variables

Variables allow the developer to abstract memory addresses. Each variable has a default value consisting of one or more words, automatically terminated by an empty word (`'\0'`).

**Encoding :**

```
uint32 -> number of variables (X)
X times -> struct variable
```

`struct variable` :

```
uint8 -> variable_flag
uint16 -> number of word in the variables (M)
M fois -> <variable_size> (inititial valur of each word)
```

`variable_flag` :

```cpp
enum VariableFlag {
    DATA_UINT8 = 0b01000000, // DB
    DATA_UINT16 = 0b10000000, // DW
    DATA_UINT32 = 0b11000000 // DD
};
```

---

##### Files

Files are binary data stored directly in the bytecode.  
Only the **descriptor** (numeric identifier), the **raw content** of the file and the file extension are encoded - the name or path is not retained.  

**Encoding :**

```
uint32 -> Number of files (X)
X fois -> struct file
```

`struct file` :

```
uint16 -> descriptor
uint8 -> NUmber of char in the extension (E)
E fois -> uint8 (extension of the file)
uint32 -> Number of word in the file (M)
M fois -> uint8 (bytecode of the file)
```

---

##### Sub_textures

Subtextures allow **spritesheets** to be natively encoded in binary.    
They define sections of an image file considered as an independent texture, referenceable via a descriptor - without storing additional image in the bytecode.

**Encoding :**

```
uint32 -> Number of sub_textures (X)
X fois -> struct subtexture
```

`struct subtexture` :

```
uint16 -> Descriptor of the origine file
uint16 -> Descriptor of the sub_texture
uint16 -> X (coordinate X in origin image)
uint16 -> Y (coordinate Y in origin image)
uint16 -> W (width in origin image)
uint16 -> H (height in origin image)
```

---

##### Instructions

The instructions are written one after the other, in the order of the `.text` section.    
Each instruction is **aligned to 32 bits** according to the following encoding :  

| Field              | Size   | Position   |
|--------------------|--------|------------|
| Mnemonic           | 8 bits | bits 1–8   |
| Handler Number     | 2 bits | bits 9–10  |
| RegX(1) is address | 1 bit  | bit 11     |
| RegX(1)            | 5 bits | bits 12–16 |
| RegX(2) is address | 1 bit  | bit 17     |
| RegX(2)            | 5 bits | bits 18–22 |
| RegX(3) is address | 1 bit  | bit 23     |
| RegX(3)            | 5 bits | bits 24–28 |
| number_of_registry | 2 bits | bits 29–30 |
| data_type          | 2 bits | bits 31–32 |

**`data_type`** indicates whether a second 32-bit block follows the instruction to encode immediate data :  

| value | Signification                                    |
|-------|--------------------------------------------------|
| `00`  | `NO_DATA` - no more data                         |
| `01`  | `Data is value` - The data is an immediate value |
| `10`  | `Data is address` - the data is a memory address |

If `data_type != NO_DATA`, a **second bloc of 32 bits** immediately follows to store the data :  

```
| data      |
| 32 bits   |
| bits 1–32 |
```

---

### List of StegASM operands

#### Operand types

| Type       | Syntax                         | Description                                                          |
|------------|--------------------------------|----------------------------------------------------------------------|
| `reg`      | `R0`                           | Direct value from the register - cannot be dereferenced              |
| `reg_both` | `R0` or `[R0]`                 | Register value **or** value at the address contained in the register |
| `imm`      | `42` or `[42]` or `[ma_var]`   | Immediate value or immediate address                                 |
| `val`      | `R0`, `[R0]`, `42`, `[ma_var]` | Shortcut for `reg_both` **or** `imm`                                 |

> `(val)` indicates an **optional** operand. When it is absent, the instruction uses op1 as both destination and source.

---

#### Load / Store

| Mnemonic    | Operand 1  | Operand 2   | Operand 3 | Description                                                   |
|-------------|------------|-------------|-----------|---------------------------------------------------------------|
| `LOAD_32`   | `reg`      | `val`       | -         | `op1 = op2` (32 bits)                                         |
| `LOAD_16`   | `reg`      | `val`       | -         | `op1 = op2` (16 bits)                                         |
| `LOAD_8`    | `reg`      | `val`       | -         | `op1 = op2` (8 bits)                                          |
| `STORE_32`  | `reg_both` | `reg / imm` | -         | `[op1] = op2` (32 bits)                                       |
| `STORE_16`  | `reg_both` | `reg / imm` | -         | `[op1] = op2` (16 bits)                                       |
| `STORE_8`   | `reg_both` | `reg / imm` | -         | `[op1] = op2` (8 bits)                                        |
| `EXTEND_8`  | `reg`      | `reg_both`  | -         | `op1 = sign_extend_8(op2)` - Extend sign from 8 -> 32 bits    |
| `EXTEND_16` | `reg`      | `reg_both`  | -         | `op1 = sign_extend_16(op2)` - Extend sign from 16 -> 32 bits  |
| `TRUNC_8`   | `reg`      | `reg_both`  | -         | `op1 = op2 & 0xFF` - trunc to 8 bits                          |
| `TRUNC_16`  | `reg`      | `reg_both`  | -         | `op1 = op2 & 0xFFFF` - trunc to 16 bits                       |
| `ALOC`      | `reg`      | `val`       | -         | Allocate `op2` bytes in memory, address of the block in `op1` |
| `FREE`      | `reg`      | -           | -         | Free the block at address `op1`                               |
| `PUSH`      | `reg`      | -           | -         | Push `op1` on the stack                                       |
| `POP`       | `reg`      | -           | -         | Pop the top af the stack in `op1`                             |

---

#### Integer Arithmetic

| Mnemonic       | Operand 1 | Operand 2  | Operand 3 | Description                                                             |
|----------------|-----------|------------|-----------|-------------------------------------------------------------------------|
| `ADD`          | `reg`     | `val`      | `(val)`   | `op1 = op1 + op2` or `op1 = op2 + op3`                                  |
| `SUB`          | `reg`     | `val`      | `(val)`   | `op1 = op1 - op2` or `op1 = op2 - op3`                                  |
| `MUL`          | `reg`     | `val`      | `(val)`   | `op1 = op1 * op2` or `op1 = op2 * op3` (unsigned)                       |
| `SMUL`         | `reg`     | `val`      | `(val)`   | `op1 = op1 * op2` or `op1 = op2 * op3` (signed)                         |
| `DIV`          | `reg`     | `val`      | `(val)`   | `op1 = op1 / op2` or `op1 = op2 / op3` (unsigned)                       |
| `SDIV`         | `reg`     | `val`      | `(val)`   | `op1 = op1 / op2` or `op1 = op2 / op3` (signed)                         |
| `MOD`          | `reg`     | `val`      | `(val)`   | `op1 = op1 % op2` or `op1 = op2 % op3`                                  |
| `MIN`          | `reg`     | `val`      | `(val)`   | `op1 = min(op1, op2)` or `op1 = min(op2, op3)`                          |
| `MAX`          | `reg`     | `val`      | `(val)`   | `op1 = max(op1, op2)` or `op1 = max(op2, op3)`                          |
| `NOT`          | `reg`     | `reg_both` | -         | `op1 = !op2` (Logical NOT)                                              |
| `AND`          | `reg`     | `val`      | `(val)`   | `op1 = op1 & op2` or `op1 = op2 & op3` (AND bit by bit)                 |
| `OR`           | `reg`     | `val`      | `(val)`   | `op1 = op1 \| op2` or `op1 = op2 \| op3` (OR bit by bit)                |
| `XOR`          | `reg`     | `val`      | `(val)`   | `op1 = op1 ^ op2` or `op1 = op2 ^ op3` (XOR bit by bit)                 |
| `BIT_NOT`      | `reg`     | `val`      | -         | `op1 = ~op2` (NOT bit by bit)                                           |
| `SHIFT_LEFT`   | `reg`     | `val`      | `(val)`   | `op1 = op1 << op2` or `op1 = op2 << op3`                                |
| `SHIFT_RIGHT`  | `reg`     | `val`      | `(val)`   | `op1 = op1 >> op2` or `op1 = op2 >> op3` (unsigned)                     |
| `SSHIFT_RIGHT` | `reg`     | `val`      | `(val)`   | `op1 = op1 >> op2` or `op1 = op2 >> op3` (signed, sign bit propagation) |
| `RAND`         | `reg`     | -          | -         | `op1 = random 32 bit number`                                            |

---

#### Floating-point arithmetic

| Mnemonic   | Operand 1 | Operand 2  | Operand 3 | Description                                    |
|------------|-----------|------------|-----------|------------------------------------------------|
| `FADD`     | `reg`     | `val`      | `(val)`   | `op1 = op1 + op2` or `op1 = op2 + op3`         |
| `FSUB`     | `reg`     | `val`      | `(val)`   | `op1 = op1 - op2` or `op1 = op2 - op3`         |
| `FMUL`     | `reg`     | `val`      | `(val)`   | `op1 = op1 * op2` or `op1 = op2 * op3`         |
| `FDIV`     | `reg`     | `val`      | `(val)`   | `op1 = op1 / op2` or `op1 = op2 / op3`         |
| `FMOD`     | `reg`     | `val`      | `(val)`   | `op1 = op1 % op2` or `op1 = op2 % op3`         |
| `FMIN`     | `reg`     | `val`      | `(val)`   | `op1 = min(op1, op2)` or `op1 = min(op2, op3)` |
| `FMAX`     | `reg`     | `val`      | `(val)`   | `op1 = max(op1, op2)` or `op1 = max(op2, op3)` |
| `FNOT`     | `reg`     | `reg_both` | -         | `op1 = !op2` (NOT floating logic)              |
| `FSIN`     | `reg`     | `val`      | -         | `op1 = sin(op2)` (radians)                     |
| `FCOS`     | `reg`     | `val`      | -         | `op1 = cos(op2)` (radians)                     |
| `FTAN`     | `reg`     | `val`      | -         | `op1 = tan(op2)` (radians)                     |
| `FATAN`    | `reg`     | `val`      | -         | `op1 = atan(op2)` (radians)                    |
| `FSQRT`    | `reg`     | `val`      | -         | `op1 = sqrt(op2)`                              |
| `FABS`     | `reg`     | `val`      | -         | `op1 = abs(op2)`                               |
| `FDEG2RAD` | `reg`     | `val`      | -         | `op1 = op2 * π / 180`                          |
| `FRAD2DEG` | `reg`     | `val`      | -         | `op1 = op2 * 180 / π`                          |

##### Integer <-> Floating Point Conversion

| Mnemonic | Operand 1 | Operand 2  | Operand 3 | Description                                |
|----------|-----------|------------|-----------|--------------------------------------------|
| `ITOF`   | `reg`     | `reg_both` | -         | `op1 = (float) op2` (signed -> floating)   |
| `UTOF`   | `reg`     | `reg_both` | -         | `op1 = (float) op2` (unsigned -> floating) |
| `FTOI`   | `reg`     | `reg_both` | -         | `op1 = (int) op2` (floating -> signed)     |
| `FTOU`   | `reg`     | `reg_both` | -         | `op1 = (uint) op2` (floating -> unsigned)  |

---

#### Comparison and conditional jumps

> `CMP`/`FCMP` updates the VM's internal flags. Jump instructions are based on the last `CMP`/`FCMP` executed.

| Mnemonic | Operand 1    | Operand 2 | Operand 3 | Description                                                          |
|----------|--------------|-----------|-----------|----------------------------------------------------------------------|
| `CMP`    | `reg_both`   | `val`     | -         | `op1` and `op2` (integer), update the flags                          |
| `FCMP`   | `reg_both`   | `val`     | -         | Compare `op1` and `op2` (floats), update the flags                   |
| `JMP`    | `imm`        | -         | -         | Unconditional jump to `op1` (label)                                  |
| `JE`     | `imm`        | -         | -         | Jump if `op1 == op2` (last CMP)                                      |
| `JNE`    | `imm`        | -         | -         | Jump if `op1 != op2`                                                 |
| `JA`     | `imm`        | -         | -         | Jump if `op1 > op2` (unsigned)                                       |
| `JSA`    | `imm`        | -         | -         | Jump if `op1 > op2` (signed)                                         |
| `JB`     | `imm`        | -         | -         | Jump if `op1 < op2` (unsigned)                                       |
| `JSB`    | `imm`        | -         | -         | Jump if `op1 < op2` (signed)                                         |
| `CALL`   | `imm`        | -         | -         | Call function to `op1` (label), PUSH the the return address in stack |
| `RET`    | -            | -         | -         | Function return, POP the return address                              |
| `HALT`   | `(reg_both)` | -         | -         | Stop the program, optional exit code `op1`                           |

---

#### Display and debug

> These instructions print in standard output

| Mnemonic      | Operand 1  | Operand 2 | Operand 3 | Description                                                                                                                                                                                                                                          |
|---------------|------------|-----------|-----------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `DISPLAY_N`   | `reg_both` | -         | -         | Display `op1` as unsigned number                                                                                                                                                                                                                     | 
| `DISPLAY_SN`  | `reg_both` | -         | -         | Display `op1` is signed number                                                                                                                                                                                                                       |
| `DISPLAY_F`   | `reg_both` | -         | -         | Display `op1` is float                                                                                                                                                                                                                               |
| `DISPLAY_C`   | `reg_both` | -         | -         | Display the char whose ASCII code is `op1`                                                                                                                                                                                                           |
| `DISPLAY_B`   | `reg_both` | -         | -         | Display `op1` as binary (`0` / `1`)                                                                                                                                                                                                                  |
| `DISPLAY_STR` | `reg_both` | -         | -         | Display the string  null-terminated at address `op1`                                                                                                                                                                                                 |
| `DEBUG_R`     | -          | -         | -         | Display the content of all registry (debug function)                                                                                                                                                                                                 |
| `DEBUG_M`     | -          | -         | -         | Display the content of the memory (debug function)                                                                                                                                                                                                   |
| `DEBUG_T`     | `val`      | -         | -         | First call: It starts the timer for the chosen ID. Subsequent calls: It calculates and displays the elapsed time (the Delta) since the last iteration, then restarts the timer. It can, for example, calculate the time of each iteration of a loop. |

---

#### Window

##### Creation and configuration

| Mnemonic                   | Operand 1  | Operand 2  | Operand 3 | Description                                                          |
|----------------------------|------------|------------|-----------|----------------------------------------------------------------------|
| `WINDOW_CREATE`            | `reg_both` | `reg_both` | `val`     | Create a window of size `op1` x `op2`, title `op3`                   |
| `WINDOW_CLOSE`             | -          | -          | -         | Close the window                                                     |
| `WINDOW_SHOULD_CLOSE`      | `reg`      | -          | -         | `op1 = 1` if closing is asked (cross / ALT+F4), else `0`             |
| `WINDOW_SET_VIEWPORT_SIZE` | `reg_both` | `reg_both` | -         | Sets the viewport size to `op1` x `op2`                              |
| `WINDOW_DISABLE_VIEWPORT`  | `reg_both` | `reg_both` | -         | Remove the viewport                                                  |
| `WINDOW_TOGGLE_FULLSCREEN` | -          | -          | -         | Toggle fullscreen                                                    |
| `WINDOW_SET_TARGET_FPS`    | `val`      | -          | -         | fix the FPS limit to `op1`                                           |
| `WINDOW_GET_DELTA`         | `reg`      | -          | -         | `op1 = delta time` since the last frame (floating-point, in seconds) |
| `WINDOW_SET_ICON`          | `val`      | -          | -         | Defines the window icon (image file descriptor)                      |

##### Drawing

| Mnemonic                          | Operand 1  | Operand 2  | Operand 3  | Description                                                  |
|-----------------------------------|------------|------------|------------|--------------------------------------------------------------|
| `WINDOW_CLEAR`                    | `reg_both` | `reg_both` | `reg_both` | Clear the window with color `(op1, op2, op3)` in RGB         |
| `WINDOW_PRESENT`                  | -          | -          | -          | Display the current frame (swap buffer)                      |
| `WINDOW_DRAW_TEXT`                | `reg_both` | `reg_both` | `val`      | Draw the string `op3` at position `(op1, op2)`               |
| `WINDOW_DRAW_TEXTURE`             | `reg_both` | `reg_both` | `val`      | Draw the texture `op3` at the position `(op1, op2)`          |
| `WINDOW_SET_TEXT_SIZE`            | `val`      | -          | -          | Define text size                                             |
| `WINDOW_SET_TEXT_COLOR`           | `reg_both` | `reg_both` | `reg_both` | Define the text color in RGB `(op1, op2, op3)`               |
| `WINDOW_SET_FONT`                 | `val`      | -          | -          | Define the font (descriptor to font file (.ttf))             |
| `WINDOW_SET_TEXTURE_COLOR_MASK`   | `reg_both` | `reg_both` | `reg_both` | Apply an RGB color mask `(op1, op2, op3)` to drawn textures |
| `WINDOW_RESET_TEXTURE_COLOR_MASK` | -          | -          | -          | reset the texture mask                                       |

##### Framebuffer

| Mnemonic                             | Operand 1  | Operand 2  | Operand 3  | Description                                                     |
|--------------------------------------|------------|------------|------------|-----------------------------------------------------------------|
| `WINDOW_TEXTURE_FRAMEBUFFER_CREATE`  | `reg`      | `reg_both` | `reg_both` | Create a framebuffer of size `op2` x `op3`, descriptor in `op1` |
| `WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS` | `reg`      | `reg_both` | -          | `op1 = adresse` of pixels in framebuffer op2`                   |
| `WINDOW_TEXTURE_FRAMEBUFFER_SYNC`    | `reg_both` | -          | -          | Sync the framebuffer `op1` to the GPU                           |
| `WINDOW_TEXTURE_FRAMEBUFFER_DRAW`    | `reg_both` | `reg_both` | `reg_both` | Draw framebuffer `op3` at position `(op1, op2)`                 |

##### Keyboard inputs 

| Mnemonic             | Operand 1 | Operand 2 | Operand 3 | Description                                          |
|----------------------|-----------|-----------|-----------|------------------------------------------------------|
| `WINDOW_KEY_PRESSED` | `reg`     | `val`     | -         | `op1 = 1` if key `op2` has just been pressed (event) |
| `WINDOW_KEY_DOWN`    | `reg`     | `val`     | -         | `op1 = 1` if key `op2` is currently pressed          |

##### Mouse inputs

| Mnemonic                       | Operand 1 | Operand 2 | Operand 3 | Description                                                 |
|--------------------------------|-----------|-----------|-----------|-------------------------------------------------------------|
| `WINDOW_MOUSE_X`               | `reg`     | -         | -         | `op1 = position X` of mouse                                 |
| `WINDOW_MOUSE_Y`               | `reg`     | -         | -         | `op1 = position Y` of mouse                                 |
| `WINDOW_MOUSE_DELTA_X`         | `reg`     | -         | -         | `op1 = shift X` of mouse from last frame                    |
| `WINDOW_MOUSE_DELTA_Y`         | `reg`     | -         | -         | `op1 = shift Y` of mouse from last frame                    |
| `WINDOW_MOUSE_WHEEL_DELTA`     | `reg`     | -         | -         | `op1 = scroll` of the scroll wheel since the last frame     |
| `WINDOW_MOUSE_BUTTON_PRESSED`  | `reg`     | `val`     | -         | `op1 = 1` if the button `op2` has just been pressed (event) |
| `WINDOW_MOUSE_BUTTON_DOWN`     | `reg`     | `val`     | -         | `op1 = 1` if the button `op2` is held down                  |
| `WINDOW_MOUSE_BUTTON_RELEASED` | `reg`     | `val`     | -         | `op1 = 1` if the button `op2` has just been released        |
| `WINDOW_HIDE_CURSOR`           | -         | -         | -         | Hide cursor of the mouse                                    |
| `WINDOW_SHOW_CURSOR`           | -         | -         | -         | Display the cursor of the mouse                             |
| `WINDOW_DISABLE_CURSOR`        | -         | -         | -         | Disable and lock the cursor                                 |
| `WINDOW_ENABLE_CURSOR`         | -         | -         | -         | Reactivates the cursor                                      |

---

#### Image

| Mnemonic           | Operand 1 | Operand 2 | Operand 3 | Description                                                              |
|--------------------|-----------|-----------|-----------|--------------------------------------------------------------------------|
| `MAP_IMAGE`        | `reg`     | `val`     | -         | Maps the image file `op2` in memory, address in `op1`; Each pixel uint32 |
| `MAP_IMAGE_SIZE_X` | `reg`     | -         | -         | `op1 = width` of the last image mapped                                   |
| `MAP_IMAGE_SIZE_Y` | `reg`     | -         | -         | `op1 = height` of the last image mapped                                  |

---

#### Files

##### Files handling

| Mnemonic               | Operand 1 | Operand 2  | Operand 3  | Description                                                      |
|------------------------|-----------|------------|------------|------------------------------------------------------------------|
| `FILE_OPEN`            | `reg`     | `val`      | -          | Open file at path `op2`, descriptor in`op1`                      |
| `FILE_CREATE`          | `reg`     | `val`      | -          | Create the file at path `op2`, descriptor in `op1`               |
| `FILE_SAVE`            | `reg`     | -          | -          | Save modification in the file `op1` on the disk                  |
| `FILE_DELETE`          | `reg`     | -          | -          | Delete the file `op1`                                            |
| `FILE_CLOSE`           | `reg`     | -          | -          | Close the file `op1`                                             |
| `FILE_GET_SIZE`        | `reg`     | `reg_both` | -          | `op1 = size` byte of file `op2`                                  |
| `FILE_MAP`             | `reg`     | `reg_both` | -          | Maps the entire file `op2` into memory, address in `op1`         |
| `FILE_MAP_FROM_CURSOR` | `reg`     | `reg_both` | `reg_both` | Maps `op3` bytes from the cursor in file `op2`, address in `op1` |
| `FILE_CLEAR_DATA`      | `reg`     | -          | -          | Empty the content of `op1`                                       |

##### Cursor

| Mnemonic            | Operand 1 | Operand 2 | Operand 3 | Description                                                |
|---------------------|-----------|-----------|-----------|------------------------------------------------------------|
| `FILE_RESET_CURSOR` | `reg`     | -         | -         | Move the cursor at the beginning of `op1`                  |
| `FILE_SEEK_CURSOR`  | `reg`     | `reg`     | -         | Move the cursor of `op1` at offset `op2`                   |
| `FILE_GET_CURSOR`   | `reg`     | `reg`     | -         | `op1 = position` current cursor position in the file `op2` |

##### Reading

| Mnemonic                         | Operand 1 | Operand 2  | Operand 3 | Description                                                           |
|----------------------------------|-----------|------------|-----------|-----------------------------------------------------------------------|
| `FILE_READ_BYTE`                 | `reg`     | `reg`      | -         | `op1 = byte` read from the file cursor `op2`                          |
| `FILE_READ_BYTE_AT`              | `reg`     | `reg_both` | `val`     | `op1 = byte` read at offset `op3` of file `op2`                       |
| `FILE_READ_WORD`                 | `reg`     | `reg`      | -         | `op1 = word 16 bits` read from the file cursor (big-endian)           |
| `FILE_READ_WORD_AT`              | `reg`     | `reg_both` | `val`     | `op1 = word 16 bits` at offset `op3` (big-endian)                     |
| `FILE_READ_WORD_LITTLE`          | `reg`     | `reg`      | -         | `op1 = word 16 bits` read from the file cursor (little-endian)        |
| `FILE_READ_WORD_LITTLE_AT`       | `reg`     | `reg_both` | `val`     | `op1 = word 16 bits` at offset `op3` (little-endian)                  |
| `FILE_READ_DOUBLEWORD`           | `reg`     | `reg`      | -         | `op1 = double word 32 bits` read from the file cursor (big-endian)    |
| `FILE_READ_DOUBLEWORD_AT`        | `reg`     | `reg_both` | `val`     | `op1 = double word 32 bits` at offset `op3` (big-endian)              |
| `FILE_READ_DOUBLEWORD_LITTLE`    | `reg`     | `reg`      | -         | `op1 = double word 32 bits` read from the file cursor (little-endian) |
| `FILE_READ_DOUBLEWORD_LITTLE_AT` | `reg`     | `reg_both` | `val`     | `op1 = double word 32 bits` at offset `op3` (little-endian)           |
| `FILE_IS_BYTE_REMAINING`         | `reg`     | `reg`      | -         | `op1 = 1` if there remains at least 1 byte à lire dans le file `op2`  |
| `FILE_IS_WORD_REMAINING`         | `reg`     | `reg`      | -         | `op1 = 1` if there remains at least 2 bytes to read in file `op2`     |
| `FILE_IS_DOUBLEWORD_REMAINING`   | `reg`     | `reg`      | -         | `op1 = 1` if there remains at least 4 bytes to read in file `op2`     |

##### Writing and adding

| Mnemonic                        | Operand 1 | Operand 2 | Operand 3 | Description                                                                    |
|---------------------------------|-----------|-----------|-----------|--------------------------------------------------------------------------------|
| `FILE_WRITE_BYTE`               | `reg`     | `reg`     | -         | Write byte `op2` at the cursor position in `op1`                               |
| `FILE_WRITE_WORD`               | `reg`     | `reg`     | -         | Write the word of 16 bits `op2` at the cursor position (big-endian)            |
| `FILE_WRITE_WORD_LITTLE`        | `reg`     | `reg`     | -         | Write the word of 16 bits `op2` at the cursor position (little-endian)         |
| `FILE_WRITE_DOUBLEWORD`         | `reg`     | `reg`     | -         | Write double-word of 32 bits `op2` at the cursor position (big-endian)         |
| `FILE_WRITE_DOUBLEWORD_LITTLE`  | `reg`     | `reg`     | -         | Write double-word of 32 bits `op2` at the cursor position (little-endian)      |
| `FILE_APPEND_BYTE`              | `reg`     | `reg`     | -         | Append the byte `op2` at the end of the file `op1`                             |
| `FILE_APPEND_WORD`              | `reg`     | `reg`     | -         | Append the word of 16 bits `op2` at the end of the file (big-endian)           |
| `FILE_APPEND_WORD_LITTLE`       | `reg`     | `reg`     | -         | Append the word of 16 bits `op2` at the end of the file (little-endian)        |
| `FILE_APPEND_DOUBLEWORD`        | `reg`     | `reg`     | -         | Append the double-word of 32 bits `op2` at the end of the file (big-endian)    |
| `FILE_APPEND_DOUBLEWORD_LITTLE` | `reg`     | `reg`     | -         | Append the double-word of 32 bits `op2` at the end of the file (little-endian) |

---

### Batched rendering instructions

See [ray tests and indexed spans](raycast-and-raster.md) for the operands, memory layout, and return values of `RAYCAST_SEGMENTS` and `RASTER_INDEXED_SPAN`.

#### Clocks

| Mnemonic               | Operand 1 | Operand 2 | Operand 3 | Description                                                                      |
|------------------------|-----------|-----------|-----------|----------------------------------------------------------------------------------|
| `CLOCK_CREATE`         | `reg`     | -         | -         | Create a clock and start it, descriptor in `op1`                                 |
| `CLOCK_DELETE`         | `reg`     | -         | -         | Delete the clock `op1`                                                           |
| `CLOCK_GET_ELAPSED_MS` | `reg`     | `reg`     | -         | `op1 = time` elapsed in milliseconds from the creation/last reset of clock `op2` |
| `CLOCK_GET_ELAPSED_S`  | `reg`     | `reg`     | -         | `op1 = time` elapsed in secondes of clock `op2`                                  |
| `CLOCK_RESET`          | `reg`     | -         | -         | Reset the clock `op1`                                                            |