#include "doctest/doctest.h"
#include "jdksmidi/parser.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

using namespace jdksmidi;

TEST_CASE("MIDIParser construction and initialization") {
    SUBCASE("Default construction") {
        MIDIParser parser;
        CHECK(parser.GetSystemExclusive() != nullptr);
    }
    
    SUBCASE("Construction with custom sysex size") {
        MIDIParser parser(1024);
        CHECK(parser.GetSystemExclusive() != nullptr);
    }
    
    SUBCASE("Clear method resets state") {
        MIDIParser parser;
        MIDIMessage msg;
        
        // Parse some data to change state
        parser.Parse(0x90, &msg);  // Note On status
        parser.Clear();
        
        // After clear, should be back to initial state
        // Data bytes should be ignored until new status
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Data byte should be ignored
    }
}

TEST_CASE("MIDIParser channel messages parsing") {
    MIDIParser parser;
    MIDIMessage msg;
    
    SUBCASE("Note On message parsing") {
        // Parse Note On (0x91 = Note On, channel 1)
        CHECK_FALSE(parser.Parse(0x91, &msg));  // Status byte, no message yet
        CHECK_FALSE(parser.Parse(0x40, &msg));  // First data byte (note), still need velocity
        CHECK(parser.Parse(0x7F, &msg));        // Second data byte (velocity), message complete
        
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
        CHECK(msg.GetNote() == 0x40);
        CHECK(msg.GetVelocity() == 0x7F);
    }
    
    SUBCASE("Note Off message parsing") {
        CHECK_FALSE(parser.Parse(0x81, &msg));  // Note Off, channel 1
        CHECK_FALSE(parser.Parse(0x45, &msg));  // Note
        CHECK(parser.Parse(0x40, &msg));        // Velocity
        
        CHECK(msg.GetType() == NOTE_OFF);
        CHECK(msg.GetChannel() == 1);
        CHECK(msg.GetNote() == 0x45);
        CHECK(msg.GetVelocity() == 0x40);
    }
    
    SUBCASE("Control Change message parsing") {
        CHECK_FALSE(parser.Parse(0xB2, &msg));  // CC, channel 2
        CHECK_FALSE(parser.Parse(0x07, &msg));  // Controller 7 (volume)
        CHECK(parser.Parse(0x64, &msg));        // Value 100
        
        CHECK(msg.GetType() == CONTROL_CHANGE);
        CHECK(msg.GetChannel() == 2);
        CHECK(msg.GetController() == 0x07);
        CHECK(msg.GetControllerValue() == 0x64);
    }
    
    SUBCASE("Program Change message parsing") {
        CHECK_FALSE(parser.Parse(0xC3, &msg));  // Program Change, channel 3
        CHECK(parser.Parse(0x10, &msg));        // Program 16
        
        CHECK(msg.GetType() == PROGRAM_CHANGE);
        CHECK(msg.GetChannel() == 3);
        CHECK(msg.GetByte1() == 0x10);
    }
    
    SUBCASE("Channel Pressure message parsing") {
        CHECK_FALSE(parser.Parse(0xD4, &msg));  // Channel Pressure, channel 4
        CHECK(parser.Parse(0x50, &msg));        // Pressure value
        
        CHECK(msg.GetType() == CHANNEL_PRESSURE);
        CHECK(msg.GetChannel() == 4);
        CHECK(msg.GetByte1() == 0x50);
    }
    
    SUBCASE("Pitch Bend message parsing") {
        CHECK_FALSE(parser.Parse(0xE5, &msg));  // Pitch Bend, channel 5
        CHECK_FALSE(parser.Parse(0x00, &msg));  // LSB
        CHECK(parser.Parse(0x40, &msg));        // MSB
        
        CHECK(msg.GetType() == PITCH_BEND);
        CHECK(msg.GetChannel() == 5);
        CHECK(msg.GetByte1() == 0x00);
        CHECK(msg.GetByte2() == 0x40);
    }
}

TEST_CASE("MIDIParser running status") {
    MIDIParser parser;
    MIDIMessage msg;
    
    SUBCASE("Note On running status") {
        // First complete message
        CHECK_FALSE(parser.Parse(0x91, &msg));  // Note On status
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Note
        CHECK(parser.Parse(0x7F, &msg));        // Velocity
        
        CHECK(msg.GetNote() == 0x40);
        CHECK(msg.GetVelocity() == 0x7F);
        
        // Second message with running status (no status byte)
        CHECK_FALSE(parser.Parse(0x41, &msg));  // Note (running status)
        CHECK(parser.Parse(0x60, &msg));        // Velocity
        
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
        CHECK(msg.GetNote() == 0x41);
        CHECK(msg.GetVelocity() == 0x60);
    }
    
    SUBCASE("Control Change running status") {
        // First message
        CHECK_FALSE(parser.Parse(0xB0, &msg));  // CC status
        CHECK_FALSE(parser.Parse(0x07, &msg));  // Controller
        CHECK(parser.Parse(0x7F, &msg));        // Value
        
        // Running status message
        CHECK_FALSE(parser.Parse(0x0A, &msg));  // Controller (running status)
        CHECK(parser.Parse(0x40, &msg));        // Value
        
        CHECK(msg.GetType() == CONTROL_CHANGE);
        CHECK(msg.GetController() == 0x0A);
        CHECK(msg.GetControllerValue() == 0x40);
    }
}

TEST_CASE("MIDIParser system messages") {
    MIDIParser parser;
    MIDIMessage msg;
    
    SUBCASE("System real-time messages") {
        // These are single-byte messages that can interrupt other messages
        CHECK(parser.Parse(TIMING_CLOCK, &msg));
        CHECK(msg.GetStatus() == TIMING_CLOCK);
        
        CHECK(parser.Parse(START, &msg));
        CHECK(msg.GetStatus() == START);
        
        CHECK(parser.Parse(CONTINUE, &msg));
        CHECK(msg.GetStatus() == CONTINUE);
        
        CHECK(parser.Parse(STOP, &msg));
        CHECK(msg.GetStatus() == STOP);
        
        CHECK(parser.Parse(ACTIVE_SENSE, &msg));
        CHECK(msg.GetStatus() == ACTIVE_SENSE);
        
        CHECK(parser.Parse(TUNE_REQUEST, &msg));
        CHECK(msg.GetStatus() == TUNE_REQUEST);
    }
    
    SUBCASE("MIDI Time Code (MTC)") {
        CHECK_FALSE(parser.Parse(MTC, &msg));     // MTC status
        CHECK(parser.Parse(0x20, &msg));          // Time code data
        
        CHECK(msg.GetStatus() == MTC);
        CHECK(msg.GetByte1() == 0x20);
        
        // MTC does not support running status
        CHECK_FALSE(parser.Parse(0x21, &msg));    // This should be ignored (no status)
    }
    
    SUBCASE("Song Position Pointer") {
        CHECK_FALSE(parser.Parse(SONG_POSITION, &msg));  // Song Position status
        CHECK_FALSE(parser.Parse(0x00, &msg));           // LSB
        CHECK(parser.Parse(0x02, &msg));                 // MSB
        
        CHECK(msg.GetStatus() == SONG_POSITION);
        CHECK(msg.GetByte1() == 0x00);
        CHECK(msg.GetByte2() == 0x02);
    }
    
    SUBCASE("Song Select") {
        CHECK_FALSE(parser.Parse(SONG_SELECT, &msg));    // Song Select status
        CHECK(parser.Parse(0x05, &msg));                 // Song number
        
        CHECK(msg.GetStatus() == SONG_SELECT);
        CHECK(msg.GetByte1() == 0x05);
    }
    
    SUBCASE("System Reset") {
        // Start parsing a note message
        CHECK_FALSE(parser.Parse(0x90, &msg));  // Note On
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Note
        
        // System Reset should clear the parser state
        CHECK_FALSE(parser.Parse(RESET, &msg));
        
        // Now data bytes should be ignored until new status
        CHECK_FALSE(parser.Parse(0x7F, &msg));  // This should be ignored
    }
}

TEST_CASE("MIDIParser System Exclusive messages") {
    MIDIParser parser;
    MIDIMessage msg;
    
    SUBCASE("Basic SysEx parsing") {
        CHECK_FALSE(parser.Parse(SYSEX_START, &msg));    // Start SysEx
        CHECK_FALSE(parser.Parse(0x41, &msg));           // Manufacturer ID
        CHECK_FALSE(parser.Parse(0x10, &msg));           // Data byte 1
        CHECK_FALSE(parser.Parse(0x16, &msg));           // Data byte 2
        CHECK_FALSE(parser.Parse(0x12, &msg));           // Data byte 3
        CHECK(parser.Parse(SYSEX_END, &msg));            // End SysEx
        
        CHECK(msg.GetStatus() == SYSEX_START);
        
        // Check the SysEx buffer
        MIDISystemExclusive* sysex = parser.GetSystemExclusive();
        CHECK(sysex != nullptr);
        CHECK(sysex->GetLength() == 6);  // F0 + 4 data bytes + F7
    }
    
    SUBCASE("SysEx interrupted by status byte") {
        CHECK_FALSE(parser.Parse(SYSEX_START, &msg));    // Start SysEx
        CHECK_FALSE(parser.Parse(0x41, &msg));           // Data
        
        // Interrupt with a Note On message
        CHECK_FALSE(parser.Parse(0x91, &msg));           // Note On (interrupts SysEx)
        CHECK_FALSE(parser.Parse(0x40, &msg));           // Note
        CHECK(parser.Parse(0x7F, &msg));                 // Velocity
        
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
        
        // SysEx End without start should be ignored
        CHECK_FALSE(parser.Parse(SYSEX_END, &msg));
    }
    
    SUBCASE("SysEx End without Start") {
        // SysEx End without a preceding Start should be ignored
        CHECK_FALSE(parser.Parse(SYSEX_END, &msg));
    }
}

TEST_CASE("MIDIParser data byte handling") {
    MIDIParser parser;
    MIDIMessage msg;
    
    SUBCASE("Data bytes ignored without status") {
        // Data bytes should be ignored when no status is set
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Data byte ignored
        CHECK_FALSE(parser.Parse(0x50, &msg));  // Data byte ignored
        CHECK_FALSE(parser.Parse(0x60, &msg));  // Data byte ignored
        
        // Now send a valid message
        CHECK_FALSE(parser.Parse(0x91, &msg));  // Note On
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Note
        CHECK(parser.Parse(0x7F, &msg));        // Velocity
        
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
    }
    
    SUBCASE("Status bytes with high bit clear are ignored") {
        // Bytes with bit 7 clear (< 0x80) are treated as data bytes
        CHECK_FALSE(parser.Parse(0x70, &msg));  // Ignored (not a status byte)
        
        // Send valid status
        CHECK_FALSE(parser.Parse(0x91, &msg));  // Valid status
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Data
        CHECK(parser.Parse(0x7F, &msg));        // Data
        
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
    }
}

TEST_CASE("MIDIParser error handling") {
    MIDIParser parser;
    MIDIMessage msg;
    
    SUBCASE("Unknown system messages") {
        // Unknown system messages (0xF4, 0xF5, 0xFD) should be ignored
        // Note: 0xF9 is MEASURE_END, not undefined
        CHECK_FALSE(parser.Parse(0xF4, &msg));  // Undefined system message
        CHECK_FALSE(parser.Parse(0xF5, &msg));  // Undefined system message
        CHECK(parser.Parse(MEASURE_END, &msg));  // This is a valid system message
        CHECK(msg.GetStatus() == MEASURE_END);
        CHECK_FALSE(parser.Parse(0xFD, &msg));  // Undefined system message
        
        // Parser should recover and handle valid messages
        CHECK_FALSE(parser.Parse(0x91, &msg));  // Note On
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Note
        CHECK(parser.Parse(0x7F, &msg));        // Velocity
        
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
    }
    
    SUBCASE("System real-time interrupting messages") {
        // Start a Note On message
        CHECK_FALSE(parser.Parse(0x91, &msg));  // Note On
        CHECK_FALSE(parser.Parse(0x40, &msg));  // Note
        
        // System real-time message interrupts
        CHECK(parser.Parse(TIMING_CLOCK, &msg));
        CHECK(msg.GetStatus() == TIMING_CLOCK);
        
        // Complete the original Note On message
        CHECK(parser.Parse(0x7F, &msg));        // Velocity
        CHECK(msg.GetType() == NOTE_ON);
        CHECK(msg.GetChannel() == 1);
        CHECK(msg.GetNote() == 0x40);
        CHECK(msg.GetVelocity() == 0x7F);
    }
}
