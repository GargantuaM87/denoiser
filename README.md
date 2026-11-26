# Denoiser

A native Linux desktop application to find wasted disk space in software development projects and explain where it exactly went.

Can help get rid of accumulated gigabytes of build artifacts, dependency caches, old binaries, generated files, and duplicate assets.

(Most of this is still under the works...)

## Features

- Visualizing a directory as a nested tree
- Printing out summaries of directory information
- Searching for the largest files, filtering with minimum memory requirements, and filtering for file extensions

## Requirements 
- Linux (Ubuntu, and my other distros should suffice)
- CMake 3.20 or newer
- A C++20 compiler 
- The following dependences:
    - `libedit`
    - `fmt`
    - `GoogleTest` (only if testing)

## Build Instructions
Make sure to have the required libraries installed system-wide before building.

1. Clone the repository
```bash
git clone https://github.com/GargantuaM87/denoiser
cd denoiser
```

2. Build the project
```bash
cmake -S . -B build
cmake --build build
```

3. Optionally run the tests
```bash
cd build && ctest
```
or
```bash
cd build && ./test/tests
```

## Usage Examples
The CLI binary is listed under `build/src` as `denoiser`

### To Start
```bash
./build/src/denoiser ~/whatever-path <ignore-set>
```

The `ignore-set` is a list of space separated arguments which makes the scanner ignore files and directories of those provided names.

The command loop is interactive and commands will continue operating on the current set directory until changed.

### Commands
``` 
tree - Commands for visualizing a directory tree
setdir - Commands for changing the current operating directory 
search - Commands for finding and filtering for specific file information
help - Commands for finding more information about commands 
```
The general help menu after calling `help` within the program loop. More information can be gleaned by calling `help` then the command name.

## Limitations
- So far, operating on giant directories is a very slow task unless the user provides a good `ignore-set`
- Of course operating on directories with insufficient permissions is a no-go. 

## Future Plans
- Making the architecture much more modular
- Creating a more thorough scanning engine
- Adding a Qt interface


