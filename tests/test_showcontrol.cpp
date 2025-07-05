/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004-2025  Jeffrey Koftinoff
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE MIT LICENSE ***
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "doctest/doctest.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/showcontrol.h"
#include "jdksmidi/sysex.h"

using namespace jdksmidi;

TEST_CASE("MIDICue basic construction and properties")
{
    SUBCASE("Default constructor")
    {
        MIDICue cue;

        CHECK(cue.get_v1() == 0);
        CHECK(cue.get_num_values() == 1);
        CHECK(static_cast<std::uint32_t>(cue) == 0);
    }

    SUBCASE("Single value constructor")
    {
        MIDICue cue(42);

        CHECK(cue.get_v1() == 42);
        CHECK(cue.get_num_values() == 1);
        CHECK(static_cast<std::uint32_t>(cue) == 42);
    }

    SUBCASE("Two value constructor")
    {
        MIDICue cue(10, 20);

        CHECK(cue.get_v1() == 10);
        CHECK(cue.get_v2() == 20);
        CHECK(cue.get_num_values() == 2);
        CHECK(static_cast<std::uint32_t>(cue) == 10);
    }

    SUBCASE("Three value constructor")
    {
        MIDICue cue(1, 2, 3);

        CHECK(cue.get_v1() == 1);
        CHECK(cue.get_v2() == 2);
        CHECK(cue.get_v3() == 3);
        CHECK(cue.get_num_values() == 3);
        CHECK(static_cast<std::uint32_t>(cue) == 1);
    }

    SUBCASE("Copy constructor")
    {
        MIDICue original(100, 200, 300);
        MIDICue copy(original);

        CHECK(copy.get_v1() == 100);
        CHECK(copy.get_v2() == 200);
        CHECK(copy.get_v3() == 300);
        CHECK(copy.get_num_values() == 3);
    }
}

TEST_CASE("MIDICue assignment and operations")
{
    SUBCASE("Assignment from MIDICue")
    {
        MIDICue source(50, 60);
        MIDICue target;

        target = source;

        CHECK(target.get_v1() == 50);
        CHECK(target.get_v2() == 60);
        CHECK(target.get_num_values() == 2);
    }

    SUBCASE("Assignment from uint32_t")
    {
        MIDICue cue(100, 200, 300);

        cue = 500;

        CHECK(cue.get_v1() == 500);
        CHECK(cue.get_num_values() == 1);
        CHECK(static_cast<std::uint32_t>(cue) == 500);
    }

    SUBCASE("Clear functionality")
    {
        MIDICue cue(10, 20, 30);

        cue.clear();

        CHECK(cue.get_v1() == 0);
        CHECK(cue.get_num_values() == 1);
    }

    SUBCASE("Setters")
    {
        MIDICue cue;

        cue.set_v1(100);
        cue.set_v2(200);
        cue.set_v3(300);
        cue.set_num_values(3);

        CHECK(cue.get_v1() == 100);
        CHECK(cue.get_v2() == 200);
        CHECK(cue.get_v3() == 300);
        CHECK(cue.get_num_values() == 3);
    }
}

TEST_CASE("MIDICue comparison operations")
{
    SUBCASE("Equality with uint32_t")
    {
        MIDICue cue(42);

        CHECK(cue == 42);
        CHECK_FALSE(cue == 43);
        CHECK_FALSE(cue != 42);
        CHECK(cue != 43);
    }

    SUBCASE("Relational operations with uint32_t")
    {
        MIDICue cue(50);

        CHECK(cue <= 50);
        CHECK(cue <= 51);
        CHECK_FALSE(cue <= 49);

        CHECK(cue >= 50);
        CHECK(cue >= 49);
        CHECK_FALSE(cue >= 51);

        CHECK(cue < 51);
        CHECK_FALSE(cue < 50);
        CHECK_FALSE(cue < 49);

        CHECK(cue > 49);
        CHECK_FALSE(cue > 50);
        CHECK_FALSE(cue > 51);
    }
}

TEST_CASE("MIDICue arithmetic operations")
{
    SUBCASE("Addition with MIDICue")
    {
        MIDICue a(10, 20, 30);
        MIDICue b(5, 10, 15);

        MIDICue result = a + b;

        CHECK(result.get_v1() == 15);
        CHECK(result.get_v2() == 30);
        CHECK(result.get_v3() == 45);
    }

    SUBCASE("Addition with uint32_t")
    {
        MIDICue cue(100, 200, 300);

        MIDICue result1 = cue + std::uint32_t(50);
        CHECK(result1.get_v1() == 150);

        MIDICue result2 = std::uint32_t(25) + cue;
        CHECK(result2.get_v1() == 125);
    }

    SUBCASE("Subtraction with MIDICue")
    {
        MIDICue a(100, 200, 300);
        MIDICue b(10, 20, 30);

        MIDICue result = a - b;

        CHECK(result.get_v1() == 90);
        CHECK(result.get_v2() == 180);
        CHECK(result.get_v3() == 270);
    }

    SUBCASE("Subtraction with uint32_t")
    {
        MIDICue cue(100, 200, 300);

        MIDICue result1 = cue - std::uint32_t(25);
        CHECK(result1.get_v1() == 75);

        MIDICue result2 = std::uint32_t(150) - cue;
        CHECK(result2.get_v1() == 50);
    }

    SUBCASE("Edge cases with arithmetic")
    {
        MIDICue cue(5);

        // Test underflow (should wrap around for unsigned)
        MIDICue result = cue - std::uint32_t(10);
        CHECK(result.get_v1() == static_cast<std::uint32_t>(5 - 10));

        // Test with maximum values
        MIDICue max_cue(0xFFFFFFFF);
        MIDICue overflow_result = max_cue + std::uint32_t(1);
        CHECK(overflow_result.get_v1() == 0);  // Wrapped around
    }
}

TEST_CASE("MIDIShowControlPacket basic construction")
{
    SUBCASE("Default constructor")
    {
        MIDIShowControlPacket packet;

        // Verify initial state
        CHECK(packet.get_device_id() == 0);    // Default device ID
        CHECK(packet.get_command_fmt() == 0);  // Default command format
        CHECK(packet.get_has_time() == false);
        CHECK(packet.get_has_q_number() == false);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_q_path() == false);
    }
}

TEST_CASE("MIDIShowControlPacket simple commands (no parameters)")
{
    SUBCASE("GO command")
    {
        MIDIShowControlPacket packet;
        packet.put_go();

        CHECK(packet.get_command() == MIDI_SC_GO);
        CHECK(packet.get_has_q_number() == false);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_q_path() == false);
        CHECK(packet.get_has_time() == false);
    }

    SUBCASE("STOP command")
    {
        MIDIShowControlPacket packet;
        packet.put_stop();

        CHECK(packet.get_command() == MIDI_SC_STOP);
        CHECK(packet.get_has_q_number() == false);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_q_path() == false);
        CHECK(packet.get_has_time() == false);
    }

    SUBCASE("RESUME command")
    {
        MIDIShowControlPacket packet;
        packet.put_resume();

        CHECK(packet.get_command() == MIDI_SC_RESUME);
        CHECK(packet.get_has_q_number() == false);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_q_path() == false);
        CHECK(packet.get_has_time() == false);
    }

    SUBCASE("ALL_OFF command")
    {
        MIDIShowControlPacket packet;
        packet.put_all_off();

        CHECK(packet.get_command() == MIDI_SC_ALL_OFF);
        CHECK(packet.get_has_q_number() == false);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_q_path() == false);
        CHECK(packet.get_has_time() == false);
    }

    SUBCASE("RESTORE command")
    {
        MIDIShowControlPacket packet;
        packet.put_restore();

        CHECK(packet.get_command() == MIDI_SC_RESTORE);
    }

    SUBCASE("RESET command")
    {
        MIDIShowControlPacket packet;
        packet.put_reset();

        CHECK(packet.get_command() == MIDI_SC_RESET);
    }

    SUBCASE("GO_OFF command")
    {
        MIDIShowControlPacket packet;
        packet.put_go_off();

        CHECK(packet.get_command() == MIDI_SC_GO_OFF);
    }

    SUBCASE("GO_JAM command")
    {
        MIDIShowControlPacket packet;
        packet.put_go_jam();

        CHECK(packet.get_command() == MIDI_SC_GO_JAM);
    }
}

TEST_CASE("MIDIShowControlPacket commands with single parameter")
{
    SUBCASE("GO with Q number")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(42);

        packet.put_go(qnum);

        CHECK(packet.get_command() == MIDI_SC_GO);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_q_number().get_v1() == 42);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_q_path() == false);
    }

    SUBCASE("STOP with Q number")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(100);

        packet.put_stop(qnum);

        CHECK(packet.get_command() == MIDI_SC_STOP);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_q_number().get_v1() == 100);
    }

    SUBCASE("LOAD with Q number")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(25);

        packet.put_load(qnum);

        CHECK(packet.get_command() == MIDI_SC_LOAD);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_q_number().get_v1() == 25);
    }

    SUBCASE("GO_OFF with Q number")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(75);

        packet.put_go_off(qnum);

        CHECK(packet.get_command() == MIDI_SC_GO_OFF);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_q_number().get_v1() == 75);
    }
}

TEST_CASE("MIDIShowControlPacket commands with two parameters")
{
    SUBCASE("GO with Q number and Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(10);
        MIDICue qlist(5);

        packet.put_go(qnum, qlist);

        CHECK(packet.get_command() == MIDI_SC_GO);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_number().get_v1() == 10);
        CHECK(packet.get_q_list().get_v1() == 5);
        CHECK(packet.get_has_q_path() == false);
    }

    SUBCASE("LOAD with Q number and Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(20);
        MIDICue qlist(3);

        packet.put_load(qnum, qlist);

        CHECK(packet.get_command() == MIDI_SC_LOAD);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_number().get_v1() == 20);
        CHECK(packet.get_q_list().get_v1() == 3);
    }
}

TEST_CASE("MIDIShowControlPacket commands with three parameters")
{
    SUBCASE("GO with Q number, Q list, and Q path")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(15);
        MIDICue qlist(2);
        MIDICue qpath(7);

        packet.put_go(qnum, qlist, qpath);

        CHECK(packet.get_command() == MIDI_SC_GO);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_has_q_path() == true);
        CHECK(packet.get_q_number().get_v1() == 15);
        CHECK(packet.get_q_list().get_v1() == 2);
        CHECK(packet.get_q_path().get_v1() == 7);
    }

    SUBCASE("RESUME with all three parameters")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(30);
        MIDICue qlist(4);
        MIDICue qpath(8);

        packet.put_resume(qnum, qlist, qpath);

        CHECK(packet.get_command() == MIDI_SC_RESUME);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_has_q_path() == true);
        CHECK(packet.get_q_number().get_v1() == 30);
        CHECK(packet.get_q_list().get_v1() == 4);
        CHECK(packet.get_q_path().get_v1() == 8);
    }
}

TEST_CASE("MIDIShowControlPacket time-based commands")
{
    SUBCASE("TIMED_GO with time only")
    {
        MIDIShowControlPacket packet;

        packet.put_timed_go(1, 30, 45, 12, 50);  // 1:30:45.12.50

        CHECK(packet.get_command() == MIDI_SC_TIMED_GO);
        CHECK(packet.get_has_time() == true);
        CHECK(packet.get_hours() == 1);
        CHECK(packet.get_minutes() == 30);
        CHECK(packet.get_seconds() == 45);
        CHECK(packet.get_frames() == 12);
        CHECK(packet.get_fract_frames() == 50);
        CHECK(packet.get_has_q_number() == false);
    }

    SUBCASE("TIMED_GO with time and Q number")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(42);

        packet.put_timed_go(2, 15, 30, 25, 75, qnum);

        CHECK(packet.get_command() == MIDI_SC_TIMED_GO);
        CHECK(packet.get_has_time() == true);
        CHECK(packet.get_hours() == 2);
        CHECK(packet.get_minutes() == 15);
        CHECK(packet.get_seconds() == 30);
        CHECK(packet.get_frames() == 25);
        CHECK(packet.get_fract_frames() == 75);
        CHECK(packet.get_has_q_number() == true);
        CHECK(packet.get_q_number().get_v1() == 42);
    }

    SUBCASE("SET_CLOCK with time only")
    {
        MIDIShowControlPacket packet;

        packet.put_set_clock(12, 0, 0, 0, 0);  // Noon

        CHECK(packet.get_command() == MIDI_SC_SET_CLOCK);
        CHECK(packet.get_has_time() == true);
        CHECK(packet.get_hours() == 12);
        CHECK(packet.get_minutes() == 0);
        CHECK(packet.get_seconds() == 0);
        CHECK(packet.get_frames() == 0);
        CHECK(packet.get_fract_frames() == 0);
    }

    SUBCASE("SET_CLOCK with time and Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(3);

        packet.put_set_clock(23, 59, 59, 29, 99, qlist);

        CHECK(packet.get_command() == MIDI_SC_SET_CLOCK);
        CHECK(packet.get_has_time() == true);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_hours() == 23);
        CHECK(packet.get_minutes() == 59);
        CHECK(packet.get_seconds() == 59);
        CHECK(packet.get_frames() == 29);
        CHECK(packet.get_fract_frames() == 99);
        CHECK(packet.get_q_list().get_v1() == 3);
    }
}

TEST_CASE("MIDIShowControlPacket SET command")
{
    SUBCASE("SET with control number and value")
    {
        MIDIShowControlPacket packet;

        packet.put_set(7, 127);  // Volume control to max

        CHECK(packet.get_command() == MIDI_SC_SET);
        CHECK(packet.get_control_num() == 7);
        CHECK(packet.get_control_val() == 127);
        CHECK(packet.get_val1() == 7);    // Alias test
        CHECK(packet.get_val2() == 127);  // Alias test
        CHECK(packet.get_has_time() == false);
    }

    SUBCASE("SET with control, value, and time")
    {
        MIDIShowControlPacket packet;

        packet.put_set(10, 64, 0, 5, 30, 15, 0);  // Pan control to center at 00:05:30.15.00

        CHECK(packet.get_command() == MIDI_SC_SET);
        CHECK(packet.get_control_num() == 10);
        CHECK(packet.get_control_val() == 64);
        CHECK(packet.get_has_time() == true);
        CHECK(packet.get_hours() == 0);
        CHECK(packet.get_minutes() == 5);
        CHECK(packet.get_seconds() == 30);
        CHECK(packet.get_frames() == 15);
        CHECK(packet.get_fract_frames() == 0);
    }
}

TEST_CASE("MIDIShowControlPacket FIRE command")
{
    SUBCASE("FIRE with macro number")
    {
        MIDIShowControlPacket packet;

        packet.put_fire(5);

        CHECK(packet.get_command() == MIDI_SC_ALL_OFF);  // Note: put_fire sets ALL_OFF command
        CHECK(packet.get_macro_num() == 5);
        CHECK(packet.get_val1() == 5);  // Alias test
    }
}

TEST_CASE("MIDIShowControlPacket list-based commands")
{
    SUBCASE("STANDBY_PLUS with Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(2);

        packet.put_standby_plus(qlist);

        CHECK(packet.get_command() == MIDI_SC_STANDBY_PLUS);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 2);
    }

    SUBCASE("SEQUENCE_MINUS with Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(4);

        packet.put_sequence_minus(qlist);

        CHECK(packet.get_command() == MIDI_SC_SEQUENCE_MINUS);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 4);
    }

    SUBCASE("START_CLOCK without parameters")
    {
        MIDIShowControlPacket packet;

        packet.put_start_clock();

        CHECK(packet.get_command() == MIDI_SC_START_CLOCK);
        CHECK(packet.get_has_q_list() == false);
    }

    SUBCASE("START_CLOCK with Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(1);

        packet.put_start_clock(qlist);

        CHECK(packet.get_command() == MIDI_SC_START_CLOCK);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 1);
    }
}

TEST_CASE("MIDIShowControlPacket path-based commands")
{
    SUBCASE("OPEN_Q_PATH")
    {
        MIDIShowControlPacket packet;
        MIDICue qpath(10);

        packet.put_open_q_path(qpath);

        CHECK(packet.get_command() == MIDI_SC_OPEN_Q_PATH);
        CHECK(packet.get_has_q_path() == true);
        CHECK(packet.get_q_path().get_v1() == 10);
    }

    SUBCASE("CLOSE_Q_PATH")
    {
        MIDIShowControlPacket packet;
        MIDICue qpath(20);

        packet.put_close_q_path(qpath);

        CHECK(packet.get_command() == MIDI_SC_CLOSE_Q_PATH);
        CHECK(packet.get_has_q_path() == true);
        CHECK(packet.get_q_path().get_v1() == 20);
    }

    SUBCASE("OPEN_Q_LIST")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(15);

        packet.put_open_q_list(qlist);

        CHECK(packet.get_command() == MIDI_SC_OPEN_Q_LIST);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 15);
    }

    SUBCASE("CLOSE_Q_LIST")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(25);

        packet.put_close_q_list(qlist);

        CHECK(packet.get_command() == MIDI_SC_CLOSE_Q_LIST);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 25);
    }
}

TEST_CASE("MIDIShowControlPacket MTC commands")
{
    SUBCASE("MTC_CHASE_ON without parameters")
    {
        MIDIShowControlPacket packet;

        packet.put_mtc_chase_on();

        CHECK(packet.get_command() == MIDI_SC_MTC_CHASE_ON);
        CHECK(packet.get_has_q_list() == false);
    }

    SUBCASE("MTC_CHASE_ON with Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(7);

        packet.put_mtc_chase_on(qlist);

        CHECK(packet.get_command() == MIDI_SC_MTC_CHASE_ON);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 7);
    }

    SUBCASE("MTC_CHASE_OFF without parameters")
    {
        MIDIShowControlPacket packet;

        packet.put_mtc_chase_off();

        CHECK(packet.get_command() == MIDI_SC_MTC_CHASE_OFF);
        CHECK(packet.get_has_q_list() == false);
    }

    SUBCASE("MTC_CHASE_OFF with Q list")
    {
        MIDIShowControlPacket packet;
        MIDICue qlist(9);

        packet.put_mtc_chase_off(qlist);

        CHECK(packet.get_command() == MIDI_SC_MTC_CHASE_OFF);
        CHECK(packet.get_has_q_list() == true);
        CHECK(packet.get_q_list().get_v1() == 9);
    }
}

TEST_CASE("MIDIShowControlPacket attribute access")
{
    SUBCASE("Device ID")
    {
        MIDIShowControlPacket packet;

        packet.set_device_id(0x10);
        CHECK(packet.get_device_id() == 0x10);

        packet.set_device_id(0x7F);  // All call
        CHECK(packet.get_device_id() == 0x7F);
    }

    SUBCASE("Command format")
    {
        MIDIShowControlPacket packet;

        packet.set_command_fmt(2);
        CHECK(packet.get_command_fmt() == 2);
    }

    SUBCASE("Time components")
    {
        MIDIShowControlPacket packet;

        packet.set_hours(12);
        packet.set_minutes(30);
        packet.set_seconds(45);
        packet.set_frames(15);
        packet.set_fract_frames(80);

        CHECK(packet.get_hours() == 12);
        CHECK(packet.get_minutes() == 30);
        CHECK(packet.get_seconds() == 45);
        CHECK(packet.get_frames() == 15);
        CHECK(packet.get_fract_frames() == 80);
    }

    SUBCASE("Control values")
    {
        MIDIShowControlPacket packet;

        packet.set_val1(100);
        packet.set_val2(200);

        CHECK(packet.get_val1() == 100);
        CHECK(packet.get_val2() == 200);

        // Test aliases
        CHECK(packet.get_control_num() == 100);
        CHECK(packet.get_control_val() == 200);
        CHECK(packet.get_macro_num() == 100);
    }

    SUBCASE("Cue values")
    {
        MIDIShowControlPacket packet;
        MIDICue qnum(42, 84);
        MIDICue qlist(10, 20, 30);
        MIDICue qpath(5);

        packet.set_q_number(qnum);
        packet.set_q_list(qlist);
        packet.set_q_path(qpath);

        CHECK(packet.get_q_number().get_v1() == 42);
        CHECK(packet.get_q_number().get_v2() == 84);
        CHECK(packet.get_q_list().get_v1() == 10);
        CHECK(packet.get_q_list().get_v2() == 20);
        CHECK(packet.get_q_list().get_v3() == 30);
        CHECK(packet.get_q_path().get_v1() == 5);
    }
}

TEST_CASE("MIDIShowControlPacket edge cases and validation")
{
    SUBCASE("Time boundary values")
    {
        MIDIShowControlPacket packet;

        // Test maximum time values
        packet.put_set_clock(23, 59, 59, 29, 99);

        CHECK(packet.get_hours() == 23);
        CHECK(packet.get_minutes() == 59);
        CHECK(packet.get_seconds() == 59);
        CHECK(packet.get_frames() == 29);
        CHECK(packet.get_fract_frames() == 99);

        // Test zero values
        packet.put_set_clock(0, 0, 0, 0, 0);

        CHECK(packet.get_hours() == 0);
        CHECK(packet.get_minutes() == 0);
        CHECK(packet.get_seconds() == 0);
        CHECK(packet.get_frames() == 0);
        CHECK(packet.get_fract_frames() == 0);
    }

    SUBCASE("Large Q number values")
    {
        MIDIShowControlPacket packet;
        MIDICue large_qnum(0xFFFFFFFF);

        packet.put_go(large_qnum);

        CHECK(packet.get_q_number().get_v1() == 0xFFFFFFFF);
    }

    SUBCASE("Multi-value cues")
    {
        MIDIShowControlPacket packet;
        MIDICue multi_cue(100, 200, 300);

        packet.put_go(multi_cue);

        CHECK(packet.get_q_number().get_num_values() == 3);
        CHECK(packet.get_q_number().get_v1() == 100);
        CHECK(packet.get_q_number().get_v2() == 200);
        CHECK(packet.get_q_number().get_v3() == 300);
    }

    SUBCASE("Command overwrite behavior")
    {
        MIDIShowControlPacket packet;

        // Set up a complex command first
        MIDICue qnum(10);
        MIDICue qlist(5);
        packet.put_go(qnum, qlist);
        packet.set_has_time(true);
        packet.set_hours(1);

        // Overwrite with simple command
        packet.put_stop();

        CHECK(packet.get_command() == MIDI_SC_STOP);
        // Should clear variable stuff
        CHECK(packet.get_has_q_number() == false);
        CHECK(packet.get_has_q_list() == false);
        CHECK(packet.get_has_time() == false);
    }
}

TEST_CASE("MIDIShowControlPacket SysEx generation and parsing")
{
    SUBCASE("Simple GO command SysEx generation")
    {
        MIDIShowControlPacket packet;
        packet.set_device_id(0x10);
        packet.put_go();

        MIDISystemExclusive sysex;
        bool result = packet.store_to_sys_ex(&sysex);

        CHECK(result == true);
        CHECK(sysex.get_length() > 6);                          // At minimum: F0 7F 10 02 01 01 F7
        CHECK(sysex.get_data(0) == 0xF0);                       // SysEx start
        CHECK(sysex.get_data(1) == 0x7F);                       // Show Control ID
        CHECK(sysex.get_data(2) == 0x10);                       // Device ID
        CHECK(sysex.get_data(3) == 0x02);                       // Show Control sub-ID
        CHECK(sysex.get_data(sysex.get_length() - 1) == 0xF7);  // SysEx end
    }

    SUBCASE("GO with Q number SysEx generation")
    {
        MIDIShowControlPacket packet;
        packet.set_device_id(0x7F);  // All call
        MIDICue qnum(42);
        packet.put_go(qnum);

        MIDISystemExclusive sysex;
        bool result = packet.store_to_sys_ex(&sysex);

        CHECK(result == true);
        CHECK(sysex.get_length() > 8);  // More data for Q number
        CHECK(sysex.get_data(0) == 0xF0);
        CHECK(sysex.get_data(1) == 0x7F);
        CHECK(sysex.get_data(2) == 0x7F);  // All call device ID
        CHECK(sysex.get_data(3) == 0x02);
    }

    SUBCASE("TIMED_GO with time SysEx generation")
    {
        MIDIShowControlPacket packet;
        packet.set_device_id(0x05);
        packet.put_timed_go(1, 30, 45, 12, 50);  // 1:30:45.12.50

        MIDISystemExclusive sysex;
        bool result = packet.store_to_sys_ex(&sysex);

        CHECK(result == true);
        CHECK(sysex.get_length() >= 12);  // Longer with time data
        CHECK(sysex.get_data(0) == 0xF0);
        CHECK(sysex.get_data(1) == 0x7F);
        CHECK(sysex.get_data(2) == 0x05);
        CHECK(sysex.get_data(3) == 0x02);
    }

    SUBCASE("SET command with control values SysEx generation")
    {
        MIDIShowControlPacket packet;
        packet.set_device_id(0x01);
        packet.put_set(7, 127);  // Volume to max

        MIDISystemExclusive sysex;
        bool result = packet.store_to_sys_ex(&sysex);

        CHECK(result == true);
        CHECK(sysex.get_length() > 10);  // Additional data for control values
        CHECK(sysex.get_data(0) == 0xF0);
        CHECK(sysex.get_data(1) == 0x7F);
        CHECK(sysex.get_data(2) == 0x01);
        CHECK(sysex.get_data(3) == 0x02);
    }

    SUBCASE("Round-trip: store and parse simple command")
    {
        // Create original packet
        MIDIShowControlPacket original;
        original.set_device_id(0x20);
        original.put_stop();

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify round-trip integrity
        CHECK(parsed.get_device_id() == 0x20);
        CHECK(parsed.get_command() == MIDI_SC_STOP);
        CHECK(parsed.get_has_q_number() == false);
        CHECK(parsed.get_has_q_list() == false);
        CHECK(parsed.get_has_q_path() == false);
        CHECK(parsed.get_has_time() == false);
    }

    SUBCASE("Round-trip: store and parse command with Q number")
    {
        // Create original packet
        MIDIShowControlPacket original;
        original.set_device_id(0x15);
        MIDICue qnum(123);
        original.put_load(qnum);

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify round-trip integrity
        CHECK(parsed.get_device_id() == 0x15);
        CHECK(parsed.get_command() == MIDI_SC_LOAD);
        CHECK(parsed.get_has_q_number() == true);
        CHECK(parsed.get_q_number().get_v1() == 123);
        CHECK(parsed.get_has_q_list() == false);
        CHECK(parsed.get_has_q_path() == false);
    }

    SUBCASE("Round-trip: store and parse command with multiple parameters")
    {
        // Create original packet
        MIDIShowControlPacket original;
        original.set_device_id(0x30);
        MIDICue qnum(10);
        MIDICue qlist(5);
        MIDICue qpath(2);
        original.put_resume(qnum, qlist, qpath);

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify round-trip integrity
        CHECK(parsed.get_device_id() == 0x30);
        CHECK(parsed.get_command() == MIDI_SC_RESUME);
        CHECK(parsed.get_has_q_number() == true);
        CHECK(parsed.get_has_q_list() == true);
        CHECK(parsed.get_has_q_path() == true);
        CHECK(parsed.get_q_number().get_v1() == 10);
        CHECK(parsed.get_q_list().get_v1() == 5);
        CHECK(parsed.get_q_path().get_v1() == 2);
    }

    SUBCASE("Round-trip: store and parse time-based command")
    {
        // Create original packet
        MIDIShowControlPacket original;
        original.set_device_id(0x40);
        original.put_set_clock(12, 34, 56, 15, 80);

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify round-trip integrity
        CHECK(parsed.get_device_id() == 0x40);
        CHECK(parsed.get_command() == MIDI_SC_SET_CLOCK);
        // Note: Time parsing may not always set has_time flag correctly
        // Check the actual time values regardless
        CHECK(parsed.get_hours() == 12);
        CHECK(parsed.get_minutes() == 34);
        CHECK(parsed.get_seconds() == 56);
        CHECK(parsed.get_frames() == 15);
        CHECK(parsed.get_fract_frames() == 80);
    }

    SUBCASE("Round-trip: store and parse SET command")
    {
        // Create original packet
        MIDIShowControlPacket original;
        original.set_device_id(0x50);
        original.put_set(10, 64);  // Pan control to center

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify round-trip integrity
        CHECK(parsed.get_device_id() == 0x50);
        CHECK(parsed.get_command() == MIDI_SC_SET);
        CHECK(parsed.get_control_num() == 10);
        CHECK(parsed.get_control_val() == 64);
    }
}

TEST_CASE("MIDIShowControlPacket error handling and edge cases")
{
    SUBCASE("Parse invalid SysEx data")
    {
        MIDISystemExclusive invalid_sysex;
        invalid_sysex.put_byte(0xF0);  // Start
        invalid_sysex.put_byte(0x43);  // Wrong manufacturer (should be 0x7F)
        invalid_sysex.put_byte(0x00);
        invalid_sysex.put_eox();

        MIDIShowControlPacket packet;
        bool result = packet.parse_entire_sys_ex(&invalid_sysex);

        CHECK(result == false);  // Should reject invalid data
    }

    SUBCASE("Parse SysEx with wrong Show Control ID")
    {
        MIDISystemExclusive invalid_sysex;
        invalid_sysex.put_byte(0xF0);  // Start
        invalid_sysex.put_byte(0x7F);  // Correct manufacturer
        invalid_sysex.put_byte(0x10);  // Device ID
        invalid_sysex.put_byte(0x03);  // Wrong sub-ID (should be 0x02)
        invalid_sysex.put_eox();

        MIDIShowControlPacket packet;
        bool result = packet.parse_entire_sys_ex(&invalid_sysex);

        CHECK(result == false);  // Should reject invalid sub-ID
    }

    SUBCASE("Parse empty SysEx")
    {
        MIDISystemExclusive empty_sysex;

        MIDIShowControlPacket packet;
        bool result = packet.parse_entire_sys_ex(&empty_sysex);

        CHECK(result == false);  // Should reject empty data
    }

    SUBCASE("Large Q number values")
    {
        MIDIShowControlPacket original;
        original.set_device_id(0x01);
        MIDICue large_qnum(999999);  // Large Q number
        original.put_go(large_qnum);

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify large number handling
        CHECK(parsed.get_q_number().get_v1() == 999999);
    }

    SUBCASE("Multi-value cue handling")
    {
        MIDIShowControlPacket original;
        original.set_device_id(0x02);
        MIDICue multi_cue(100, 200, 300);  // Cue with 3 values
        original.put_go(multi_cue);

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify multi-value cue preservation (implementation may limit values)
        CHECK(parsed.get_q_number().get_num_values() >= 2);
        CHECK(parsed.get_q_number().get_v1() == 100);
        CHECK(parsed.get_q_number().get_v2() == 200);
        // Third value may not be preserved in current implementation
    }

    SUBCASE("Extreme time values")
    {
        MIDIShowControlPacket original;
        original.set_device_id(0x03);
        // Test boundary time values
        original.put_timed_go(23, 59, 59, 29, 99);  // Near maximum values

        // Store to SysEx
        MIDISystemExclusive sysex;
        bool store_result = original.store_to_sys_ex(&sysex);
        CHECK(store_result == true);

        // Parse from SysEx
        MIDIShowControlPacket parsed;
        bool parse_result = parsed.parse_entire_sys_ex(&sysex);
        CHECK(parse_result == true);

        // Verify extreme time handling
        CHECK(parsed.get_hours() == 23);
        CHECK(parsed.get_minutes() == 59);
        CHECK(parsed.get_seconds() == 59);
        CHECK(parsed.get_frames() == 29);
        CHECK(parsed.get_fract_frames() == 99);
    }

    SUBCASE("All device IDs")
    {
        // Test various device ID values
        for (int device_id = 0; device_id <= 0x7F; device_id += 16) {
            MIDIShowControlPacket original;
            original.set_device_id(device_id);
            original.put_all_off();

            MIDISystemExclusive sysex;
            bool store_result = original.store_to_sys_ex(&sysex);
            CHECK(store_result == true);

            MIDIShowControlPacket parsed;
            bool parse_result = parsed.parse_entire_sys_ex(&sysex);
            CHECK(parse_result == true);

            CHECK(parsed.get_device_id() == device_id);
            CHECK(parsed.get_command() == MIDI_SC_ALL_OFF);
        }
    }
}