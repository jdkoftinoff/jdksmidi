#include "doctest/doctest.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/parser.h"
#include "jdksmidi/sysex.h"

using namespace jdksmidi;

TEST_CASE("MIDIParser construction and initialization")
{
    SUBCASE("Default construction")
    {
        MIDIParser parser;
        CHECK(parser.get_system_exclusive() != nullptr);
    }

    SUBCASE("Construction with custom sysex size")
    {
        MIDIParser parser(1024);
        CHECK(parser.get_system_exclusive() != nullptr);
    }

    SUBCASE("Clear method resets state")
    {
        MIDIParser parser;
        MIDIMessage msg;

        // Parse some data to change state
        parser.parse(0x90, &msg);  // Note On status
        parser.clear();

        // After clear, should be back to initial state
        // Data bytes should be ignored until new status
        CHECK_FALSE(parser.parse(0x40, &msg));  // Data byte should be ignored
    }
}

TEST_CASE("MIDIParser channel messages parsing")
{
    MIDIParser parser;
    MIDIMessage msg;

    SUBCASE("Note On message parsing")
    {
        // Parse Note On (0x91 = Note On, channel 1)
        CHECK_FALSE(parser.parse(0x91, &msg));  // Status byte, no message yet
        CHECK_FALSE(parser.parse(0x40, &msg));  // First data byte (note), still need velocity
        CHECK(parser.parse(0x7F, &msg));        // Second data byte (velocity), message complete

        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);
        CHECK(msg.get_note() == 0x40);
        CHECK(msg.get_velocity() == 0x7F);
    }

    SUBCASE("Note Off message parsing")
    {
        CHECK_FALSE(parser.parse(0x81, &msg));  // Note Off, channel 1
        CHECK_FALSE(parser.parse(0x45, &msg));  // Note
        CHECK(parser.parse(0x40, &msg));        // Velocity

        CHECK(msg.get_type() == NOTE_OFF);
        CHECK(msg.get_channel() == 1);
        CHECK(msg.get_note() == 0x45);
        CHECK(msg.get_velocity() == 0x40);
    }

    SUBCASE("Control Change message parsing")
    {
        CHECK_FALSE(parser.parse(0xB2, &msg));  // CC, channel 2
        CHECK_FALSE(parser.parse(0x07, &msg));  // Controller 7 (volume)
        CHECK(parser.parse(0x64, &msg));        // Value 100

        CHECK(msg.get_type() == CONTROL_CHANGE);
        CHECK(msg.get_channel() == 2);
        CHECK(msg.get_controller() == 0x07);
        CHECK(msg.get_controller_value() == 0x64);
    }

    SUBCASE("Program Change message parsing")
    {
        CHECK_FALSE(parser.parse(0xC3, &msg));  // Program Change, channel 3
        CHECK(parser.parse(0x10, &msg));        // Program 16

        CHECK(msg.get_type() == PROGRAM_CHANGE);
        CHECK(msg.get_channel() == 3);
        CHECK(msg.get_byte1() == 0x10);
    }

    SUBCASE("Channel Pressure message parsing")
    {
        CHECK_FALSE(parser.parse(0xD4, &msg));  // Channel Pressure, channel 4
        CHECK(parser.parse(0x50, &msg));        // Pressure value

        CHECK(msg.get_type() == CHANNEL_PRESSURE);
        CHECK(msg.get_channel() == 4);
        CHECK(msg.get_byte1() == 0x50);
    }

    SUBCASE("Pitch Bend message parsing")
    {
        CHECK_FALSE(parser.parse(0xE5, &msg));  // Pitch Bend, channel 5
        CHECK_FALSE(parser.parse(0x00, &msg));  // LSB
        CHECK(parser.parse(0x40, &msg));        // MSB

        CHECK(msg.get_type() == PITCH_BEND);
        CHECK(msg.get_channel() == 5);
        CHECK(msg.get_byte1() == 0x00);
        CHECK(msg.get_byte2() == 0x40);
    }
}

TEST_CASE("MIDIParser running status")
{
    MIDIParser parser;
    MIDIMessage msg;

    SUBCASE("Note On running status")
    {
        // First complete message
        CHECK_FALSE(parser.parse(0x91, &msg));  // Note On status
        CHECK_FALSE(parser.parse(0x40, &msg));  // Note
        CHECK(parser.parse(0x7F, &msg));        // Velocity

        CHECK(msg.get_note() == 0x40);
        CHECK(msg.get_velocity() == 0x7F);

        // Second message with running status (no status byte)
        CHECK_FALSE(parser.parse(0x41, &msg));  // Note (running status)
        CHECK(parser.parse(0x60, &msg));        // Velocity

        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);
        CHECK(msg.get_note() == 0x41);
        CHECK(msg.get_velocity() == 0x60);
    }

    SUBCASE("Control Change running status")
    {
        // First message
        CHECK_FALSE(parser.parse(0xB0, &msg));  // CC status
        CHECK_FALSE(parser.parse(0x07, &msg));  // Controller
        CHECK(parser.parse(0x7F, &msg));        // Value

        // Running status message
        CHECK_FALSE(parser.parse(0x0A, &msg));  // Controller (running status)
        CHECK(parser.parse(0x40, &msg));        // Value

        CHECK(msg.get_type() == CONTROL_CHANGE);
        CHECK(msg.get_controller() == 0x0A);
        CHECK(msg.get_controller_value() == 0x40);
    }
}

TEST_CASE("MIDIParser system messages")
{
    MIDIParser parser;
    MIDIMessage msg;

    SUBCASE("System real-time messages")
    {
        // These are single-byte messages that can interrupt other messages
        CHECK(parser.parse(TIMING_CLOCK, &msg));
        CHECK(msg.get_status() == TIMING_CLOCK);

        CHECK(parser.parse(START, &msg));
        CHECK(msg.get_status() == START);

        CHECK(parser.parse(CONTINUE, &msg));
        CHECK(msg.get_status() == CONTINUE);

        CHECK(parser.parse(STOP, &msg));
        CHECK(msg.get_status() == STOP);

        CHECK(parser.parse(ACTIVE_SENSE, &msg));
        CHECK(msg.get_status() == ACTIVE_SENSE);

        CHECK(parser.parse(TUNE_REQUEST, &msg));
        CHECK(msg.get_status() == TUNE_REQUEST);
    }

    SUBCASE("MIDI Time Code (MTC)")
    {
        CHECK_FALSE(parser.parse(MTC, &msg));  // MTC status
        CHECK(parser.parse(0x20, &msg));       // Time code data

        CHECK(msg.get_status() == MTC);
        CHECK(msg.get_byte1() == 0x20);

        // MTC does not support running status
        CHECK_FALSE(parser.parse(0x21, &msg));  // This should be ignored (no status)
    }

    SUBCASE("Song Position Pointer")
    {
        CHECK_FALSE(parser.parse(SONG_POSITION, &msg));  // Song Position status
        CHECK_FALSE(parser.parse(0x00, &msg));           // LSB
        CHECK(parser.parse(0x02, &msg));                 // MSB

        CHECK(msg.get_status() == SONG_POSITION);
        CHECK(msg.get_byte1() == 0x00);
        CHECK(msg.get_byte2() == 0x02);
    }

    SUBCASE("Song Select")
    {
        CHECK_FALSE(parser.parse(SONG_SELECT, &msg));  // Song Select status
        CHECK(parser.parse(0x05, &msg));               // Song number

        CHECK(msg.get_status() == SONG_SELECT);
        CHECK(msg.get_byte1() == 0x05);
    }

    SUBCASE("System Reset")
    {
        // Start parsing a note message
        CHECK_FALSE(parser.parse(0x90, &msg));  // Note On
        CHECK_FALSE(parser.parse(0x40, &msg));  // Note

        // System Reset should clear the parser state
        CHECK_FALSE(parser.parse(RESET, &msg));

        // Now data bytes should be ignored until new status
        CHECK_FALSE(parser.parse(0x7F, &msg));  // This should be ignored
    }
}

TEST_CASE("MIDIParser System Exclusive messages")
{
    MIDIParser parser;
    MIDIMessage msg;

    SUBCASE("Basic SysEx parsing")
    {
        CHECK_FALSE(parser.parse(SYSEX_START, &msg));  // Start SysEx
        CHECK_FALSE(parser.parse(0x41, &msg));         // Manufacturer ID
        CHECK_FALSE(parser.parse(0x10, &msg));         // Data byte 1
        CHECK_FALSE(parser.parse(0x16, &msg));         // Data byte 2
        CHECK_FALSE(parser.parse(0x12, &msg));         // Data byte 3
        CHECK(parser.parse(SYSEX_END, &msg));          // End SysEx

        CHECK(msg.get_status() == SYSEX_START);

        // Check the SysEx buffer
        MIDISystemExclusive* sysex = parser.get_system_exclusive();
        CHECK(sysex != nullptr);
        CHECK(sysex->get_length() == 6);  // F0 + 4 data bytes + F7
    }

    SUBCASE("SysEx interrupted by status byte")
    {
        CHECK_FALSE(parser.parse(SYSEX_START, &msg));  // Start SysEx
        CHECK_FALSE(parser.parse(0x41, &msg));         // Data

        // Interrupt with a Note On message
        CHECK_FALSE(parser.parse(0x91, &msg));  // Note On (interrupts SysEx)
        CHECK_FALSE(parser.parse(0x40, &msg));  // Note
        CHECK(parser.parse(0x7F, &msg));        // Velocity

        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);

        // SysEx End without start should be ignored
        CHECK_FALSE(parser.parse(SYSEX_END, &msg));
    }

    SUBCASE("SysEx End without Start")
    {
        // SysEx End without a preceding Start should be ignored
        CHECK_FALSE(parser.parse(SYSEX_END, &msg));
    }
}

TEST_CASE("MIDIParser data byte handling")
{
    MIDIParser parser;
    MIDIMessage msg;

    SUBCASE("Data bytes ignored without status")
    {
        // Data bytes should be ignored when no status is set
        CHECK_FALSE(parser.parse(0x40, &msg));  // Data byte ignored
        CHECK_FALSE(parser.parse(0x50, &msg));  // Data byte ignored
        CHECK_FALSE(parser.parse(0x60, &msg));  // Data byte ignored

        // Now send a valid message
        CHECK_FALSE(parser.parse(0x91, &msg));  // Note On
        CHECK_FALSE(parser.parse(0x40, &msg));  // Note
        CHECK(parser.parse(0x7F, &msg));        // Velocity

        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);
    }

    SUBCASE("Status bytes with high bit clear are ignored")
    {
        // Bytes with bit 7 clear (< 0x80) are treated as data bytes
        CHECK_FALSE(parser.parse(0x70, &msg));  // Ignored (not a status byte)

        // Send valid status
        CHECK_FALSE(parser.parse(0x91, &msg));  // Valid status
        CHECK_FALSE(parser.parse(0x40, &msg));  // Data
        CHECK(parser.parse(0x7F, &msg));        // Data

        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);
    }
}

TEST_CASE("MIDIParser error handling")
{
    MIDIParser parser;
    MIDIMessage msg;

    SUBCASE("Unknown system messages")
    {
        // Unknown system messages (0xF4, 0xF5, 0xFD) should be ignored
        // Note: 0xF9 is MEASURE_END, not undefined
        CHECK_FALSE(parser.parse(0xF4, &msg));   // Undefined system message
        CHECK_FALSE(parser.parse(0xF5, &msg));   // Undefined system message
        CHECK(parser.parse(MEASURE_END, &msg));  // This is a valid system message
        CHECK(msg.get_status() == MEASURE_END);
        CHECK_FALSE(parser.parse(0xFD, &msg));  // Undefined system message

        // Parser should recover and handle valid messages
        CHECK_FALSE(parser.parse(0x91, &msg));  // Note On
        CHECK_FALSE(parser.parse(0x40, &msg));  // Note
        CHECK(parser.parse(0x7F, &msg));        // Velocity

        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);
    }

    SUBCASE("System real-time interrupting messages")
    {
        // Start a Note On message
        CHECK_FALSE(parser.parse(0x91, &msg));  // Note On
        CHECK_FALSE(parser.parse(0x40, &msg));  // Note

        // System real-time message interrupts
        CHECK(parser.parse(TIMING_CLOCK, &msg));
        CHECK(msg.get_status() == TIMING_CLOCK);

        // Complete the original Note On message
        CHECK(parser.parse(0x7F, &msg));  // Velocity
        CHECK(msg.get_type() == NOTE_ON);
        CHECK(msg.get_channel() == 1);
        CHECK(msg.get_note() == 0x40);
        CHECK(msg.get_velocity() == 0x7F);
    }
}
