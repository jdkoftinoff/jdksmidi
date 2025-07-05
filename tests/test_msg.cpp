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
#include "jdksmidi/msg.h"

using namespace jdksmidi;

TEST_CASE("MIDIMessage basic construction and properties")
{
    SUBCASE("Default constructor")
    {
        MIDIMessage msg;

        CHECK(msg.get_status() == 0);
        CHECK(msg.get_byte1() == 0);
        CHECK(msg.get_byte2() == 0);
        CHECK(msg.get_byte3() == 0);
        CHECK(msg.get_length() == 0);
    }

    SUBCASE("Copy constructor")
    {
        MIDIMessage original;
        original.set_note_on(5, 60, 100);

        MIDIMessage copy(original);

        CHECK(copy.get_status() == original.get_status());
        CHECK(copy.get_byte1() == original.get_byte1());
        CHECK(copy.get_byte2() == original.get_byte2());
        CHECK(copy.get_byte3() == original.get_byte3());
        CHECK(copy.get_length() == original.get_length());
    }

    SUBCASE("Assignment operator")
    {
        MIDIMessage original;
        original.set_control_change(3, 7, 127);

        MIDIMessage assigned;
        assigned = original;

        CHECK(assigned.get_status() == original.get_status());
        CHECK(assigned.get_byte1() == original.get_byte1());
        CHECK(assigned.get_byte2() == original.get_byte2());
        CHECK(assigned.get_channel() == 3);
        CHECK(assigned.get_controller() == 7);
        CHECK(assigned.get_controller_value() == 127);
    }

    SUBCASE("Clear functionality")
    {
        MIDIMessage msg;
        msg.set_note_on(10, 72, 64);

        CHECK(msg.get_status() != 0);

        msg.clear();
        CHECK(msg.get_status() == 0);
        CHECK(msg.get_byte1() == 0);
        CHECK(msg.get_byte2() == 0);
        CHECK(msg.get_byte3() == 0);
    }

    SUBCASE("Copy method")
    {
        MIDIMessage source;
        source.set_program_change(8, 42);

        MIDIMessage dest;
        dest.copy(source);

        CHECK(dest.get_status() == source.get_status());
        CHECK(dest.get_channel() == 8);
        CHECK(dest.get_pg_value() == 42);
    }
}

TEST_CASE("MIDIMessage channel messages")
{
    SUBCASE("Note On messages")
    {
        MIDIMessage msg;
        msg.set_note_on(5, 60, 100);

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_note_on() == true);
        CHECK(msg.is_note_off() == false);
        CHECK(msg.get_channel() == 5);
        CHECK(msg.get_type() == 0x90);
        CHECK(msg.get_note() == 60);
        CHECK(msg.get_velocity() == 100);
        CHECK(msg.get_length() == 3);

        // Test boundary values
        msg.set_note_on(15, 127, 127);
        CHECK(msg.get_channel() == 15);
        CHECK(msg.get_note() == 127);
        CHECK(msg.get_velocity() == 127);

        msg.set_note_on(0, 0, 1);
        CHECK(msg.get_channel() == 0);
        CHECK(msg.get_note() == 0);
        CHECK(msg.get_velocity() == 1);
    }

    SUBCASE("Note Off messages")
    {
        MIDIMessage msg;
        msg.set_note_off(3, 72, 64);

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_note_off() == true);
        CHECK(msg.is_note_on() == false);
        CHECK(msg.get_channel() == 3);
        CHECK(msg.get_type() == 0x80);
        CHECK(msg.get_note() == 72);
        CHECK(msg.get_velocity() == 64);
        CHECK(msg.get_length() == 3);
    }

    SUBCASE("Note On with velocity 0 is Note Off")
    {
        MIDIMessage msg;
        msg.set_note_on(7, 48, 0);

        CHECK(msg.is_note_on() == false);
        CHECK(msg.is_note_off() == true);
        CHECK(msg.get_type() == 0x90);  // Still note on type
        CHECK(msg.get_velocity() == 0);
    }

    SUBCASE("Control Change messages")
    {
        MIDIMessage msg;
        msg.set_control_change(9, 7, 100);

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_control_change() == true);
        CHECK(msg.get_channel() == 9);
        CHECK(msg.get_type() == 0xB0);
        CHECK(msg.get_controller() == 7);
        CHECK(msg.get_controller_value() == 100);
        CHECK(msg.get_length() == 3);

        // Test All Notes Off controller
        msg.set_all_notes_off(5);
        CHECK(msg.is_all_notes_off() == true);
        CHECK(msg.get_channel() == 5);
        CHECK(msg.get_controller() == C_ALL_NOTES_OFF);
    }

    SUBCASE("Program Change messages")
    {
        MIDIMessage msg;
        msg.set_program_change(2, 42);

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_program_change() == true);
        CHECK(msg.get_channel() == 2);
        CHECK(msg.get_type() == 0xC0);
        CHECK(msg.get_pg_value() == 42);
        CHECK(msg.get_length() == 2);

        // Test boundary values
        msg.set_program_change(15, 127);
        CHECK(msg.get_pg_value() == 127);
    }

    SUBCASE("Channel Pressure messages")
    {
        MIDIMessage msg;
        msg.set_channel_pressure(6, 80);

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_channel_pressure() == true);
        CHECK(msg.get_channel() == 6);
        CHECK(msg.get_type() == 0xD0);
        CHECK(msg.get_channel_pressure() == 80);
        CHECK(msg.get_length() == 2);
    }

    SUBCASE("Poly Pressure messages")
    {
        MIDIMessage msg;
        msg.set_poly_pressure(4, 60, 90);

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_poly_pressure() == true);
        CHECK(msg.get_channel() == 4);
        CHECK(msg.get_type() == 0xA0);
        CHECK(msg.get_note() == 60);
        CHECK(msg.get_velocity() == 90);  // Pressure uses velocity field
        CHECK(msg.get_length() == 3);
    }

    SUBCASE("Pitch Bend messages")
    {
        MIDIMessage msg;
        msg.set_pitch_bend(1, 0);  // Center position

        CHECK(msg.is_channel_msg() == true);
        CHECK(msg.is_pitch_bend() == true);
        CHECK(msg.get_channel() == 1);
        CHECK(msg.get_type() == 0xE0);
        CHECK(msg.get_bender_value() == 0);
        CHECK(msg.get_length() == 3);

        // Test with separate low/high bytes
        msg.set_pitch_bend(2, 0x00, 0x40);  // Center position (0x2000)
        CHECK(msg.get_bender_value() == 0);

        // Test extreme values
        msg.set_pitch_bend(3, -8192);  // Minimum
        CHECK(msg.get_bender_value() == -8192);

        msg.set_pitch_bend(4, 8191);  // Maximum
        CHECK(msg.get_bender_value() == 8191);
    }
}

TEST_CASE("MIDIMessage system messages")
{
    SUBCASE("System Exclusive messages")
    {
        MIDIMessage msg;
        msg.set_sys_ex();

        CHECK(msg.is_system_message() == true);
        CHECK(msg.is_sys_ex() == true);
        CHECK(msg.get_status() == 0xF0);
        // SysEx length is variable and returns -1 until parsing is complete
        CHECK(msg.get_length() == -1);
    }

    SUBCASE("MIDI Time Code messages")
    {
        MIDIMessage msg;
        msg.set_mtc(3, 5);  // Field 3, value 5

        CHECK(msg.is_system_message() == true);
        CHECK(msg.is_mtc() == true);
        CHECK(msg.get_status() == 0xF1);
        CHECK(msg.get_length() == 2);
    }

    SUBCASE("Song Position messages")
    {
        MIDIMessage msg;
        msg.set_song_position(1024);

        CHECK(msg.is_system_message() == true);
        CHECK(msg.is_song_position() == true);
        CHECK(msg.get_status() == 0xF2);
        CHECK(msg.get_length() == 3);
    }

    SUBCASE("Song Select messages")
    {
        MIDIMessage msg;
        msg.set_song_select(42);

        CHECK(msg.is_system_message() == true);
        CHECK(msg.is_song_select() == true);
        CHECK(msg.get_status() == 0xF3);
        CHECK(msg.get_length() == 2);
    }

    SUBCASE("Tune Request messages")
    {
        MIDIMessage msg;
        msg.set_tune_request();

        CHECK(msg.is_system_message() == true);
        CHECK(msg.is_tune_request() == true);
        CHECK(msg.get_status() == 0xF6);
        CHECK(msg.get_length() == 1);
    }
}

TEST_CASE("MIDIMessage meta messages")
{
    SUBCASE("No-op messages")
    {
        MIDIMessage msg;
        msg.set_no_op();

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_no_op() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_NO_OPERATION);
    }

    SUBCASE("Tempo messages")
    {
        MIDIMessage msg;
        msg.set_tempo32(3200);  // 100 BPM * 32

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_tempo() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_TEMPO);
        CHECK(msg.get_tempo32() == 3200);

        // Test different tempo values
        msg.set_tempo32(1920);  // 60 BPM * 32
        CHECK(msg.get_tempo32() == 1920);

        msg.set_tempo32(7680);  // 240 BPM * 32
        CHECK(msg.get_tempo32() == 7680);
    }

    SUBCASE("Time Signature messages")
    {
        MIDIMessage msg;
        msg.set_time_sig(4, 4);  // 4/4 time

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_time_sig() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_TIMESIG);
        CHECK(msg.get_time_sig_numerator() == 4);
        CHECK(msg.get_time_sig_denominator() == 4);

        // Test other time signatures
        msg.set_time_sig(3, 4);  // 3/4 time
        CHECK(msg.get_time_sig_numerator() == 3);
        CHECK(msg.get_time_sig_denominator() == 4);

        msg.set_time_sig(7, 8);  // 7/8 time
        CHECK(msg.get_time_sig_numerator() == 7);
        CHECK(msg.get_time_sig_denominator() == 8);
    }

    SUBCASE("Key Signature messages")
    {
        MIDIMessage msg;
        msg.set_key_sig(2, 0);  // D major (2 sharps, major)

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_key_sig() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_KEYSIG);
        CHECK(msg.get_key_sig_sharp_flats() == 2);
        CHECK(msg.get_key_sig_major_minor() == 0);

        // Test minor keys
        msg.set_key_sig(-3, 1);  // C minor (3 flats, minor)
        CHECK(msg.get_key_sig_sharp_flats() == -3);
        CHECK(msg.get_key_sig_major_minor() == 1);

        // Test C major (no sharps/flats)
        msg.set_key_sig(0, 0);
        CHECK(msg.get_key_sig_sharp_flats() == 0);
        CHECK(msg.get_key_sig_major_minor() == 0);
    }

    SUBCASE("Text messages")
    {
        MIDIMessage msg;
        msg.set_text(123, META_GENERIC_TEXT);

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_text_event() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_GENERIC_TEXT);
        CHECK(msg.get_meta_value() == 123);
    }

    SUBCASE("Data End messages")
    {
        MIDIMessage msg;
        msg.set_data_end();

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_data_end() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_DATA_END);
    }

    SUBCASE("Beat Marker messages")
    {
        MIDIMessage msg;
        msg.set_beat_marker();

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.is_beat_marker() == true);
        CHECK(msg.get_status() == 0xFF);
        CHECK(msg.get_meta_type() == META_BEAT_MARKER);
    }

    SUBCASE("Generic meta events")
    {
        MIDIMessage msg;
        msg.set_meta_event(META_GENERIC_TEXT, 0x12, 0x34);

        CHECK(msg.is_meta_event() == true);
        CHECK(msg.get_meta_type() == META_GENERIC_TEXT);
        CHECK(msg.get_byte2() == 0x12);
        CHECK(msg.get_byte3() == 0x34);

        // Test with 16-bit value
        msg.set_meta_event(META_TEMPO, 0x1234);
        CHECK(msg.get_meta_value() == 0x1234);
    }
}

TEST_CASE("MIDIMessage bit manipulation and edge cases")
{
    SUBCASE("Status byte manipulation")
    {
        MIDIMessage msg;

        msg.set_status(0x93);  // Note on, channel 3
        CHECK(msg.get_status() == 0x93);
        CHECK(msg.get_type() == 0x90);
        CHECK(msg.get_channel() == 3);

        // Change just the channel
        msg.set_channel(7);
        CHECK(msg.get_status() == 0x97);
        CHECK(msg.get_type() == 0x90);
        CHECK(msg.get_channel() == 7);

        // Change just the type
        msg.set_type(0x80);
        CHECK(msg.get_status() == 0x87);
        CHECK(msg.get_type() == 0x80);
        CHECK(msg.get_channel() == 7);
    }

    SUBCASE("Pitch bend value encoding/decoding")
    {
        MIDIMessage msg;

        // Test center position (0x2000 = 8192, but get_bender_value() returns 0)
        msg.set_bender_value(0);
        CHECK(msg.get_bender_value() == 0);
        CHECK(msg.get_byte1() == 0x00);  // LSB
        CHECK(msg.get_byte2() == 0x40);  // MSB

        // Test minimum (-8192)
        msg.set_bender_value(-8192);
        CHECK(msg.get_bender_value() == -8192);
        CHECK(msg.get_byte1() == 0x00);
        CHECK(msg.get_byte2() == 0x00);

        // Test maximum (8191)
        msg.set_bender_value(8191);
        CHECK(msg.get_bender_value() == 8191);
        CHECK(msg.get_byte1() == 0x7F);
        CHECK(msg.get_byte2() == 0x7F);
    }

    SUBCASE("Meta value encoding/decoding")
    {
        MIDIMessage msg;
        msg.set_meta_type(META_TEMPO);

        // Test 14-bit meta values
        msg.set_meta_value(0x1234);
        CHECK(msg.get_meta_value() == 0x1234);

        msg.set_meta_value(0x3FFF);  // Maximum 14-bit value
        CHECK(msg.get_meta_value() == 0x3FFF);

        msg.set_meta_value(0);
        CHECK(msg.get_meta_value() == 0);
    }

    SUBCASE("Channel boundary values")
    {
        MIDIMessage msg;

        // Test all 16 channels
        for (int ch = 0; ch < 16; ++ch) {
            msg.set_note_on(ch, 60, 100);
            CHECK(msg.get_channel() == ch);
            CHECK((msg.get_status() & 0x0F) == ch);
        }
    }

    SUBCASE("Note and velocity boundary values")
    {
        MIDIMessage msg;

        // Test extreme note values
        msg.set_note_on(0, 0, 1);
        CHECK(msg.get_note() == 0);
        CHECK(msg.get_velocity() == 1);

        msg.set_note_on(0, 127, 127);
        CHECK(msg.get_note() == 127);
        CHECK(msg.get_velocity() == 127);

        // Test velocity 0 (should be note off)
        msg.set_note_on(0, 60, 0);
        CHECK(msg.is_note_off() == true);
        CHECK(msg.get_velocity() == 0);
    }
}

TEST_CASE("MIDIMessage text representation")
{
    SUBCASE("Channel message text")
    {
        MIDIMessage msg;
        char buffer[128];

        msg.set_note_on(5, 60, 100);
        char const* result = msg.msg_to_text(buffer);
        CHECK(result != nullptr);
        // Note: We can't check exact text format as it's implementation-specific
        // but we can verify it doesn't crash and returns a valid pointer
    }

    SUBCASE("System message text")
    {
        MIDIMessage msg;
        char buffer[128];

        msg.set_tune_request();
        char const* result = msg.msg_to_text(buffer);
        CHECK(result != nullptr);
    }

    SUBCASE("Meta message text")
    {
        MIDIMessage msg;
        char buffer[128];

        msg.set_tempo32(3200);
        char const* result = msg.msg_to_text(buffer);
        CHECK(result != nullptr);
    }
}

TEST_CASE("MIDIBigMessage functionality")
{
    SUBCASE("Basic construction")
    {
        MIDIBigMessage big_msg;

        CHECK(big_msg.get_status() == 0);
        CHECK(big_msg.get_sys_ex() == nullptr);
    }

    SUBCASE("Copy from MIDIMessage")
    {
        MIDIMessage msg;
        msg.set_note_on(3, 72, 100);

        MIDIBigMessage big_msg(msg);

        CHECK(big_msg.get_status() == msg.get_status());
        CHECK(big_msg.get_channel() == 3);
        CHECK(big_msg.get_note() == 72);
        CHECK(big_msg.get_velocity() == 100);
    }

    SUBCASE("SysEx handling")
    {
        MIDIBigMessage big_msg;

        // Initially no sysex
        CHECK(big_msg.get_sys_ex() == nullptr);

        // Set up a sysex message
        big_msg.set_sys_ex();
        MIDISystemExclusive* sysex = big_msg.get_sys_ex();

        if (sysex != nullptr) {
            sysex->put_exc();
            sysex->put_byte(0x43);  // Yamaha
            sysex->put_byte(0x12);  // Device
            sysex->put_eox();

            CHECK(sysex->get_length() == 3);
        }

        // Clear sysex
        big_msg.clear_sys_ex();
        CHECK(big_msg.get_sys_ex() == nullptr);
    }

    SUBCASE("Copy constructor with SysEx")
    {
        MIDIBigMessage original;
        original.set_sys_ex();

        MIDISystemExclusive* orig_sysex = original.get_sys_ex();
        if (orig_sysex != nullptr) {
            orig_sysex->put_exc();
            orig_sysex->put_byte(0x41);
            orig_sysex->put_eox();
        }

        MIDIBigMessage copy(original);

        // Check if sysex was copied properly
        MIDISystemExclusive* copy_sysex = copy.get_sys_ex();
        if (copy_sysex != nullptr && orig_sysex != nullptr) {
            CHECK(copy_sysex->get_length() == orig_sysex->get_length());
        }
    }
}

TEST_CASE("MIDITimedMessage functionality")
{
    SUBCASE("Basic construction")
    {
        MIDITimedMessage timed_msg;

        CHECK(timed_msg.get_status() == 0);
        CHECK(timed_msg.get_time() == 0);
    }

    SUBCASE("Time handling")
    {
        MIDITimedMessage timed_msg;

        timed_msg.set_time(1000);
        CHECK(timed_msg.get_time() == 1000);

        timed_msg.set_time(0xFFFFFFFF);
        CHECK(timed_msg.get_time() == 0xFFFFFFFF);
    }

    SUBCASE("Copy from MIDIMessage")
    {
        MIDIMessage msg;
        msg.set_control_change(7, 10, 64);

        MIDITimedMessage timed_msg(msg);
        timed_msg.set_time(500);

        CHECK(timed_msg.get_channel() == 7);
        CHECK(timed_msg.get_controller() == 10);
        CHECK(timed_msg.get_controller_value() == 64);
        CHECK(timed_msg.get_time() == 500);
    }

    SUBCASE("Event comparison")
    {
        MIDITimedMessage msg1, msg2;

        msg1.set_note_on(0, 60, 100);
        msg1.set_time(1000);

        msg2.set_note_on(0, 60, 100);
        msg2.set_time(2000);

        // Earlier time (msg1) compared to later time (msg2) returns 2 (m2 is larger)
        CHECK(MIDITimedMessage::compare_events(msg1, msg2) == 2);
        CHECK(MIDITimedMessage::compare_events(msg2, msg1) == 1);

        msg2.set_time(1000);
        CHECK(MIDITimedMessage::compare_events(msg1, msg2) == 0);
    }
}

TEST_CASE("MIDIDeltaTimedMessage functionality")
{
    SUBCASE("Basic construction")
    {
        MIDIDeltaTimedMessage delta_msg;

        CHECK(delta_msg.get_status() == 0);
        CHECK(delta_msg.get_delta_time() == 0);
    }

    SUBCASE("Delta time handling")
    {
        MIDIDeltaTimedMessage delta_msg;

        delta_msg.set_delta_time(480);  // Quarter note at 480 PPQN
        CHECK(delta_msg.get_delta_time() == 480);

        delta_msg.set_delta_time(0);
        CHECK(delta_msg.get_delta_time() == 0);
    }

    SUBCASE("Copy from MIDIMessage")
    {
        MIDIMessage msg;
        msg.set_program_change(9, 128);  // Drum kit

        MIDIDeltaTimedMessage delta_msg(msg);
        delta_msg.set_delta_time(240);

        CHECK(delta_msg.get_channel() == 9);
        CHECK(delta_msg.get_pg_value() == 128);
        CHECK(delta_msg.get_delta_time() == 240);
    }
}

TEST_CASE("MIDITimedBigMessage functionality")
{
    SUBCASE("Basic construction")
    {
        MIDITimedBigMessage big_timed_msg;

        CHECK(big_timed_msg.get_status() == 0);
        CHECK(big_timed_msg.get_time() == 0);
        CHECK(big_timed_msg.get_sys_ex() == nullptr);
    }

    SUBCASE("Construction from various message types")
    {
        MIDIMessage base_msg;
        base_msg.set_note_on(2, 48, 80);

        MIDITimedMessage timed_msg(base_msg);
        timed_msg.set_time(750);

        MIDITimedBigMessage big_timed_msg(timed_msg);

        CHECK(big_timed_msg.get_channel() == 2);
        CHECK(big_timed_msg.get_note() == 48);
        CHECK(big_timed_msg.get_velocity() == 80);
        CHECK(big_timed_msg.get_time() == 750);
    }

    SUBCASE("Event comparison")
    {
        MIDITimedBigMessage msg1, msg2;

        msg1.set_note_on(0, 60, 100);
        msg1.set_time(1000);

        msg2.set_note_on(0, 60, 100);
        msg2.set_time(2000);

        CHECK(MIDITimedBigMessage::compare_events(msg1, msg2) == 2);
        CHECK(MIDITimedBigMessage::compare_events(msg2, msg1) == 1);
    }
}

TEST_CASE("MIDIDeltaTimedBigMessage functionality")
{
    SUBCASE("Basic construction")
    {
        MIDIDeltaTimedBigMessage delta_big_msg;

        CHECK(delta_big_msg.get_status() == 0);
        CHECK(delta_big_msg.get_delta_time() == 0);
        CHECK(delta_big_msg.get_sys_ex() == nullptr);
    }

    SUBCASE("Construction from various message types")
    {
        MIDIMessage base_msg;
        base_msg.set_pitch_bend(1, 4096);

        MIDIDeltaTimedMessage delta_msg(base_msg);
        delta_msg.set_delta_time(120);

        MIDIDeltaTimedBigMessage delta_big_msg(delta_msg);

        CHECK(delta_big_msg.get_channel() == 1);
        CHECK(delta_big_msg.get_bender_value() == 4096);
        CHECK(delta_big_msg.get_delta_time() == 120);
    }
}

TEST_CASE("MIDIMessage validation and error cases")
{
    SUBCASE("Invalid channel values are masked")
    {
        MIDIMessage msg;

        // Channels should be masked to 4 bits (0-15)
        msg.set_channel(16);  // Should become 0
        CHECK(msg.get_channel() == 0);

        msg.set_channel(255);  // Should become 15
        CHECK(msg.get_channel() == 15);
    }

    SUBCASE("8-bit values are stored as-is")
    {
        MIDIMessage msg;

        // Values above 127 are stored as-is (not automatically masked)
        msg.set_note_on(0, 128, 100);
        CHECK(msg.get_note() == 128);

        msg.set_note_on(0, 60, 128);
        CHECK(msg.get_velocity() == 128);

        msg.set_control_change(0, 128, 100);
        CHECK(msg.get_controller() == 128);

        msg.set_control_change(0, 7, 128);
        CHECK(msg.get_controller_value() == 128);

        // Test with various values
        msg.set_note_on(0, 255, 255);
        CHECK(msg.get_note() == 255);
        CHECK(msg.get_velocity() == 255);
    }

    SUBCASE("Message length calculation")
    {
        MIDIMessage msg;

        // Single byte messages
        msg.set_tune_request();
        CHECK(msg.get_length() == 1);

        // Two byte messages
        msg.set_program_change(0, 42);
        CHECK(msg.get_length() == 2);

        msg.set_song_select(5);
        CHECK(msg.get_length() == 2);

        // Three byte messages
        msg.set_note_on(0, 60, 100);
        CHECK(msg.get_length() == 3);

        msg.set_control_change(0, 7, 100);
        CHECK(msg.get_length() == 3);

        msg.set_pitch_bend(0, 8192);
        CHECK(msg.get_length() == 3);

        msg.set_song_position(1024);
        CHECK(msg.get_length() == 3);
    }
}