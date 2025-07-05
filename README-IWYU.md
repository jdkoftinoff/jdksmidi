# Using include-what-you-use (IWYU) with jdksmidi

## Overview

The jdksmidi project supports include-what-you-use (IWYU) as an optional static analysis tool to help ensure proper header dependencies and reduce compilation times by identifying unnecessary includes.

## Prerequisites

Install include-what-you-use:

**macOS (Homebrew):**
```bash
brew install include-what-you-use
```

**Ubuntu/Debian:**
```bash
sudo apt-get install iwyu
```

**Building from source:**
```bash
git clone https://github.com/include-what-you-use/include-what-you-use.git
cd include-what-you-use
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/llvm-14
make
sudo make install
```

## Usage

Enable IWYU during CMake configuration:

```bash
mkdir build && cd build
cmake .. -DJDKSMIDI_ENABLE_IWYU=ON
make
```

## Configuration

The project includes a custom mapping file (`iwyu.imp`) that helps IWYU understand:
- Standard library header mappings
- Project-specific header relationships
- Platform-specific header requirements

## Interpreting Results

IWYU will output suggestions during compilation, such as:
- Headers that should be added (missing includes)
- Headers that can be removed (unnecessary includes)
- Forward declarations that could replace includes

## Example Output

```
src/jdksmidi_queue.cpp should add these lines:
#include <atomic>  // for atomic

src/jdksmidi_queue.cpp should remove these lines:
- #include "jdksmidi/world.h"  // lines 29-29

The full include-list for src/jdksmidi_queue.cpp:
#include "jdksmidi/queue.h"  // for MIDIQueue, etc.
#include <atomic>  // for atomic
---
```

## Integration with Development Workflow

- **CI/CD**: Add IWYU checks to your continuous integration pipeline
- **Code Review**: Use IWYU suggestions during code reviews
- **Refactoring**: Run IWYU before major refactoring to clean up dependencies

## Customization

Modify `iwyu.imp` to add project-specific mappings:

```json
[
  { include: ["\"myheader.h\"", "private", "\"publicheader.h\"", "public"] }
]
```

## Troubleshooting

- **False positives**: Add mappings to `iwyu.imp` to suppress incorrect suggestions
- **Missing tool**: Ensure include-what-you-use is in your PATH
- **Clang compatibility**: IWYU requires the same Clang version it was built with