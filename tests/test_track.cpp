#include "doctest/doctest.h"
#include "jdksmidi/track.h"
#include "jdksmidi/msg.h"

using namespace jdksmidi;

TEST_CASE("MIDITrack basic functionality") {
    MIDITrack track;

    SUBCASE("Initial state") {
        CHECK(track.GetNumEvents() == 0);
        CHECK(track.GetBufferSize() == 0);
    }

    SUBCASE("Clear functionality") {
        MIDITimedBigMessage msg;
        msg.SetNoteOn(0, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 1);
        track.Clear();
        CHECK(track.GetNumEvents() == 0);
    }
}

TEST_CASE("MIDITrack event management") {
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("Single event insertion") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        bool result = track.PutEvent(msg);
        
        CHECK(result == true);
        CHECK(track.GetNumEvents() == 1);
    }

    SUBCASE("Multiple events in chronological order") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOff(1, 60, 64);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        msg.SetNoteOn(1, 64, 100);
        msg.SetTime(300);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 3);
    }

    SUBCASE("Multiple events out of chronological order") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(300);
        track.PutEvent(msg);
        
        msg.SetNoteOff(1, 64, 64);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOn(1, 64, 100);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 3);
    }

    SUBCASE("Events at same timestamp") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOn(2, 64, 100);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOn(3, 67, 80);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 3);
    }
}

TEST_CASE("MIDITrack event access") {
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("GetEvent functionality") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOff(1, 60, 64);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        msg.SetControlChange(1, 7, 100);
        msg.SetTime(300);
        track.PutEvent(msg);
        
        // Access first event
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event != nullptr);
        CHECK(event->GetTime() == 100);
        CHECK(event->IsNoteOn() == true);
        CHECK(event->GetNote() == 60);
        
        // Access second event
        event = track.GetEvent(1);
        CHECK(event != nullptr);
        CHECK(event->GetTime() == 200);
        CHECK(event->IsNoteOff() == true);
        CHECK(event->GetNote() == 60);
        
        // Access third event
        event = track.GetEvent(2);
        CHECK(event != nullptr);
        CHECK(event->GetTime() == 300);
        CHECK(event->IsControlChange() == true);
        CHECK(event->GetController() == 7);
        
        // Access out of bounds
        event = track.GetEvent(3);
        CHECK(event == nullptr);
    }

    SUBCASE("FindEventNumber functionality") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOff(1, 60, 64);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        msg.SetControlChange(1, 7, 100);
        msg.SetTime(300);
        track.PutEvent(msg);
        
        int event_num;
        bool found;
        
        // Find exact time
        found = track.FindEventNumber(200, &event_num);
        CHECK(found == true);
        CHECK(event_num == 1);
        
        // Find time between events
        found = track.FindEventNumber(150, &event_num);
        CHECK(found == true);
        CHECK(event_num == 1);
        
        // Find time before first event
        found = track.FindEventNumber(50, &event_num);
        CHECK(found == true);
        CHECK(event_num == 0);
        
        // Find time after last event
        found = track.FindEventNumber(400, &event_num);
        CHECK(found == false);
        CHECK(event_num == 3);
    }
}

TEST_CASE("MIDITrack event types") {
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("Note messages") {
        msg.SetNoteOn(5, 72, 120);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOff(5, 72, 64);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 2);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsNoteOn() == true);
        CHECK(event->GetChannel() == 5);
        CHECK(event->GetNote() == 72);
        CHECK(event->GetVelocity() == 120);
        
        event = track.GetEvent(1);
        CHECK(event->IsNoteOff() == true);
        CHECK(event->GetChannel() == 5);
        CHECK(event->GetNote() == 72);
        CHECK(event->GetVelocity() == 64);
    }

    SUBCASE("Control change messages") {
        msg.SetControlChange(3, C_MODULATION, 64);
        msg.SetTime(150);
        track.PutEvent(msg);
        
        msg.SetControlChange(3, C_VOLUME, 100);
        msg.SetTime(250);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 2);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsControlChange() == true);
        CHECK(event->GetChannel() == 3);
        CHECK(event->GetController() == C_MODULATION);
        CHECK(event->GetControllerValue() == 64);
        
        event = track.GetEvent(1);
        CHECK(event->IsControlChange() == true);
        CHECK(event->GetController() == C_VOLUME);
        CHECK(event->GetControllerValue() == 100);
    }

    SUBCASE("Program change messages") {
        msg.SetProgramChange(7, 42);
        msg.SetTime(300);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 1);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsProgramChange() == true);
        CHECK(event->GetChannel() == 7);
        CHECK(event->GetPGValue() == 42);
    }

    SUBCASE("Pitch bend messages") {
        msg.SetPitchBend(2, 1000);
        msg.SetTime(400);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 1);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsPitchBend() == true);
        CHECK(event->GetChannel() == 2);
        CHECK(event->GetBenderValue() == 1000);
    }

    SUBCASE("System messages") {
        msg.SetSongSelect(5);
        msg.SetTime(500);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 1);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsSongSelect() == true);
        CHECK(event->GetByte1() == 5);
    }
}

TEST_CASE("MIDITrack event modification") {
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("SetEvent functionality") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOff(1, 60, 64);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 2);
        
        // Modify first event
        MIDITimedBigMessage new_msg;
        new_msg.SetControlChange(2, C_VOLUME, 100);
        new_msg.SetTime(150);
        bool result = track.SetEvent(0, new_msg);
        
        CHECK(result == true);
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsControlChange() == true);
        CHECK(event->GetChannel() == 2);
        CHECK(event->GetTime() == 150);
        
        // Try to set out of bounds event
        result = track.SetEvent(5, new_msg);
        CHECK(result == false);
    }

    SUBCASE("MakeEventNoOp functionality") {
        msg.SetNoteOn(1, 60, 127);
        msg.SetTime(100);
        track.PutEvent(msg);
        
        msg.SetNoteOff(1, 60, 64);
        msg.SetTime(200);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 2);
        
        // Make first event NoOp
        bool result = track.MakeEventNoOp(0);
        CHECK(result == true);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->IsNoOp() == true);
        
        // Second event should be unchanged
        event = track.GetEvent(1);
        CHECK(event->IsNoteOff() == true);
        
        // Try to make out of bounds event NoOp
        result = track.MakeEventNoOp(5);
        CHECK(result == false);
    }
}

TEST_CASE("MIDITrack edge cases") {
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("Zero timestamp events") {
        msg.SetNoteOn(0, 60, 127);
        msg.SetTime(0);
        track.PutEvent(msg);
        
        msg.SetNoteOff(0, 60, 64);
        msg.SetTime(0);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 2);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event != nullptr);
        CHECK(event->GetTime() == 0);
    }

    SUBCASE("Large timestamp values") {
        msg.SetNoteOn(0, 60, 127);
        msg.SetTime(1000000);
        track.PutEvent(msg);
        
        CHECK(track.GetNumEvents() == 1);
        
        const MIDITimedBigMessage* event = track.GetEvent(0);
        CHECK(event->GetTime() == 1000000);
    }

    SUBCASE("All channels (0-15)") {
        for (int channel = 0; channel < 16; ++channel) {
            msg.SetNoteOn(channel, 60, 127);
            msg.SetTime(channel * 100);
            track.PutEvent(msg);
        }
        
        CHECK(track.GetNumEvents() == 16);
        
        for (int channel = 0; channel < 16; ++channel) {
            const MIDITimedBigMessage* event = track.GetEvent(channel);
            CHECK(event->GetChannel() == channel);
            CHECK(event->GetTime() == channel * 100);
        }
    }

    SUBCASE("All note range (0-127)") {
        for (int note = 0; note < 128; note += 8) {
            msg.SetNoteOn(0, note, 127);
            msg.SetTime(note * 10);
            track.PutEvent(msg);
        }
        
        CHECK(track.GetNumEvents() == 16);
        
        for (int i = 0; i < 16; ++i) {
            const MIDITimedBigMessage* event = track.GetEvent(i);
            CHECK(event->GetNote() == i * 8);
            CHECK(event->GetTime() == i * 8 * 10);
        }
    }

    SUBCASE("Track expansion") {
        // Add enough events to trigger expansion
        for (int i = 0; i < MIDITrackChunkSize + 10; ++i) {
            msg.SetNoteOn(0, 60, 127);
            msg.SetTime(i);
            bool result = track.PutEvent(msg);
            CHECK(result == true);
        }
        
        CHECK(track.GetNumEvents() == MIDITrackChunkSize + 10);
        CHECK(track.GetBufferSize() >= MIDITrackChunkSize + 10);
    }
}