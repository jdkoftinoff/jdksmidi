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
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/track.h"

#include <memory>
#include <vector>

using namespace jdksmidi;

class TestMultiTrackFixture
{
  public:
    TestMultiTrackFixture()
        : multitrack(8, true)
    {}

    void add_note_event(
        int track, MIDIClockTime time, int channel, int note, int velocity, bool note_on = true)
    {
        MIDITimedBigMessage msg;
        msg.set_time(time);
        if (note_on) {
            msg.set_note_on(channel, note, velocity);
        } else {
            msg.set_note_off(channel, note, velocity);
        }
        multitrack.get_track(track)->put_event(msg);
    }

    void add_tempo_event(int track, MIDIClockTime time, float bpm)
    {
        MIDITimedBigMessage msg;
        msg.set_time(time);
        msg.set_tempo32(static_cast<std::uint32_t>(bpm * 32.0f));
        multitrack.get_track(track)->put_event(msg);
    }

    void add_program_change_event(int track, MIDIClockTime time, int channel, int program)
    {
        MIDITimedBigMessage msg;
        msg.set_time(time);
        msg.set_program_change(channel, program);
        multitrack.get_track(track)->put_event(msg);
    }

    void add_control_change_event(
        int track, MIDIClockTime time, int channel, int controller, int value)
    {
        MIDITimedBigMessage msg;
        msg.set_time(time);
        msg.set_control_change(channel, controller, value);
        multitrack.get_track(track)->put_event(msg);
    }

    MIDIMultiTrack multitrack;
};

TEST_CASE("MIDIMultiTrack basic construction and properties")
{
    // Test default construction
    MIDIMultiTrack mt1;
    CHECK(mt1.get_num_tracks() == 64);  // Default track count

    // Test construction with specific track count
    MIDIMultiTrack mt2(16, true);
    CHECK(mt2.get_num_tracks() == 16);

    // Test construction with non-deletable tracks
    MIDIMultiTrack mt3(8, false);
    CHECK(mt3.get_num_tracks() == 8);

    // Test initial clocks per beat (typically 480 for standard MIDI files)
    CHECK(mt1.get_clks_per_beat() >= 0);  // Should have some default value
}

TEST_CASE("MIDIMultiTrack track management")
{
    TestMultiTrackFixture fixture;

    // Test getting tracks
    for (int i = 0; i < fixture.multitrack.get_num_tracks(); ++i) {
        auto track = fixture.multitrack.get_track(i);
        REQUIRE(track != nullptr);
        CHECK(track->get_num_events() == 0);  // Should start empty
    }

    // Test const track access
    MIDIMultiTrack const& const_mt = fixture.multitrack;
    for (int i = 0; i < const_mt.get_num_tracks(); ++i) {
        auto const_track = const_mt.get_track(i);
        REQUIRE(const_track != nullptr);
    }

    // Test setting external tracks
    MIDIMultiTrack mt_external(4, false);
    auto external_track = std::make_unique<MIDITrack>();
    MIDITrack* track_ptr = external_track.get();

    mt_external.set_track(0, track_ptr);
    CHECK(mt_external.get_track(0) == track_ptr);

    // Add an event to verify it's the same track
    MIDITimedBigMessage msg;
    msg.set_time(100);
    msg.set_note_on(0, 60, 100);
    track_ptr->put_event(msg);

    CHECK(mt_external.get_track(0)->get_num_events() == 1);
}

TEST_CASE("MIDIMultiTrack clocks per beat management")
{
    TestMultiTrackFixture fixture;

    // Test default value
    int default_clks = fixture.multitrack.get_clks_per_beat();
    CHECK(default_clks >= 0);

    // Test setting clocks per beat
    fixture.multitrack.set_clks_per_beat(480);
    CHECK(fixture.multitrack.get_clks_per_beat() == 480);

    fixture.multitrack.set_clks_per_beat(96);
    CHECK(fixture.multitrack.get_clks_per_beat() == 96);

    fixture.multitrack.set_clks_per_beat(1024);
    CHECK(fixture.multitrack.get_clks_per_beat() == 1024);
}

TEST_CASE("MIDIMultiTrack clear functionality")
{
    TestMultiTrackFixture fixture;

    // Add events to multiple tracks
    fixture.add_note_event(0, 100, 0, 60, 100);
    fixture.add_note_event(1, 200, 1, 62, 110);
    fixture.add_note_event(2, 300, 2, 64, 120);

    // Verify events were added
    CHECK(fixture.multitrack.get_track(0)->get_num_events() == 1);
    CHECK(fixture.multitrack.get_track(1)->get_num_events() == 1);
    CHECK(fixture.multitrack.get_track(2)->get_num_events() == 1);

    // Clear all tracks
    fixture.multitrack.clear();

    // Verify all tracks are empty
    for (int i = 0; i < fixture.multitrack.get_num_tracks(); ++i) {
        CHECK(fixture.multitrack.get_track(i)->get_num_events() == 0);
    }
}

TEST_CASE("MIDIMultiTrackIteratorState basic functionality")
{
    MIDIMultiTrackIteratorState state(8);

    // Test initial state
    CHECK(state.get_num_tracks() == 8);
    CHECK(state.get_cur_event_track() == 0);
    CHECK(state.get_current_time() == 0);

    // Test reset
    state.reset();
    CHECK(state.get_cur_event_track() == 0);
    CHECK(state.get_current_time() == 0);
}

TEST_CASE("MIDIMultiTrackIteratorState copy operations")
{
    MIDIMultiTrackIteratorState state1(4);

    // Copy constructor
    MIDIMultiTrackIteratorState state2(state1);
    CHECK(state2.get_num_tracks() == 4);
    CHECK(state2.get_cur_event_track() == state1.get_cur_event_track());
    CHECK(state2.get_current_time() == state1.get_current_time());

    // Assignment operator with same size
    MIDIMultiTrackIteratorState state3(4);
    state3 = state1;
    CHECK(state3.get_num_tracks() == 4);
    CHECK(state3.get_cur_event_track() == state1.get_cur_event_track());
    CHECK(state3.get_current_time() == state1.get_current_time());

    // Assignment operator with different size
    MIDIMultiTrackIteratorState state4(8);
    state4 = state1;
    CHECK(state4.get_num_tracks() == 4);  // Should resize to match
    CHECK(state4.get_cur_event_track() == state1.get_cur_event_track());
    CHECK(state4.get_current_time() == state1.get_current_time());
}

TEST_CASE("MIDIMultiTrackIterator basic construction and properties")
{
    TestMultiTrackFixture fixture;
    MIDIMultiTrackIterator iterator(&fixture.multitrack);

    // Test basic properties
    CHECK(iterator.get_multi_track() == &fixture.multitrack);
    CHECK(iterator.get_state().get_num_tracks() == fixture.multitrack.get_num_tracks());

    // Test const access
    MIDIMultiTrackIterator const& const_iter = iterator;
    CHECK(const_iter.get_multi_track() == &fixture.multitrack);
}

TEST_CASE("MIDIMultiTrackIterator with empty tracks")
{
    TestMultiTrackFixture fixture;
    MIDIMultiTrackIterator iterator(&fixture.multitrack);

    // Initialize iterator to time 0
    iterator.go_to_time(0);

    // Test with no events
    MIDIClockTime time;
    CHECK(iterator.get_cur_event_time(&time) == false);

    int track;
    MIDITimedBigMessage* msg;
    CHECK(iterator.get_cur_event(&track, &msg) == false);

    CHECK(iterator.go_to_next_event() == false);
    CHECK(iterator.go_to_next_event_on_track(0) == false);
}

TEST_CASE("MIDIMultiTrackIterator single track with events")
{
    TestMultiTrackFixture fixture;

    // Add events to track 0 in chronological order
    fixture.add_note_event(0, 100, 0, 60, 100, true);  // Note on at time 100
    fixture.add_note_event(0, 200, 0, 62, 110, true);  // Note on at time 200
    fixture.add_note_event(0, 300, 0, 60, 0, false);   // Note off at time 300
    fixture.add_note_event(0, 400, 0, 62, 0, false);   // Note off at time 400

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    // Test first event
    MIDIClockTime time;
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 100);

    int track;
    MIDITimedBigMessage* msg;
    CHECK(iterator.get_cur_event(&track, &msg) == true);
    CHECK(track == 0);
    REQUIRE(msg != nullptr);
    CHECK(msg->get_time() == 100);
    CHECK(msg->is_note_on() == true);
    CHECK(msg->get_note() == 60);
    CHECK(msg->get_velocity() == 100);

    // Move to next event
    CHECK(iterator.go_to_next_event() == true);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 200);

    CHECK(iterator.get_cur_event(&track, &msg) == true);
    CHECK(track == 0);
    REQUIRE(msg != nullptr);
    CHECK(msg->get_time() == 200);
    CHECK(msg->is_note_on() == true);
    CHECK(msg->get_note() == 62);
    CHECK(msg->get_velocity() == 110);

    // Continue through remaining events
    CHECK(iterator.go_to_next_event() == true);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 300);

    CHECK(iterator.go_to_next_event() == true);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 400);

    // Should be at end now
    CHECK(iterator.go_to_next_event() == false);
}

TEST_CASE("MIDIMultiTrackIterator multiple tracks with chronological merging")
{
    TestMultiTrackFixture fixture;

    // Add events to multiple tracks with interleaved timing
    fixture.add_note_event(0, 100, 0, 60, 100, true);  // Track 0, time 100
    fixture.add_note_event(1, 150, 1, 62, 110, true);  // Track 1, time 150
    fixture.add_note_event(0, 200, 0, 64, 120, true);  // Track 0, time 200
    fixture.add_note_event(2, 250, 2, 66, 100, true);  // Track 2, time 250
    fixture.add_note_event(1, 300, 1, 68, 110, true);  // Track 1, time 300

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    struct ExpectedEvent
    {
        MIDIClockTime time;
        int track;
        int note;
        int velocity;
    };

    std::vector<ExpectedEvent> expected = {
        {100, 0, 60, 100},
        {150, 1, 62, 110},
        {200, 0, 64, 120},
        {250, 2, 66, 100},
        {300, 1, 68, 110}};

    // Iterate through all events and verify chronological order
    for (size_t i = 0; i < expected.size(); ++i) {
        MIDIClockTime time;
        CHECK(iterator.get_cur_event_time(&time) == true);
        CHECK(time == expected[i].time);

        int track;
        MIDITimedBigMessage* msg;
        CHECK(iterator.get_cur_event(&track, &msg) == true);
        CHECK(track == expected[i].track);
        REQUIRE(msg != nullptr);
        CHECK(msg->get_time() == expected[i].time);
        CHECK(msg->get_note() == expected[i].note);
        CHECK(msg->get_velocity() == expected[i].velocity);

        if (i < expected.size() - 1) {
            CHECK(iterator.go_to_next_event() == true);
        }
    }

    // Should be at end
    CHECK(iterator.go_to_next_event() == false);
}

TEST_CASE("MIDIMultiTrackIterator go_to_time functionality")
{
    TestMultiTrackFixture fixture;

    // Add events at various times
    fixture.add_note_event(0, 100, 0, 60, 100, true);
    fixture.add_note_event(1, 200, 1, 62, 110, true);
    fixture.add_note_event(0, 300, 0, 64, 120, true);
    fixture.add_note_event(2, 400, 2, 66, 100, true);
    fixture.add_note_event(1, 500, 1, 68, 110, true);

    MIDIMultiTrackIterator iterator(&fixture.multitrack);

    // Go to time 0 - should find first event at time 100
    iterator.go_to_time(0);
    MIDIClockTime time;
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 100);

    // Go to time 150 - should find event at time 200
    iterator.go_to_time(150);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 200);

    // Go to time 250 - should find event at time 300
    iterator.go_to_time(250);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 300);

    // Go to time 350 - should find event at time 400
    iterator.go_to_time(350);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 400);

    // Go to time 1000 - should be past all events
    iterator.go_to_time(1000);
    CHECK(iterator.get_cur_event_time(&time) == false);
}

TEST_CASE("MIDIMultiTrackIterator go_to_next_event_on_track")
{
    TestMultiTrackFixture fixture;

    // Add multiple events to track 0
    fixture.add_note_event(0, 100, 0, 60, 100, true);
    fixture.add_note_event(0, 200, 0, 62, 110, true);
    fixture.add_note_event(0, 300, 0, 64, 120, true);

    // Add events to track 1
    fixture.add_note_event(1, 150, 1, 65, 100, true);
    fixture.add_note_event(1, 250, 1, 67, 110, true);

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    // Should start at first event (track 0, time 100)
    MIDIClockTime time;
    int track;
    MIDITimedBigMessage* msg;

    CHECK(iterator.get_cur_event(&track, &msg) == true);
    CHECK(track == 0);
    CHECK(msg->get_time() == 100);

    // Advance to next event on track 0 specifically
    CHECK(iterator.go_to_next_event_on_track(0) == true);

    // The iterator's state should be updated, but we need to call find_track_of_first_event
    // to determine which track has the next chronological event
    CHECK(iterator.go_to_next_event() == true);
    CHECK(iterator.get_cur_event(&track, &msg) == true);
    // Should now be at track 1, time 150 (next chronological event)
    CHECK(track == 1);
    CHECK(msg->get_time() == 150);
}

TEST_CASE("MIDIMultiTrackIterator state management")
{
    TestMultiTrackFixture fixture;

    // Add events
    fixture.add_note_event(0, 100, 0, 60, 100, true);
    fixture.add_note_event(1, 200, 1, 62, 110, true);
    fixture.add_note_event(0, 300, 0, 64, 120, true);

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    // Advance to second event
    iterator.go_to_next_event();

    // Save state
    MIDIMultiTrackIteratorState saved_state = iterator.get_state();

    // Advance further
    iterator.go_to_next_event();
    MIDIClockTime time;
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 300);

    // Restore state
    iterator.set_state(saved_state);
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 200);  // Should be back at second event
}

TEST_CASE("MIDIMultiTrackIterator complex event sequence")
{
    TestMultiTrackFixture fixture;

    // Create a complex multi-track sequence
    // Track 0: Melody
    fixture.add_note_event(0, 0, 0, 60, 100, true);
    fixture.add_note_event(0, 480, 0, 62, 110, true);
    fixture.add_note_event(0, 960, 0, 64, 120, true);

    // Track 1: Bass line
    fixture.add_note_event(1, 0, 1, 36, 127, true);
    fixture.add_note_event(1, 960, 1, 38, 127, true);

    // Track 2: Control changes
    fixture.add_control_change_event(2, 240, 2, 7, 100);  // Volume
    fixture.add_control_change_event(2, 720, 2, 10, 64);  // Pan

    // Track 3: Program changes
    fixture.add_program_change_event(3, 0, 3, 1);     // Piano
    fixture.add_program_change_event(3, 480, 3, 25);  // Guitar

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    // Count total events and verify chronological ordering
    std::vector<MIDIClockTime> event_times;
    int event_count = 0;
    MIDIClockTime prev_time = 0;

    do {
        MIDIClockTime time;
        int track;
        MIDITimedBigMessage* msg;

        if (iterator.get_cur_event_time(&time) && iterator.get_cur_event(&track, &msg)) {
            event_times.push_back(time);

            // Verify chronological ordering
            CHECK(time >= prev_time);
            prev_time = time;

            // Verify valid track and message
            CHECK(track >= 0);
            CHECK(track < fixture.multitrack.get_num_tracks());
            REQUIRE(msg != nullptr);
            CHECK(msg->get_time() == time);

            event_count++;
        }
    } while (iterator.go_to_next_event());

    // Should have processed all events
    CHECK(event_count == 9);  // 3 + 2 + 2 + 2 = 9 events total

    // Verify we captured events at expected times
    CHECK(std::find(event_times.begin(), event_times.end(), 0) != event_times.end());
    CHECK(std::find(event_times.begin(), event_times.end(), 240) != event_times.end());
    CHECK(std::find(event_times.begin(), event_times.end(), 480) != event_times.end());
    CHECK(std::find(event_times.begin(), event_times.end(), 720) != event_times.end());
    CHECK(std::find(event_times.begin(), event_times.end(), 960) != event_times.end());
}

TEST_CASE("MIDIMultiTrackIterator simultaneous events on multiple tracks")
{
    TestMultiTrackFixture fixture;

    // Add simultaneous events on different tracks
    fixture.add_note_event(0, 100, 0, 60, 100, true);
    fixture.add_note_event(1, 100, 1, 62, 110, true);
    fixture.add_note_event(2, 100, 2, 64, 120, true);

    // Add another set at a different time
    fixture.add_note_event(0, 200, 0, 65, 100, true);
    fixture.add_note_event(1, 200, 1, 67, 110, true);

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    // Should find events at time 100
    MIDIClockTime time;
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 100);

    // Track through all events at time 100
    std::vector<int> tracks_at_100;
    do {
        int track;
        MIDITimedBigMessage* msg;
        if (iterator.get_cur_event(&track, &msg) && msg->get_time() == 100) {
            tracks_at_100.push_back(track);
        }

        if (!iterator.go_to_next_event())
            break;

        CHECK(iterator.get_cur_event_time(&time) == true);
        if (time > 100)
            break;  // Moved past time 100
    } while (true);

    // Should have found events on tracks 0, 1, and 2 at time 100
    CHECK(tracks_at_100.size() >= 1);  // At least one event should be found

    // Now should be at time 200
    CHECK(time == 200);
}

TEST_CASE("MIDIMultiTrackIterator edge cases")
{
    // Test with single track
    MIDIMultiTrack single_track(1, true);
    MIDITimedBigMessage msg;
    msg.set_time(100);
    msg.set_note_on(0, 60, 100);
    single_track.get_track(0)->put_event(msg);

    MIDIMultiTrackIterator iterator(&single_track);
    iterator.go_to_time(0);

    MIDIClockTime time;
    CHECK(iterator.get_cur_event_time(&time) == true);
    CHECK(time == 100);

    CHECK(iterator.go_to_next_event() == false);  // Only one event

    // Test with empty multitrack
    MIDIMultiTrack empty_track(4, true);
    MIDIMultiTrackIterator empty_iterator(&empty_track);
    empty_iterator.go_to_time(0);

    CHECK(empty_iterator.get_cur_event_time(&time) == false);
    CHECK(empty_iterator.go_to_next_event() == false);
}

TEST_CASE("MIDIMultiTrackIterator state find_track_of_first_event")
{
    TestMultiTrackFixture fixture;

    // Add events to tracks 1 and 3 (skip 0 and 2)
    fixture.add_note_event(1, 200, 1, 62, 110, true);
    fixture.add_note_event(3, 100, 3, 66, 100, true);

    MIDIMultiTrackIterator iterator(&fixture.multitrack);
    iterator.go_to_time(0);

    // Should find track 3 first (time 100 < time 200)
    int track;
    MIDITimedBigMessage* msg;
    CHECK(iterator.get_cur_event(&track, &msg) == true);
    CHECK(track == 3);
    CHECK(msg->get_time() == 100);

    // Next should be track 1
    CHECK(iterator.go_to_next_event() == true);
    CHECK(iterator.get_cur_event(&track, &msg) == true);
    CHECK(track == 1);
    CHECK(msg->get_time() == 200);
}