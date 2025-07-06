#include "doctest/doctest.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/track.h"

using namespace jdksmidi;

TEST_CASE("MIDITrack basic functionality")
{
    MIDITrack track;

    SUBCASE("Initial state")
    {
        CHECK(track.get_num_events() == 0);
        CHECK(track.get_buffer_size() == 0);
    }

    SUBCASE("Clear functionality")
    {
        MIDITimedBigMessage msg;
        msg.set_note_on(0, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 1);
        track.clear();
        CHECK(track.get_num_events() == 0);
    }
}

TEST_CASE("MIDITrack event management")
{
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("Single event insertion")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        bool result = track.put_event(msg);

        CHECK(result == true);
        CHECK(track.get_num_events() == 1);
    }

    SUBCASE("Multiple events in chronological order")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_off(1, 60, 64);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        msg.set_note_on(1, 64, 100);
        msg.set_time(300);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 3);
    }

    SUBCASE("Multiple events out of chronological order")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(300);
        CHECK(track.put_event(msg));

        msg.set_note_off(1, 64, 64);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_on(1, 64, 100);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 3);
    }

    SUBCASE("Events at same timestamp")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_on(2, 64, 100);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_on(3, 67, 80);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 3);
    }
}

TEST_CASE("MIDITrack event access")
{
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("GetEvent functionality")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_off(1, 60, 64);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        msg.set_control_change(1, 7, 100);
        msg.set_time(300);
        CHECK(track.put_event(msg));

        // Access first event
        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event != nullptr);
        CHECK(event->get_time() == 100);
        CHECK(event->is_note_on() == true);
        CHECK(event->get_note() == 60);

        // Access second event
        event = track.get_event(1);
        CHECK(event != nullptr);
        CHECK(event->get_time() == 200);
        CHECK(event->is_note_off() == true);
        CHECK(event->get_note() == 60);

        // Access third event
        event = track.get_event(2);
        CHECK(event != nullptr);
        CHECK(event->get_time() == 300);
        CHECK(event->is_control_change() == true);
        CHECK(event->get_controller() == 7);

        // Access out of bounds
        event = track.get_event(3);
        CHECK(event == nullptr);
    }

    SUBCASE("FindEventNumber functionality")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_off(1, 60, 64);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        msg.set_control_change(1, 7, 100);
        msg.set_time(300);
        CHECK(track.put_event(msg));

        int event_num;
        bool found;

        // Find exact time
        found = track.find_event_number(200, &event_num);
        CHECK(found == true);
        CHECK(event_num == 1);

        // Find time between events
        found = track.find_event_number(150, &event_num);
        CHECK(found == true);
        CHECK(event_num == 1);

        // Find time before first event
        found = track.find_event_number(50, &event_num);
        CHECK(found == true);
        CHECK(event_num == 0);

        // Find time after last event
        found = track.find_event_number(400, &event_num);
        CHECK(found == false);
        CHECK(event_num == 3);
    }
}

TEST_CASE("MIDITrack event types")
{
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("Note messages")
    {
        msg.set_note_on(5, 72, 120);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_off(5, 72, 64);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 2);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_note_on() == true);
        CHECK(event->get_channel() == 5);
        CHECK(event->get_note() == 72);
        CHECK(event->get_velocity() == 120);

        event = track.get_event(1);
        CHECK(event->is_note_off() == true);
        CHECK(event->get_channel() == 5);
        CHECK(event->get_note() == 72);
        CHECK(event->get_velocity() == 64);
    }

    SUBCASE("Control change messages")
    {
        msg.set_control_change(3, C_MODULATION, 64);
        msg.set_time(150);
        CHECK(track.put_event(msg));

        msg.set_control_change(3, C_MAIN_VOLUME, 100);
        msg.set_time(250);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 2);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_control_change() == true);
        CHECK(event->get_channel() == 3);
        CHECK(event->get_controller() == C_MODULATION);
        CHECK(event->get_controller_value() == 64);

        event = track.get_event(1);
        CHECK(event->is_control_change() == true);
        CHECK(event->get_controller() == C_MAIN_VOLUME);
        CHECK(event->get_controller_value() == 100);
    }

    SUBCASE("Program change messages")
    {
        msg.set_program_change(7, 42);
        msg.set_time(300);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 1);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_program_change() == true);
        CHECK(event->get_channel() == 7);
        CHECK(event->get_pg_value() == 42);
    }

    SUBCASE("Pitch bend messages")
    {
        msg.set_pitch_bend(2, 1000);
        msg.set_time(400);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 1);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_pitch_bend() == true);
        CHECK(event->get_channel() == 2);
        CHECK(event->get_bender_value() == 1000);
    }

    SUBCASE("System messages")
    {
        msg.set_song_select(5);
        msg.set_time(500);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 1);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_song_select() == true);
        CHECK(event->get_byte1() == 5);
    }
}

TEST_CASE("MIDITrack event modification")
{
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("SetEvent functionality")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_off(1, 60, 64);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 2);

        // Modify first event
        MIDITimedBigMessage new_msg;
        new_msg.set_control_change(2, C_MAIN_VOLUME, 100);
        new_msg.set_time(150);
        bool result = track.set_event(0, new_msg);

        CHECK(result == true);
        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_control_change() == true);
        CHECK(event->get_channel() == 2);
        CHECK(event->get_time() == 150);

        // Try to set out of bounds event
        result = track.set_event(5, new_msg);
        CHECK(result == false);
    }

    SUBCASE("MakeEventNoOp functionality")
    {
        msg.set_note_on(1, 60, 127);
        msg.set_time(100);
        CHECK(track.put_event(msg));

        msg.set_note_off(1, 60, 64);
        msg.set_time(200);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 2);

        // Make first event NoOp
        bool result = track.make_event_no_op(0);
        CHECK(result == true);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->is_no_op() == true);

        // Second event should be unchanged
        event = track.get_event(1);
        CHECK(event->is_note_off() == true);

        // Try to make out of bounds event NoOp
        result = track.make_event_no_op(5);
        CHECK(result == false);
    }
}

TEST_CASE("MIDITrack edge cases")
{
    MIDITrack track;
    MIDITimedBigMessage msg;

    SUBCASE("Zero timestamp events")
    {
        msg.set_note_on(0, 60, 127);
        msg.set_time(0);
        CHECK(track.put_event(msg));

        msg.set_note_off(0, 60, 64);
        msg.set_time(0);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 2);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event != nullptr);
        CHECK(event->get_time() == 0);
    }

    SUBCASE("Large timestamp values")
    {
        msg.set_note_on(0, 60, 127);
        msg.set_time(1000000);
        CHECK(track.put_event(msg));

        CHECK(track.get_num_events() == 1);

        MIDITimedBigMessage const* event = track.get_event(0);
        CHECK(event->get_time() == 1000000);
    }

    SUBCASE("All channels (0-15)")
    {
        for (int channel = 0; channel < 16; ++channel) {
            msg.set_note_on(channel, 60, 127);
            msg.set_time(channel * 100);
            CHECK(track.put_event(msg));
        }

        CHECK(track.get_num_events() == 16);

        for (int channel = 0; channel < 16; ++channel) {
            MIDITimedBigMessage const* event = track.get_event(channel);
            CHECK(event->get_channel() == channel);
            CHECK(event->get_time() == channel * 100);
        }
    }

    SUBCASE("All note range (0-127)")
    {
        for (int note = 0; note < 128; note += 8) {
            msg.set_note_on(0, note, 127);
            msg.set_time(note * 10);
            CHECK(track.put_event(msg));
        }

        CHECK(track.get_num_events() == 16);

        for (int i = 0; i < 16; ++i) {
            MIDITimedBigMessage const* event = track.get_event(i);
            CHECK(event->get_note() == i * 8);
            CHECK(event->get_time() == i * 8 * 10);
        }
    }

    SUBCASE("Track expansion")
    {
        // Add enough events to trigger expansion
        for (int i = 0; i < MIDITrackChunkSize + 10; ++i) {
            msg.set_note_on(0, 60, 127);
            msg.set_time(i);
            bool result = track.put_event(msg);
            CHECK(result == true);
        }

        CHECK(track.get_num_events() == MIDITrackChunkSize + 10);
        CHECK(track.get_buffer_size() >= MIDITrackChunkSize + 10);
    }
}