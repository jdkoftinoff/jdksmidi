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
- Platform-specific header requirements

### Project Modernization

The jdksmidi project has been modernized to eliminate the monolithic `world.h` header that previously included many standard library headers. This modernization provides several benefits:

- **Faster compilation**: Each source file includes only what it needs
- **Better dependency tracking**: IWYU can provide more accurate suggestions
- **Reduced coupling**: Changes to one header are less likely to trigger unnecessary recompilations
- **Explicit dependencies**: It's clear what each file actually depends on

As a result of this modernization, IWYU now provides more targeted and actionable feedback about specific headers rather than suggesting the old monolithic include pattern.

## Interpreting Results

IWYU will output suggestions during compilation, such as:
- Headers that should be added (missing includes)
- Headers that can be removed (unnecessary includes)
- Forward declarations that could replace includes

## Example Output

```
src/jdksmidi_driver.cpp should add these lines:
#include "jdksmidi/matrix.h"   // for MIDIMatrix
#include "jdksmidi/midi.h"     // for C_DAMPER
#include "jdksmidi/msg.h"      // for MIDITimedBigMessage

The full include-list for src/jdksmidi_driver.cpp:
#include <cstdint>             // for uint8_t, uint32_t
#include "jdksmidi/driver.h"   // for MIDIDriver
#include "jdksmidi/matrix.h"   // for MIDIMatrix
#include "jdksmidi/midi.h"     // for C_DAMPER
#include "jdksmidi/msg.h"      // for MIDITimedBigMessage
---
```

## Integration with Development Workflow

- **CI/CD**: Add IWYU checks to your continuous integration pipeline
- **Code Review**: Use IWYU suggestions during code reviews
- **Refactoring**: Run IWYU before major refactoring to clean up dependencies

## Customization

Modify `iwyu.imp` to add project-specific mappings. The current mapping file focuses on standard library mappings and platform-specific headers:

```json
[
  # Standard library mappings
  { include: ["<bits/stdint-intn.h>", "private", "<cstdint>", "public"] },
  
  # Platform-specific mappings  
  { include: ["<mmsystem.h>", "private", "<windows.h>", "public"] }
]
```

**Note**: The previous monolithic `world.h` mapping has been removed as part of the project modernization to encourage explicit, minimal includes.

## Troubleshooting

- **False positives**: Add mappings to `iwyu.imp` to suppress incorrect suggestions
- **Missing tool**: Ensure include-what-you-use is in your PATH
- **Clang compatibility**: IWYU requires the same Clang version it was built with