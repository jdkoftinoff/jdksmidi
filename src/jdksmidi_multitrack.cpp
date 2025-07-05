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
/*
** Copyright 1986 to 1998 By Jeffrey Koftinoff
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by Jeffrey Koftinoff
**
*/

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/track.h"

#include <vector>

#ifndef DEBUG_MDMLTTRK
#    define DEBUG_MDMLTTRK 0
#endif

#if DEBUG_MDMLTTRK
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIMultiTrack::MIDIMultiTrack(int num_tracks_, bool deletable_)
    : _tracks(num_tracks_, nullptr)
    , _num_tracks(num_tracks_)
    , _deletable(deletable_)
    , _clks_per_beat(480)
{
    if (_deletable) {
        for (int i = 0; i < _num_tracks; ++i)
            _tracks[i] = new MIDITrack;
    }
}

MIDIMultiTrack::~MIDIMultiTrack()
{
    if (_deletable) {
        for (int i = 0; i < _num_tracks; ++i)
            delete _tracks[i];
    }
    // vector automatically cleans up
}

void MIDIMultiTrack::clear()
{
    for (int i = 0; i < _num_tracks; ++i) {
        _tracks[i]->clear();
    }
}

void MIDIMultiTrack::set_track(int trk, MIDITrack* t)
{
    _tracks[trk] = t;
}

MIDITrack* MIDIMultiTrack::get_track(int trk)
{
    return _tracks[trk];
}

MIDITrack const* MIDIMultiTrack::get_track(int trk) const
{
    return _tracks[trk];
}

MIDIMultiTrackIteratorState::MIDIMultiTrackIteratorState(int num_tracks_)
    : _cur_event_track(0)
    , _num_tracks(num_tracks_)
    , _next_event_number(num_tracks_)
    , _next_event_time(num_tracks_)
{
    reset();
}

MIDIMultiTrackIteratorState::MIDIMultiTrackIteratorState(MIDIMultiTrackIteratorState const& m)
    : _cur_time(m._cur_time)
    , _cur_event_track(m._cur_event_track)
    , _num_tracks(m._num_tracks)
    , _next_event_number(m._next_event_number)
    , _next_event_time(m._next_event_time)
{}

MIDIMultiTrackIteratorState::~MIDIMultiTrackIteratorState()
{
    // vectors automatically clean up
}

MIDIMultiTrackIteratorState const& MIDIMultiTrackIteratorState::operator=(
    MIDIMultiTrackIteratorState const& m)
{
    if (this != &m) {
        _num_tracks = m._num_tracks;
        _cur_time = m._cur_time;
        _cur_event_track = m._cur_event_track;
        _next_event_number = m._next_event_number;
        _next_event_time = m._next_event_time;
    }
    return *this;
}

void MIDIMultiTrackIteratorState::reset()
{
    _cur_time = 0;
    _cur_event_track = 0;

    for (int i = 0; i < _num_tracks; ++i) {
        _next_event_number[i] = 0;
        _next_event_time[i] = 0xffffffff;
    }
}

int MIDIMultiTrackIteratorState::find_track_of_first_event()
{
    MIDIClockTime minimum_time = 0xffffffff;
    int minimum_time_track = -1;
    // go through all tracks and find the track with the smallest
    // event time.

    for (int j = 0; j < _num_tracks; ++j) {
        int i = (j + _cur_event_track + 1) % _num_tracks;
        // skip any tracks that have a current event number less than 0 - these are
        // finished already

        if (_next_event_number[i] >= 0 && _next_event_time[i] < minimum_time) {
            minimum_time = _next_event_time[i];
            minimum_time_track = i;
        }
    }

    // set cur_event_track to -1 if there are no more events left
    _cur_event_track = minimum_time_track;
    _cur_time = minimum_time;
    return _cur_event_track;
}

MIDIMultiTrackIterator::MIDIMultiTrackIterator(MIDIMultiTrack* mlt)
    : multitrack(mlt)
    , state(mlt->get_num_tracks())

{}

MIDIMultiTrackIterator::~MIDIMultiTrackIterator()
{}

void MIDIMultiTrackIterator::go_to_time(MIDIClockTime time)
{
    // start at time 0
    state.reset();
    // transfer info from the first events in each track in the
    // multitrack object to our current state.

    for (int i = 0; i < multitrack->get_num_tracks(); ++i) {
        auto track = multitrack->get_track(i);
        // default: set the next_event_number for this track to -1
        // to signify end of track
        state._next_event_number[i] = -1;

        // are there any events in this track?
        if (track && track->get_num_events() > 0) {
            // yes, extract the time of the first event
            auto msg = track->get_event_address(0);

            if (msg) {
                // found the first message of the track. Keep track
                // of the event number and the event time.
                state._next_event_number[i] = 0;
                state._next_event_time[i] = msg->get_time();
            }
        }
    }

    // are there any events at all? find the track with the
    // earliest event

    if (state.find_track_of_first_event() != -1) {
        // yes
        // iterate through all the events until we find a time >= the requested time
        while (state.get_current_time() < time) {
            // did not get to the requested time yet.
            // go to the next chronological event on all tracks
            if (!go_to_next_event()) {
                // there is no more events to go to
                break;
            }
        }
    }
}

bool MIDIMultiTrackIterator::get_cur_event_time(MIDIClockTime* t) const
{
    // if there is a next event, then set *t to the time of the event and return true
    if (state.get_cur_event_track() != -1) {
        *t = state.get_current_time();
        return true;
    }

    else {
        return false;
    }
}

bool MIDIMultiTrackIterator::get_cur_event(int* track, MIDITimedBigMessage** msg) const
{
    int t = state.get_cur_event_track();

    if (t != -1) {
        if (track) {
            *track = t;
        }

        if (msg) {
            int num = state._next_event_number[t];

            if (num >= 0) {
                *msg = multitrack->get_track(t)->get_event_address(state._next_event_number[t]);
            }

            else {
                *msg = 0;
            }

            // do we really have a message?
            if (!*msg) {
                // no, return false then
                return false;
            }
        }

        return true;
    }

    else {
        return false;
    }
}

bool MIDIMultiTrackIterator::go_to_next_event()
{
    // find the next event in the multitrack list
    // and return it
    // if there is no event left, return false
    if (state._cur_event_track == -1) {
        // no tracks left - all tracks are at end
        return false;
    }

    // update the current event for the current track to the
    // next event on the same track.
    go_to_next_event_on_track(state._cur_event_track);
    // now find out which track now has the earliest event

    if (state.find_track_of_first_event() == -1) {
        // No tracks do. all tracks are at the end. return false.
        return false;
    }

    // ok, now state.cur_event_track has a valid track # of the next event
    return true;
}

bool MIDIMultiTrackIterator::go_to_next_event_on_track(int track_num)
{
    // Get the track that we are dealing with
    auto track = multitrack->get_track(track_num);
    // Get ptr to the current event number for this track
    int* event_num = &state._next_event_number[track_num];
    // skip this track if this event number is <0 - This track has hit end already.

    if (*event_num < 0) {
        return false;  // at end of track
    }

    // increment *event_num to next event on track
    (*event_num) += 1;

    // are we at end of track?
    if (*event_num >= track->get_num_events()) {
        // yes, set *event_num to -1
        *event_num = -1;
        return false;  // at end of track
    }

    else {
        // not at end of track yet - get the time of the event
        auto msg = track->get_event_address(*event_num);
        state._next_event_time[track_num] = msg->get_time();
    }

    return true;
}

}  // namespace jdksmidi
