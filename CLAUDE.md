# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

JDKSMIDI is a C++ MIDI library originally written in 1986 and evolved over decades. It provides comprehensive MIDI functionality including parsing, file I/O, sequencing, and show control. The project is released under the GNU General Public License.

## Build System

The project uses a custom build system called MagicMake:

### Building with MagicMake
```bash
# Create build directory
mkdir build && cd build

# Configure for your platform
../configure --target-platform-macosx=1    # for macOS
../configure --target-platform-linux=1     # for Linux  
../configure --target-platform-mingw32=1   # for Windows

# Build
make
```

### Platform-Specific Builds
- **macOS**: Use `--target-platform-macosx=1` or `--target-platform-macosx-universal=1`
- **Linux**: Use `--target-platform-linux=1` or `--target-platform-posix=1`
- **Windows**: Use `--target-platform-mingw32=1` with MinGW cross-compiler

### IDE Project Files
The project includes pre-built IDE project files in the `build/` directory:
- **Xcode**: `build/macosx/jdksmidi.xcodeproj/`
- **Visual Studio 2005**: `build/vs2005/`
- **Visual Studio 2010**: `build/vs2010/`
- **Visual C++ 6.0**: `build/vc6/`

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
examples/            # Example programs and tests
build/               # IDE project files for different platforms
docs-dev/            # Doxygen documentation configuration
```

### Namespace

All classes and functions are in the `jdksmidi` namespace.

## Development Workflow

### Testing
Example programs in the `examples/` directory serve as tests:
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