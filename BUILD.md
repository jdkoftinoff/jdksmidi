# Building jdksmidi

This project uses CMake for building. The traditional configure/make system has been replaced with a modern CMake build system.

## Quick Start

```bash
# Create a build directory
mkdir build
cd build

# Configure the project
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library and examples
make -j4

# Install (optional)
make install
```

## Build Options

- `JDKSMIDI_BUILD_EXAMPLES`: Build example programs (default: ON)
- `JDKSMIDI_BUILD_SHARED_LIBS`: Build shared libraries instead of static (default: OFF)

### Analysis and Debugging Options

- `JDKSMIDI_ENABLE_STATIC_ANALYSIS`: Enable clang static analysis (default: OFF)
- `JDKSMIDI_ENABLE_SANITIZERS`: Enable clang sanitizers (AddressSanitizer + UndefinedBehaviorSanitizer) (default: OFF)
- `JDKSMIDI_SANITIZER_ADDRESS`: Enable AddressSanitizer only (default: OFF)
- `JDKSMIDI_SANITIZER_MEMORY`: Enable MemorySanitizer only (default: OFF)
- `JDKSMIDI_SANITIZER_THREAD`: Enable ThreadSanitizer only (default: OFF)
- `JDKSMIDI_SANITIZER_UNDEFINED`: Enable UndefinedBehaviorSanitizer only (default: OFF)

Example with options:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DJDKSMIDI_BUILD_EXAMPLES=OFF -DJDKSMIDI_BUILD_SHARED_LIBS=ON
```

Example with sanitizers:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DJDKSMIDI_ENABLE_SANITIZERS=ON
```

Example with static analysis:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DJDKSMIDI_ENABLE_STATIC_ANALYSIS=ON
```

**Note**: Sanitizers have some mutual exclusions:
- AddressSanitizer cannot be used with MemorySanitizer or ThreadSanitizer
- MemorySanitizer cannot be used with ThreadSanitizer
- MemorySanitizer requires Clang (not available with GCC)
- Static analysis requires Clang and will automatically enable additional warning flags

## Platform Support

- **Linux**: Standard build, no special requirements
- **macOS**: Standard build, no special requirements  
- **Windows**: Automatically links with winmm library
- **MinGW**: Adds threading support (-mthreads)

## Using jdksmidi in Your Project

After installation, you can use jdksmidi in your CMake project:

```cmake
find_package(jdksmidc REQUIRED)
target_link_libraries(your_target PRIVATE jdksmidc::jdksmidi)
```

## Legacy Build System

The old configure script and makefiles (magic.mk, project.mk) are now obsolete and have been replaced by this CMake system.