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

Example with options:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DJDKSMIDI_BUILD_EXAMPLES=OFF -DJDKSMIDI_BUILD_SHARED_LIBS=ON
```

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