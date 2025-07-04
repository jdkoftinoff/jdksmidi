#include "doctest/doctest.h"
#include "jdksmidi/matrix.h"
#include "jdksmidi/msg.h"

using namespace jdksmidi;

TEST_CASE("MIDIMatrix basic functionality")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Initial state")
    {
        CHECK(matrix.get_total_count() == 0);
        CHECK(matrix.get_channel_count(0) == 0);
        CHECK(matrix.get_channel_count(15) == 0);
        CHECK(matrix.get_note_count(0, 60) == 0);
        CHECK(matrix.get_hold_pedal(0) == false);
    }

    SUBCASE("Clear functionality")
    {
        msg.set_note_on(0, 60, 127);
        matrix.process(msg);
        CHECK(matrix.get_total_count() == 1);

        matrix.clear();
        CHECK(matrix.get_total_count() == 0);
        CHECK(matrix.get_channel_count(0) == 0);
        CHECK(matrix.get_note_count(0, 60) == 0);
    }
}

TEST_CASE("MIDIMatrix note on/off tracking")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Note on message")
    {
        msg.set_note_on(1, 60, 127);
        bool result = matrix.process(msg);

        CHECK(result == true);
        CHECK(matrix.get_total_count() == 1);
        CHECK(matrix.get_channel_count(1) == 1);
        CHECK(matrix.get_note_count(1, 60) == 1);
        CHECK(matrix.get_note_count(1, 61) == 0);
    }

    SUBCASE("Note off message")
    {
        msg.set_note_on(1, 60, 127);
        matrix.process(msg);

        msg.set_note_off(1, 60, 64);
        bool result = matrix.process(msg);

        CHECK(result == true);
        CHECK(matrix.get_total_count() == 0);
        CHECK(matrix.get_channel_count(1) == 0);
        CHECK(matrix.get_note_count(1, 60) == 0);
    }

    SUBCASE("Note on with velocity 0 (treated as note off)")
    {
        msg.set_note_on(1, 60, 127);
        matrix.process(msg);

        msg.set_note_on(1, 60, 0);
        bool result = matrix.process(msg);

        CHECK(result == true);
        CHECK(matrix.get_total_count() == 0);
        CHECK(matrix.get_channel_count(1) == 0);
        CHECK(matrix.get_note_count(1, 60) == 0);
    }

    SUBCASE("Multiple notes on same channel")
    {
        msg.set_note_on(2, 60, 127);
        matrix.process(msg);
        msg.set_note_on(2, 64, 100);
        matrix.process(msg);
        msg.set_note_on(2, 67, 80);
        matrix.process(msg);

        CHECK(matrix.get_total_count() == 3);
        CHECK(matrix.get_channel_count(2) == 3);
        CHECK(matrix.get_note_count(2, 60) == 1);
        CHECK(matrix.get_note_count(2, 64) == 1);
        CHECK(matrix.get_note_count(2, 67) == 1);
    }

    SUBCASE("Multiple notes on different channels")
    {
        msg.set_note_on(0, 60, 127);
        matrix.process(msg);
        msg.set_note_on(1, 64, 100);
        matrix.process(msg);
        msg.set_note_on(2, 67, 80);
        matrix.process(msg);

        CHECK(matrix.get_total_count() == 3);
        CHECK(matrix.get_channel_count(0) == 1);
        CHECK(matrix.get_channel_count(1) == 1);
        CHECK(matrix.get_channel_count(2) == 1);
        CHECK(matrix.get_note_count(0, 60) == 1);
        CHECK(matrix.get_note_count(1, 64) == 1);
        CHECK(matrix.get_note_count(2, 67) == 1);
    }

    SUBCASE("Duplicate note on same channel")
    {
        msg.set_note_on(3, 60, 127);
        matrix.process(msg);
        msg.set_note_on(3, 60, 100);
        matrix.process(msg);

        CHECK(matrix.get_total_count() == 2);
        CHECK(matrix.get_channel_count(3) == 2);
        CHECK(matrix.get_note_count(3, 60) == 2);
    }
}

TEST_CASE("MIDIMatrix sustain pedal handling")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Sustain pedal on")
    {
        msg.set_control_change(5, C_DAMPER, 127);
        bool result = matrix.process(msg);

        CHECK(result == false);
        CHECK(matrix.get_hold_pedal(5) == true);
    }

    SUBCASE("Sustain pedal off")
    {
        msg.set_control_change(5, C_DAMPER, 127);
        matrix.process(msg);

        msg.set_control_change(5, C_DAMPER, 0);
        bool result = matrix.process(msg);

        CHECK(result == false);
        CHECK(matrix.get_hold_pedal(5) == false);
    }

    SUBCASE("Sustain pedal threshold (0x40)")
    {
        msg.set_control_change(6, C_DAMPER, 0x3F);
        matrix.process(msg);
        CHECK(matrix.get_hold_pedal(6) == false);

        msg.set_control_change(6, C_DAMPER, 0x40);
        matrix.process(msg);
        CHECK(matrix.get_hold_pedal(6) == true);

        msg.set_control_change(6, C_DAMPER, 0x41);
        matrix.process(msg);
        CHECK(matrix.get_hold_pedal(6) == true);
    }
}

TEST_CASE("MIDIMatrix all notes off handling")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("All notes off on single channel")
    {
        msg.set_note_on(7, 60, 127);
        matrix.process(msg);
        msg.set_note_on(7, 64, 100);
        matrix.process(msg);
        msg.set_note_on(7, 67, 80);
        matrix.process(msg);

        CHECK(matrix.get_channel_count(7) == 3);

        msg.set_all_notes_off(7);
        bool result = matrix.process(msg);

        CHECK(result == true);
        CHECK(matrix.get_total_count() == 0);
        CHECK(matrix.get_channel_count(7) == 0);
        CHECK(matrix.get_note_count(7, 60) == 0);
        CHECK(matrix.get_note_count(7, 64) == 0);
        CHECK(matrix.get_note_count(7, 67) == 0);
    }

    SUBCASE("All notes off preserves other channels")
    {
        msg.set_note_on(8, 60, 127);
        matrix.process(msg);
        msg.set_note_on(9, 64, 100);
        matrix.process(msg);

        msg.set_all_notes_off(8);
        matrix.process(msg);

        CHECK(matrix.get_total_count() == 1);
        CHECK(matrix.get_channel_count(8) == 0);
        CHECK(matrix.get_channel_count(9) == 1);
        CHECK(matrix.get_note_count(9, 64) == 1);
    }
}

TEST_CASE("MIDIMatrix other message types")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Program change message (not processed)")
    {
        msg.set_program_change(10, 42);
        bool result = matrix.process(msg);

        CHECK(result == false);
        CHECK(matrix.get_total_count() == 0);
    }

    SUBCASE("Pitch bend message (not processed)")
    {
        msg.set_pitch_bend(11, 1000);
        bool result = matrix.process(msg);

        CHECK(result == false);
        CHECK(matrix.get_total_count() == 0);
    }

    SUBCASE("Other control change message (not processed)")
    {
        msg.set_control_change(12, C_MODULATION, 64);
        bool result = matrix.process(msg);

        CHECK(result == false);
        CHECK(matrix.get_total_count() == 0);
    }

    SUBCASE("System message (not processed)")
    {
        msg.set_song_select(5);
        bool result = matrix.process(msg);

        CHECK(result == false);
        CHECK(matrix.get_total_count() == 0);
    }
}

TEST_CASE("MIDIMatrix edge cases")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Note off without matching note on")
    {
        msg.set_note_off(13, 60, 64);
        bool result = matrix.process(msg);

        CHECK(result == true);
        CHECK(matrix.get_total_count() == 0);
        CHECK(matrix.get_channel_count(13) == 0);
        CHECK(matrix.get_note_count(13, 60) == 0);
    }

    SUBCASE("All channel range (0-15)")
    {
        for (int channel = 0; channel < 16; ++channel) {
            msg.set_note_on(channel, 60, 127);
            matrix.process(msg);
        }

        CHECK(matrix.get_total_count() == 16);

        for (int channel = 0; channel < 16; ++channel) {
            CHECK(matrix.get_channel_count(channel) == 1);
            CHECK(matrix.get_note_count(channel, 60) == 1);
        }
    }

    SUBCASE("All note range (0-127)")
    {
        for (int note = 0; note < 128; ++note) {
            msg.set_note_on(14, note, 127);
            matrix.process(msg);
        }

        CHECK(matrix.get_total_count() == 128);
        CHECK(matrix.get_channel_count(14) == 128);

        for (int note = 0; note < 128; ++note) {
            CHECK(matrix.get_note_count(14, note) == 1);
        }
    }
}