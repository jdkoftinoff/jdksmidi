# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

JDKSMIDI is a C++ MIDI library originally written in 1986 and evolved over decades. It provides comprehensive MIDI functionality including parsing, file I/O, sequencing, and show control. The project is released under the GNU General Public License.

## Build System

The project uses CMake as its build system:

### Building with CMake
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j4
```

### CMake Options
- **JDKSMIDI_BUILD_EXAMPLES**: Build example programs (default: ON)
- **JDKSMIDI_BUILD_TESTS**: Build unit tests (default: ON)
- **JDKSMIDI_ENABLE_SANITIZERS**: Enable AddressSanitizer and UBSan (default: OFF)
- **JDKSMIDI_ENABLE_STATIC_ANALYSIS**: Enable clang static analyzer (default: OFF)

### Development Builds
```bash
# Build with sanitizers for debugging
cmake -DJDKSMIDI_ENABLE_SANITIZERS=ON ..
make -j4

# Build with static analysis
cmake -DJDKSMIDI_ENABLE_STATIC_ANALYSIS=ON ..
make -j4
```

## Architecture

### Core Components

The library is organized into several key areas:

**MIDI Message Handling** (`midi.h`, `msg.h`)
- Core MIDI message definitions and parsing
- Timestamped MIDI message encapsulation

**File I/O** (`file.h`, `fileread.h`, `filewrite.h`)
- Standard MIDI File (SMF) Type 0 and Type 1 support
- Multi-track file reading and writing

**Sequencing** (`sequencer.h`, `track.h`, `multitrack.h`)
- MIDI track objects for playback and editing
- Multi-track containers with iterators
- Sequencer core with GUI event triggering

**Advanced Features**
- **Show Control** (`showcontrol.h`) - MIDI Show Control message handling
- **Matrix** (`matrix.h`) - Note tracking to prevent stuck notes
- **Tempo** (`tempo.h`) - Tempo change calculations
- **SMPTE** (`smpte.h`) - Time code management
- **Drivers** (`driver.h`, `driverwin32.h`) - Platform-specific I/O

### Directory Structure

```
include/jdksmidi/    # All header files
src/                 # Implementation files (jdksmidi_*.cpp)
examples/            # Example programs
tests/               # Unit tests using doctest
third-party/         # External dependencies (doctest)
docs-dev/            # Doxygen documentation configuration
```

### Namespace

All classes and functions are in the `jdksmidi` namespace.

## Development Workflow

### Testing
The project uses doctest for unit testing:
```bash
# Run all tests
ctest

# Run tests with verbose output
ctest --verbose

# Build and run tests manually
make jdksmidi_tests
./jdksmidi_tests
```

Unit tests are located in the `tests/` directory:
- `test_basic.cpp` - Basic MIDI message functionality
- `test_parser.cpp` - Comprehensive MIDIParser tests
- `test_matrix.cpp` - MIDIMatrix note tracking tests

Example programs in the `examples/` directory provide additional functionality demonstrations:
- `jdksmidi_test_*.cpp` - Various functionality tests
- `jdksmidi_rewrite_midifile.cpp` - MIDI file processing example

### Code Style
The project follows traditional C++ conventions with:
- Class names prefixed with `MIDI` (e.g., `MIDISequencer`, `MIDITrack`)
- Header guards using `JDKSMIDI_FILENAME_H` format
- Consistent indentation and formatting (use `restyle.sh` for formatting)

## Key Classes

- **MIDIMultiTrack** - Container for multiple MIDI tracks
- **MIDISequencer** - Core sequencing engine with GUI events
- **MIDIFileRead/MIDIFileWrite** - Standard MIDI File I/O
- **MIDITrack** - Individual track management
- **MIDIMatrix** - Note state tracking
- **MIDIDriver** - Abstract driver interface
- **MIDIShowControl** - Show control message handling