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
        CHECK(matrix.GetTotalCount() == 0);
        CHECK(matrix.GetChannelCount(0) == 0);
        CHECK(matrix.GetChannelCount(15) == 0);
        CHECK(matrix.GetNoteCount(0, 60) == 0);
        CHECK(matrix.GetHoldPedal(0) == false);
    }

    SUBCASE("Clear functionality")
    {
        msg.SetNoteOn(0, 60, 127);
        matrix.Process(msg);
        CHECK(matrix.GetTotalCount() == 1);

        matrix.Clear();
        CHECK(matrix.GetTotalCount() == 0);
        CHECK(matrix.GetChannelCount(0) == 0);
        CHECK(matrix.GetNoteCount(0, 60) == 0);
    }
}

TEST_CASE("MIDIMatrix note on/off tracking")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Note on message")
    {
        msg.SetNoteOn(1, 60, 127);
        bool result = matrix.Process(msg);

        CHECK(result == true);
        CHECK(matrix.GetTotalCount() == 1);
        CHECK(matrix.GetChannelCount(1) == 1);
        CHECK(matrix.GetNoteCount(1, 60) == 1);
        CHECK(matrix.GetNoteCount(1, 61) == 0);
    }

    SUBCASE("Note off message")
    {
        msg.SetNoteOn(1, 60, 127);
        matrix.Process(msg);

        msg.SetNoteOff(1, 60, 64);
        bool result = matrix.Process(msg);

        CHECK(result == true);
        CHECK(matrix.GetTotalCount() == 0);
        CHECK(matrix.GetChannelCount(1) == 0);
        CHECK(matrix.GetNoteCount(1, 60) == 0);
    }

    SUBCASE("Note on with velocity 0 (treated as note off)")
    {
        msg.SetNoteOn(1, 60, 127);
        matrix.Process(msg);

        msg.SetNoteOn(1, 60, 0);
        bool result = matrix.Process(msg);

        CHECK(result == true);
        CHECK(matrix.GetTotalCount() == 0);
        CHECK(matrix.GetChannelCount(1) == 0);
        CHECK(matrix.GetNoteCount(1, 60) == 0);
    }

    SUBCASE("Multiple notes on same channel")
    {
        msg.SetNoteOn(2, 60, 127);
        matrix.Process(msg);
        msg.SetNoteOn(2, 64, 100);
        matrix.Process(msg);
        msg.SetNoteOn(2, 67, 80);
        matrix.Process(msg);

        CHECK(matrix.GetTotalCount() == 3);
        CHECK(matrix.GetChannelCount(2) == 3);
        CHECK(matrix.GetNoteCount(2, 60) == 1);
        CHECK(matrix.GetNoteCount(2, 64) == 1);
        CHECK(matrix.GetNoteCount(2, 67) == 1);
    }

    SUBCASE("Multiple notes on different channels")
    {
        msg.SetNoteOn(0, 60, 127);
        matrix.Process(msg);
        msg.SetNoteOn(1, 64, 100);
        matrix.Process(msg);
        msg.SetNoteOn(2, 67, 80);
        matrix.Process(msg);

        CHECK(matrix.GetTotalCount() == 3);
        CHECK(matrix.GetChannelCount(0) == 1);
        CHECK(matrix.GetChannelCount(1) == 1);
        CHECK(matrix.GetChannelCount(2) == 1);
        CHECK(matrix.GetNoteCount(0, 60) == 1);
        CHECK(matrix.GetNoteCount(1, 64) == 1);
        CHECK(matrix.GetNoteCount(2, 67) == 1);
    }

    SUBCASE("Duplicate note on same channel")
    {
        msg.SetNoteOn(3, 60, 127);
        matrix.Process(msg);
        msg.SetNoteOn(3, 60, 100);
        matrix.Process(msg);

        CHECK(matrix.GetTotalCount() == 2);
        CHECK(matrix.GetChannelCount(3) == 2);
        CHECK(matrix.GetNoteCount(3, 60) == 2);
    }
}

TEST_CASE("MIDIMatrix sustain pedal handling")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Sustain pedal on")
    {
        msg.SetControlChange(5, C_DAMPER, 127);
        bool result = matrix.Process(msg);

        CHECK(result == false);
        CHECK(matrix.GetHoldPedal(5) == true);
    }

    SUBCASE("Sustain pedal off")
    {
        msg.SetControlChange(5, C_DAMPER, 127);
        matrix.Process(msg);

        msg.SetControlChange(5, C_DAMPER, 0);
        bool result = matrix.Process(msg);

        CHECK(result == false);
        CHECK(matrix.GetHoldPedal(5) == false);
    }

    SUBCASE("Sustain pedal threshold (0x40)")
    {
        msg.SetControlChange(6, C_DAMPER, 0x3F);
        matrix.Process(msg);
        CHECK(matrix.GetHoldPedal(6) == false);

        msg.SetControlChange(6, C_DAMPER, 0x40);
        matrix.Process(msg);
        CHECK(matrix.GetHoldPedal(6) == true);

        msg.SetControlChange(6, C_DAMPER, 0x41);
        matrix.Process(msg);
        CHECK(matrix.GetHoldPedal(6) == true);
    }
}

TEST_CASE("MIDIMatrix all notes off handling")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("All notes off on single channel")
    {
        msg.SetNoteOn(7, 60, 127);
        matrix.Process(msg);
        msg.SetNoteOn(7, 64, 100);
        matrix.Process(msg);
        msg.SetNoteOn(7, 67, 80);
        matrix.Process(msg);

        CHECK(matrix.GetChannelCount(7) == 3);

        msg.SetAllNotesOff(7);
        bool result = matrix.Process(msg);

        CHECK(result == true);
        CHECK(matrix.GetTotalCount() == 0);
        CHECK(matrix.GetChannelCount(7) == 0);
        CHECK(matrix.GetNoteCount(7, 60) == 0);
        CHECK(matrix.GetNoteCount(7, 64) == 0);
        CHECK(matrix.GetNoteCount(7, 67) == 0);
    }

    SUBCASE("All notes off preserves other channels")
    {
        msg.SetNoteOn(8, 60, 127);
        matrix.Process(msg);
        msg.SetNoteOn(9, 64, 100);
        matrix.Process(msg);

        msg.SetAllNotesOff(8);
        matrix.Process(msg);

        CHECK(matrix.GetTotalCount() == 1);
        CHECK(matrix.GetChannelCount(8) == 0);
        CHECK(matrix.GetChannelCount(9) == 1);
        CHECK(matrix.GetNoteCount(9, 64) == 1);
    }
}

TEST_CASE("MIDIMatrix other message types")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Program change message (not processed)")
    {
        msg.SetProgramChange(10, 42);
        bool result = matrix.Process(msg);

        CHECK(result == false);
        CHECK(matrix.GetTotalCount() == 0);
    }

    SUBCASE("Pitch bend message (not processed)")
    {
        msg.SetPitchBend(11, 1000);
        bool result = matrix.Process(msg);

        CHECK(result == false);
        CHECK(matrix.GetTotalCount() == 0);
    }

    SUBCASE("Other control change message (not processed)")
    {
        msg.SetControlChange(12, C_MODULATION, 64);
        bool result = matrix.Process(msg);

        CHECK(result == false);
        CHECK(matrix.GetTotalCount() == 0);
    }

    SUBCASE("System message (not processed)")
    {
        msg.SetSongSelect(5);
        bool result = matrix.Process(msg);

        CHECK(result == false);
        CHECK(matrix.GetTotalCount() == 0);
    }
}

TEST_CASE("MIDIMatrix edge cases")
{
    MIDIMatrix matrix;
    MIDIMessage msg;

    SUBCASE("Note off without matching note on")
    {
        msg.SetNoteOff(13, 60, 64);
        bool result = matrix.Process(msg);

        CHECK(result == true);
        CHECK(matrix.GetTotalCount() == 0);
        CHECK(matrix.GetChannelCount(13) == 0);
        CHECK(matrix.GetNoteCount(13, 60) == 0);
    }

    SUBCASE("All channel range (0-15)")
    {
        for (int channel = 0; channel < 16; ++channel) {
            msg.SetNoteOn(channel, 60, 127);
            matrix.Process(msg);
        }

        CHECK(matrix.GetTotalCount() == 16);

        for (int channel = 0; channel < 16; ++channel) {
            CHECK(matrix.GetChannelCount(channel) == 1);
            CHECK(matrix.GetNoteCount(channel, 60) == 1);
        }
    }

    SUBCASE("All note range (0-127)")
    {
        for (int note = 0; note < 128; ++note) {
            msg.SetNoteOn(14, note, 127);
            matrix.Process(msg);
        }

        CHECK(matrix.GetTotalCount() == 128);
        CHECK(matrix.GetChannelCount(14) == 128);

        for (int note = 0; note < 128; ++note) {
            CHECK(matrix.GetNoteCount(14, note) == 1);
        }
    }
}