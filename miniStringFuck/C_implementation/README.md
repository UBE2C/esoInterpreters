# miniStringFuck Interpreter

A lightweight C implementation of the [miniStringFuck](https://www.codewars.com/kata/586dd26a69b6fd46dd0000c0) interpreter, based on the CodeWars kata of the same name.

## What is miniStringFuck?

miniStringFuck is a minimal esoteric language with only two commands operating on a single memory cell (initially `0`):

| Command | Description |
|---------|-------------|
| `+`     | Increment the memory cell by 1 (wraps from 255 back to 0) |
| `.`     | Append the character at the current memory cell value to the output |

All other characters are ignored.

## Requirements

- CMake >= 3.15
- A C11-compliant compiler (GCC, Clang, or MSVC)

## Building

```bash
git clone https://github.com/UBE2C/Interpreters.git
cd Interpreters/miniStringFuck/C_implementation
mkdir build && cd build
cmake ..
cmake --build .
```

The compiled binary `msfInterpreter` (or `msfInterpreter.exe` on Windows) will be placed in the `build/` directory.

## Usage

Pass the miniStringFuck code as a single command-line argument:

```bash
./msfInterpreter "<code>"
```

The interpreted output is printed to stdout.

### Example

To output the character `A` (ASCII 65):

```bash
./msfInterpreter "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
```

Output:
```
A
```

## Project Structure

```
Interpreters/
├── miniStringFuck/
    └── C_implementation
        ├── src/
        │   └── msfInterpreter.c   # Interpreter source
        ├── CMakeLists.txt          # Build configuration
        └── README.md
```
