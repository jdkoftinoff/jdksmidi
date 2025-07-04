#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"

using namespace jdksmidi;

TEST_CASE("Basic MIDI message creation")
{
    SUBCASE("Create Note On message")
    {
        MIDIMessage msg;
        msg.set_note_on(0, 60, 127);  // Channel 0, middle C, velocity 127

        CHECK(msg.get_status() == NOTE_ON);
        CHECK(msg.get_channel() == 0);
        CHECK(msg.get_note() == 60);
        CHECK(msg.get_velocity() == 127);
    }

    SUBCASE("Create Note Off message")
    {
        MIDIMessage msg;
        msg.set_note_off(0, 60, 64);  // Channel 0, middle C, velocity 64

        CHECK(msg.get_status() == NOTE_OFF);
        CHECK(msg.get_channel() == 0);
        CHECK(msg.get_note() == 60);
        CHECK(msg.get_velocity() == 64);
    }

    SUBCASE("Create Control Change message")
    {
        MIDIMessage msg;
        msg.set_control_change(0, 7, 100);  // Channel 0, volume control, value 100

        CHECK(msg.get_status() == CONTROL_CHANGE);
        CHECK(msg.get_channel() == 0);
        CHECK(msg.get_controller() == 7);
        CHECK(msg.get_controller_value() == 100);
    }
}

TEST_CASE("MIDI message validation")
{
    SUBCASE("Valid channel range")
    {
        MIDIMessage msg;
        for (int channel = 0; channel < 16; ++channel) {
            msg.set_note_on(channel, 60, 127);
            CHECK(msg.get_channel() == channel);
        }
    }

    SUBCASE("Valid note range")
    {
        MIDIMessage msg;
        for (int note = 0; note < 128; ++note) {
            msg.set_note_on(0, note, 127);
            CHECK(msg.get_note() == note);
        }
    }

    SUBCASE("Valid velocity range")
    {
        MIDIMessage msg;
        for (int velocity = 0; velocity < 128; ++velocity) {
            msg.set_note_on(0, 60, velocity);
            CHECK(msg.get_velocity() == velocity);
        }
    }
}