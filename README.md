# Freestanding, Portable C Math Library

A self-contained, strict IEEE-754 implementation of the standard C math library (fdlibm-based). 

This library is designed for **freestanding environments** (Operating System kernels, embedded firmware, bootloaders) where the standard C library (`libc` / `libm`) is unavailable.

## Features & Caveats

* **Freestanding:** Zero dependencies. Does not require `<math.h>` or standard headers.
* **Strict IEEE-754:** Implements proper handling for `NaN`, `Inf`, and signed zero.
* **Portable C99:** Written in pure C without assembly. 
* **Endianness:** Uses `uint64_t` unions for bit-manipulation, making it Endian-Independent on all standard IEEE-754 architectures.
* **Performance:**
    * This is a **software implementation**. It is slightly slower (2x - 5x) than hardware-accelerated system libraries.
    * Use this for accuracy and portability, not for high-performance computing (HPC).

## Building & Testing

This project uses **CMake** to ensure correct compilation across Windows (MSVC), Linux (GCC/Clang), and macOS.

### Windows (PowerShell)

```powershell
mkdir build
cd build

# Configure and Build Release (Optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

# Install the executable to the root folder
cmake --install . --config Release --prefix "dist"
Move-Item -Path "dist/bin/run_tests.exe" -Destination "./run_tests.exe" -Force
Remove-Item -Recurse -Force "dist"

# Run the tests
.\run_tests.exe

# (Optional) Cleanup:
cd ..
rm -r build
```

### Linux / macOS (Bash)

```bash
mkdir -p build && cd build

# Configure and Build (Optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Run the tests directly
./run_tests

# (Optional) Cleanup:
cd ..
rm -rf build
```

## Usage

To use this in your own kernel or project, simply add `math.c` and `math.h` to your code.

```c
// In your code
#include "math.h"

// code here...

double result = sin(1.57);
```