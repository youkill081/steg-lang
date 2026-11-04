# The Steg language

**Steg** is the high-level language of the project. It compiles to StegASM via a full compilation chain :

```
Lexer -> Parseur -> AST -> Semantics analysis -> IR -> StegASM Backend
```

Files use the `.steg` extension.

> For an introduction to StegASM, the low-level language, consult the [StegASM documentation](./stegasm.md).

---

## Table of content

<!-- TOC -->
* [The Steg language](#the-steg-language)
  * [Table of content](#table-of-content)
  * [Structure of a `.steg` file](#structure-of-a-steg-file)
  * [Types](#types)
    * [Primitives Types](#primitives-types)
    * [Opaques Types](#opaques-types-)
    * [Conversions](#conversions)
  * [Variables](#variables)
    * [Locales Variables](#locales-variables)
    * [Globals Variables](#globals-variables)
    * [Strings](#strings)
  * [Functions](#functions)
    * [Declaration](#declaration)
    * [Exportation](#exportation)
  * [Imports and Exports](#imports-and-exports)
    * [Import Symbol](#import-symbol)
    * [Files propagations](#files-propagations)
  * [Control structures](#control-structures)
    * [Conditions](#conditions)
    * [While loop](#while-loop-)
    * [For Loops](#for-loops)
    * [break and continue](#break-and-continue)
  * [Files Section](#files-section-)
  * [The file type in details](#the-file-type-in-details)
  * [Built-in functions](#built-in-functions)
    * [Printing and debug](#printing-and-debug)
    * [System](#system)
    * [Mathematics](#mathematics)
    * [Images](#images)
    * [Window - lifecycle](#window---lifecycle)
    * [Window - Drawing](#window---drawing)
    * [Window - Keyboard](#window---keyboard)
    * [Window - Mouse](#window---mouse)
    * [Window - Framebuffer](#window---framebuffer)
    * [Files and vectors](#files-and-vectors)
    * [Clocks](#clocks)
<!-- TOC -->

---

## Structure of a `.steg` file

A `.steg` file can contain the following elements, in any order :  

| Élément            | Rôle                                         |
|--------------------|----------------------------------------------|
| `import { ... }`   | Import symbols from another file             |
| `files { ... }`    | Included files in the binary                 |
| Globals variables  | Globals variables are accessible in the file |
| `fn` / `export fn` | Function definition                          |

Example of a `.steg` file :

```steg
import { init_gun, update_player_pos } from "./game/player_helper.steg";
 
files {
    GAMEMAPS: "./assets/GAMEMAPS.WL6"
}
 
uint32 num_doors = 0;
 
export fn main() -> void {
    print_str("Hello world !");
}
```

---

## Types

### Primitives Types

| Type     | Taille  | Description            |
|----------|---------|------------------------|
| `uint8`  | 8 bits  | 8 bits unsigned number |
| `uint16` | 16 bits | 16 bits unsigned number |
| `uint32` | 32 bits | 32 bits unsigned number |
| `int`    | 32 bits | Signed number          |
| `float`  | 32 bits | Float                  |
| `bool`   | 8 bits  | Boolean                |

### Opaques Types 

Opaque Types are representing the resources managed by the interpreter. They cannot be created by a literal value - they are obtained and manipulated exclusively via built-in functions.  

| Type          | Description                                          |
|---------------|------------------------------------------------------|
| `file`        | File on the disk or a binary value in memory (array) |
| `framebuffer` | Drawing surface for drawing                          |
| `clock`       | Clock to handle time                                 |

### Conversions

Conversions between primitives' types (`int`, `float`, `uint8`, `uint16`, `uint32`) are all **implicit**. There is no explicit cast syntax.  

```steg
float dist = 5.56;  // uint -> float, implicit
uint32 x = dist;    // float -> uint32, implicit -> x = 5
```

> Implicits' conversions could generate **hints** if they could lose some data (ex: float -> uint). The compiler will inform you about them.  

---

## Variables

### Locales Variables

Locals' Variables are declared inside a function and are initialized with an expression.

```steg
fn exemple() -> void {
    uint32 x = 0;
    float angle = 3.14;
    uint16 door_id = file_read_little_uint16_at(d_map, (y * 64 + x) * 2);
}
```

### Globals Variables

Globals Variables are declared outside a function and are initialized with **constant values**.

```steg
uint32 num_doors = 0;  // OK
file d_map = 0;        // OK, but the descritor may be invalid without initialization
uint32 computed = 5 + 2; // Not allowed : not an constant expression
```

> ⚠️ Globals Variable **can't be exported**. To expose them, create an explicit getter function :  

```steg
uint32 num_doors = 0;
 
export fn get_num_doors() -> uint32 { // getter for num_doors
    return num_doors;
}
```

### Strings

Strings are declared with the `@` notation on a `uint8` type :

```steg
uint8 @my_string = "hello world";
```

> Arrays of number aren't yet supported. Only strings are supported in this notation.

---

## Functions

### Declaration

A function is declared with the `fn` keyword. The return type is **mandatory** :

```steg
fn add(int a, int b) -> int {
    return a + b;
}
 
fn main() -> void {
    int result = add(3, 7);
}
```

`void` indictable that the function doesn't return anything.

### Exportation

By default, a function is **not** exported. To make it accessible from another file, use the `export` keyword :

```steg
export fn get_door_openness(int x, int y) -> float {
    uint16 door_id = file_read_little_uint16_at(d_map, (y * 64 + x) * 2);
 
    if (door_id == 65535) { return 0.0; }
    return file_read_little_uint32_at(d_open, door_id * 4) / 10000.0;
}
```

---

## Imports and Exports

### Import Symbol

To import a symbol from another `.steg` file, use this following syntax :  

```steg
import { a_fonction, another_function } from "./file/to/file.steg";
```

```steg
import { load_level } from "./file_loader.steg";
import { cast_rays, init_renderer, get_zbuffer } from "./renderer.steg";
```

Only the functions marked `export` in the imported file are accessible from the current file (see [Exportation](#exportation)).

### Files propagations

The section `files { ... }` is **automatically exported** with the `.steg` file. If a file A imports B, and B declares resources in `files { ... }`, these resources are available in A.

Global variables, however, do not propagate **not** - they remain private to the file that declares them.

---

## Control structures

### Conditions

`if` / `else` blocks follow the classic syntax. The brackets are **mandatory**:

```steg
if (door_id == 65535) {
    return 0.0;
} else {
    return 1.0;
}
```

> ⚠️ `else if` isn't supported. To chain multiple conditions, nest `if` blocks inside each other :

```steg
if (x == 0) {
    // case 1
} else {
    if (x == 1) {
        // case 2
    } else {
        // case 3
    }
}
```

### While loop 

```steg
while (!window_should_close()) {
    window_clear(0, 0, 0);
    window_present();
}
```

### For Loops

```steg
uint32 i = 0;
for (i = 0; i < count; i += 1) {
    file_append_little_uint16(value, out_file);
}
```

> The loop variable must be **declared before** the `for`.

### break and continue

The keywords `break` and `continue` are available in `while` and `for` loops:

```steg
while (true) {
    if (x > 100) { break; }
    if (x % 2 == 0) { continue; }
    x += 1;
}
```

---

## Files Section 

The `files` section allows you to **embed files directly in the compiled binary**. It is the direct equivalent of the `.file` section in StegASM.

```steg
files {
    GAMEMAPS: "./assets/GAMEMAPS.WL6"
    MAPHEAD:  "./assets/MAPHEAD.WL6"
    VSWAP:    "./assets/VSWAP.WL6"
}
```

Each entry associates an **identifier** (e.g., `GAMEMAPS`) with a file path. These identifiers can then be used as arguments for built-in functions:

```steg
file header = file_open(MAPHEAD);
```

The `files` section is **automatically exported**: any file that imports a `.steg` containing this section also benefits from the declared resources.

---

## The file type in details

The `file` type is the most versatile opaque type in the language. Beyond simply reading files from the disk, it also serves as a **dynamic vector in memory** - the equivalent of a vector or a buffer.

All operations on a `file` are handled directly by the interpreter: no explicit dynamic allocation, no overhead.

**Create an empty vector :**

```steg
file z_buffer = file_create("");
```

**Write in a vector :**

```steg
file_clear_data(z_buffer);   // Empty all data
file_reset_cursor(z_buffer); // Place the cursor at the beginning
 
while (x < 320) {
    float value = dist * cos(deg2rad(angle));
    file_append_uint32(value * 10000.0, z_buffer); // append uint32 at the end of the vector
    x += 1;
}
```

**Read from a vector :**

```steg
file_seek_cursor(0, in_file);
uint16 w = file_read_little_uint16(in_file);
```

**Pass a `file` as a parameter and return it:**

The `file` type can be passed as an argument and returned by a function just like any other type:

```steg
fn rlew_decode(file in_file, uint16 rlew_tag, uint32 in_length) -> file {
    file out_file = file_create("");
    // ...
    return out_file;
}
```

---

## Built-in functions

The built-in functions are directly integrated into the language. They require no import and are compiled directly into StegASM instructions - their overhead is therefore almost **zero**.

### Printing and debug

| Signature             | Return | Description               |
|-----------------------|--------|---------------------------|
| `print_n(uint32 v)`   | `void` | Display unsigned integer  |
| `print_sn(int v)`     | `void` | Display signer integer    |
| `print_f(float v)`    | `void` | Display a float           |
| `print_c(uint8 v)`    | `void` | Display a char            |
| `print_b(uint32 v)`   | `void` | Display a value in binary |
| `print_str(uint8 @s)` | `void` | Display a string          |

### System

| Signature           | Return   | Description                                                                                                                                                                                                                                          |
|---------------------|----------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `exit(uint8 code)`  | `void`   | Terminates execution with an exit code                                                                                                                                                                                                               |
| `aloc(uint32 size)` | `void @` | Alloc a memory block                                                                                                                                                                                                                                 |
| `free(void @ptr)`   | `void`   | Free a memory block allocate with `aloc`                                                                                                                                                                                                             |
| `rand()`            | `uint32` | Return a random number                                                                                                                                                                                                                               |
| `debug_r()`         | `void`   | Display registries status (debug function)                                                                                                                                                                                                           |
| `debug_m()`         | `void`   | Display the memory content (debug function)                                                                                                                                                                                                          |
| `debug_t(uint32 v)` | `void`   | First call: It starts the timer for the chosen ID. Subsequent calls: It calculates and displays the elapsed time (the Delta) since the last iteration, then restarts the timer. It can, for example, calculate the time of each iteration of a loop. |

### Mathematics

| Signature                | Return  | Description                 |
|--------------------------|---------|-----------------------------|
| `sin(float v)`           | `float` | Sinus                       |
| `cos(float v)`           | `float` | Cosinus                     |
| `ftan(float v)`          | `float` | Tangent                     |
| `fatan(float v)`         | `float` | Arc-tangent                 |
| `sqrt(float v)`          | `float` | Square root                 |
| `fabs(float v)`          | `float` | Absolute value              |
| `fmax(float a, float b)` | `float` | Maximum of two floats       |
| `fmin(float a, float b)` | `float` | Minimum of two floats       |
| `deg2rad(float v)`       | `float` | Converts degrees to radians |
| `rad2deg(float v)`       | `float` | Converts radians to degrees |

### Images

| Signature             | Return     | Description                                                   |
|-----------------------|------------|---------------------------------------------------------------|
| `image_map(file f)`   | `uint32 @` | Maps an image to memory, returns a pointer to the RGBA pixels |
| `last_image_size_x()` | `uint32`   | With of the last mapped image                                 |
| `last_image_size_y()` | `uint32`   | Height of the last mapped image                               |

### Window - lifecycle

| Signature                                         | Return  | Description                           |
|---------------------------------------------------|---------|---------------------------------------|
| `window_create(uint32 w, uint32 h, uint8 @title)` | `void`  | Create and open a window              |
| `window_close()`                                  | `void`  | Close the window                      |
| `window_should_close()`                           | `bool`  | True if user ask closing              |
| `window_toggle_fullscreen()`                      | `void`  | Toggle fullscreen                     |
| `window_set_viewport_size(uint32 w, uint32 h)`    | `void`  | Defined the drawing viewport          |
| `window_disable_viewport()`                       | `void`  | Disable the viewport                  |
| `window_set_target_fps(uint32 fps)`               | `void`  | Target FPS                            |
| `window_get_delta()`                              | `float` | Return delta-time from the last frame |
| `window_set_icon(file f)`                         | `void`  | Define the icon of the window         |

### Window - Drawing

| Signature                                                  | Return | Description                             |
|------------------------------------------------------------|--------|-----------------------------------------|
| `window_clear(uint8 r, uint8 g, uint8 b)`                  | `void` | Clear the window with a color           |
| `window_present()`                                         | `void` | Present the current frame               |
| `window_draw_text(uint32 x, uint32 y, uint8 @s)`           | `void` | Draw text at the given coordinates      |
| `window_draw_texture(uint32 x, uint32 y, file f)`          | `void` | Draw a texture at the given coordinates |
| `window_set_text_size(uint32 size)`                        | `void` | Define text size                        |
| `window_set_text_color(uint8 r, uint8 g, uint8 b)`         | `void` | Define the color                        |
| `window_set_font(file f)`                                  | `void` | Load and enable a font                  |
| `window_set_texture_color_mask(uint8 r, uint8 g, uint8 b)` | `void` | Apply a color mask to a font            |
| `window_reset_texture_color_mask()`                        | `void` | Remove the color mask                   |

### Window - Keyboard

> The keys are from the raylib scancodes

| Signature                        | Return | Description                          |
|----------------------------------|--------|--------------------------------------|
| `window_key_pressed(uint32 key)` | `bool` | True if the key just pressed         |
| `window_key_down(uint32 key)`    | `bool` | True if the key is currently pressed |

### Window - Mouse

| Signature                                  | Return   | Description                         |
|--------------------------------------------|----------|-------------------------------------|
| `window_mouse_x()`                         | `uint32` | X Position of cursor                |
| `window_mouse_y()`                         | `uint32` | Y Position of cursor                |
| `window_mouse_delta_x()`                   | `int`    | X delta from the last frame         |
| `window_mouse_delta_y()`                   | `int`    | Y delta from the last frame         |
| `window_mouse_wheel_delta()`               | `int`    | Scrolling wheel from the last frame |
| `window_mouse_button_pressed(uint32 btn)`  | `bool`   | True if the button just clicked     |
| `window_mouse_button_down(uint32 btn)`     | `bool`   | True if the button est is down      |
| `window_mouse_button_released(uint32 btn)` | `bool`   | True if the button just released    |
| `window_hide_cursor()`                     | `void`   | Hide cursor                         |
| `window_show_cursor()`                     | `void`   | Show cursor                         |
| `window_disable_cursor()`                  | `void`   | Capture the cursor (FPS mode)       |
| `window_enable_cursor()`                   | `void`   | Reactivates the cursor              |

### Window - Framebuffer

| Signature                                                     | Return        | Description                                       |
|---------------------------------------------------------------|---------------|---------------------------------------------------|
| `window_framebuffer_create(uint32 w, uint32 h)`               | `framebuffer` | Create a framebuffer of given size                |
| `window_framebuffer_get_address(framebuffer fb)`              | `uint32 @`    | Return a pointer to the pixels of the framebuffer |
| `window_framebuffer_sync(framebuffer fb)`                     | `void`        | Sync modification with the GPU                    |
| `window_framebuffer_draw(framebuffer fb, uint32 x, uint32 y)` | `void`        | Draw the framebuffer at given position            |

### Batched rendering

| Signature | Return | Description |
|-----------|--------|-------------|
| `raycast_segments(uint32 @descriptor, file source)` | `uint32` | Find the nearest intersecting segment |
| `raster_indexed_span(uint32 @descriptor, file source, framebuffer fb)` | `void` | Draw an indexed texture span into a framebuffer |

See [ray tests and indexed spans](raycast-and-raster.md) for the descriptor layouts, flags, and outputs.

### Files and vectors

| Signature                                  | Return   | Description                               |
|--------------------------------------------|----------|-------------------------------------------|
| `file_open(uint8 @path)`                   | `file`   | Opens an existing file on disk            |
| `file_create(uint8 @path)`                 | `file`   | Creates a new empty file or vector (`""`) |
| `file_save(file f)`                        | `void`   | Save the file to disk                     |
| `file_delete(file f)`                      | `void`   | Delete the file on disk                   |
| `file_close(file f)`                       | `void`   | Close and release the descriptor          |
| `file_size(file f)`                        | `uint32` | Returns the size in bytes                 |
| `file_map(file f)`                         | `void @` | Maps the entire file in memory            |
| `file_map_x_from_cursor(file f, uint32 n)` | `void @` | Map `n` bytes from cursor position        |
| `file_reset_cursor(file f)`                | `void`   | Place the cursor at the beginning         |
| `file_seek_cursor(uint32 pos, file f)`     | `void`   | Move cursor to absolute position `pos`    |
| `file_get_cursor(file f)`                  | `uint32` | Return current position of cursor         |
| `file_clear_data(file f)`                  | `void`   | Empty all data                            |
| `file_is_uint8_remaining(file f)`          | `bool`   | True if there almost 1 byte to read       |
| `file_is_uint16_remaining(file f)`         | `bool`   | True if there almost 2 bytes to read      |
| `file_is_uint32_remaining(file f)`         | `bool`   | True if there almost 4 bytes to read      |

**Sequential reading (from the cursor):**

| Signature                         | Return   | Description               |
|-----------------------------------|----------|---------------------------|
| `file_read_uint8(file f)`         | `uint8`  | Read 1 byte               |
| `file_read_uint16(file f)`        | `uint16` | Read 2 bytes big-endian   |
| `file_read_little_uint16(file f)` | `uint16` | Read 2 bytes little-endian |
| `file_read_uint32(file f)`        | `uint32` | Read 4 bytes big-endian   |
| `file_read_little_uint32(file f)` | `uint32` | Read 4 bytes little-endian |

**Absolute reading (at a given position):**

| Signature                                        | Return   | Description                        |
|--------------------------------------------------|----------|------------------------------------|
| `file_read_uint8_at(file f, uint32 pos)`         | `uint8`  | Read 1 byte from `pos`             |
| `file_read_uint16_at(file f, uint32 pos)`        | `uint16` | Read 2 bytes big-endian from `pos`    |
| `file_read_little_uint16_at(file f, uint32 pos)` | `uint16` | Read 2 bytes little-endian from `pos` |
| `file_read_uint32_at(file f, uint32 pos)`        | `uint32` | Read 4 bytes big-endian from `pos`    |
| `file_read_little_uint32_at(file f, uint32 pos)` | `uint32` | Read 4 bytes little-endian from `pos` |

**Sequential writing (append):**

| Signature                                     | Return | Description                  |
|-----------------------------------------------|--------|------------------------------|
| `file_append_uint8(uint8 v, file f)`          | `void` | Append 1 byte                |
| `file_append_uint16(uint16 v, file f)`        | `void` | Append 2 bytes big-endian    |
| `file_append_little_uint16(uint16 v, file f)` | `void` | Append 2 bytes little-endian |
| `file_append_uint32(uint32 v, file f)`        | `void` | Append 4 bytes big-endian    |
| `file_append_little_uint32(uint32 v, file f)` | `void` | Append 4 bytes little-endian |

**Writing at the cursor position:**

| Signature                                    | Return | Description                     |
|----------------------------------------------|--------|---------------------------------|
| `file_write_uint8(uint8 v, file f)`          | `void` | Write 1 byte at cursor position |
| `file_write_uint16(uint16 v, file f)`        | `void` | Write 2 bytes big-endian        |
| `file_write_little_uint16(uint16 v, file f)` | `void` | Write 2 bytes little-endian     |
| `file_write_uint32(uint32 v, file f)`        | `void` | Write 4 bytes big-endian        |
| `file_write_little_uint32(uint32 v, file f)` | `void` | Write 4 bytes little-endian     |

### Clocks

| Signature                       | Return   | Description                  |
|---------------------------------|----------|------------------------------|
| `clock_create()`                | `clock`  | Create a clock and start it  |
| `clock_delete(clock c)`         | `void`   | Free the clock               |
| `clock_reset(clock c)`          | `void`   | Reset the clock to zero      |
| `clock_get_elapsed_ms(clock c)` | `uint32` | Elapsed time in milliseconds |
| `clock_get_elapsed_s(clock c)`  | `uint32` | Elapsed time in seconds      |