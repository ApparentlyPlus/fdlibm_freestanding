# Freestanding, Portable C Math Library

A self-contained, strict IEEE-754 implementation of the standard C math library, based on [fdlibm](https://www.netlib.org/fdlibm/). 

This library is designed for **freestanding environments** (Operating System kernels, embedded firmware, bootloaders) where the standard C library (`libc` / `libm`) is unavailable.

## Features & Caveats

* **Freestanding:** Zero dependencies. Does not require `<math.h>` or standard headers.
* **Strict IEEE-754:** Implements proper handling for `NaN`, `Inf`, and signed zero.
* **Portable C99:** Written in pure C without assembly. 
* **Endianness:** Uses `uint64_t` unions for bit-manipulation, making it Endian-Independent on all standard IEEE-754 architectures.
* **Performance:**
    * This is a **software implementation**. It is slightly slower (2x - 5x) than hardware-accelerated system libraries.
    * Use this for accuracy and portability, not for high-performance computing (HPC).

## Attribution

The algorithms, polynomial coefficients and most of the code in `math.c` come from fdlibm, written at Sun Microsystems. Its copyright notices are kept at the top of `math.c` and must stay with any copy or modification of it.

What this repository adds is the freestanding, architecture independent adaptation. Doubles are accessed through a `uint64_t` union instead of fdlibm's high/low word macros, and nothing beyond `<stdint.h>` is needed. `trunc` and `round` are not part of fdlibm.

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