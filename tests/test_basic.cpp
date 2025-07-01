#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"

using namespace jdksmidi;

TEST_CASE("Basic MIDI message creation") {
    SUBCASE("Create Note On message") {
        MIDIMessage msg;
        msg.SetNoteOn(0, 60, 127);  // Channel 0, middle C, velocity 127
        
        CHECK(msg.GetStatus() == NOTE_ON);
        CHECK(msg.GetChannel() == 0);
        CHECK(msg.GetNote() == 60);
        CHECK(msg.GetVelocity() == 127);
    }
    
    SUBCASE("Create Note Off message") {
        MIDIMessage msg;
        msg.SetNoteOff(0, 60, 64);  // Channel 0, middle C, velocity 64
        
        CHECK(msg.GetStatus() == NOTE_OFF);
        CHECK(msg.GetChannel() == 0);
        CHECK(msg.GetNote() == 60);
        CHECK(msg.GetVelocity() == 64);
    }
    
    SUBCASE("Create Control Change message") {
        MIDIMessage msg;
        msg.SetControlChange(0, 7, 100);  // Channel 0, volume control, value 100
        
        CHECK(msg.GetStatus() == CONTROL_CHANGE);
        CHECK(msg.GetChannel() == 0);
        CHECK(msg.GetController() == 7);
        CHECK(msg.GetControllerValue() == 100);
    }
}

TEST_CASE("MIDI message validation") {
    SUBCASE("Valid channel range") {
        MIDIMessage msg;
        for (int channel = 0; channel < 16; ++channel) {
            msg.SetNoteOn(channel, 60, 127);
            CHECK(msg.GetChannel() == channel);
        }
    }
    
    SUBCASE("Valid note range") {
        MIDIMessage msg;
        for (int note = 0; note < 128; ++note) {
            msg.SetNoteOn(0, note, 127);
            CHECK(msg.GetNote() == note);
        }
    }
    
    SUBCASE("Valid velocity range") {
        MIDIMessage msg;
        for (int velocity = 0; velocity < 128; ++velocity) {
            msg.SetNoteOn(0, 60, velocity);
            CHECK(msg.GetVelocity() == velocity);
        }
    }
}