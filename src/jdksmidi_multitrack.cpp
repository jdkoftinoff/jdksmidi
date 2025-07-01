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

#include "jdksmidi/multitrack.h"
#include "jdksmidi/world.h"

#ifndef DEBUG_MDMLTTRK
#    define DEBUG_MDMLTTRK 0
#endif

#if DEBUG_MDMLTTRK
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIMultiTrack::MIDIMultiTrack(int num_tracks_, bool deletable_)
    : num_tracks(num_tracks_)
    , deletable(deletable_)
{
    ENTER("MIDIMultiTrack::MIDIMultiTrack()");
    tracks = new MIDITrack*[num_tracks];

    if (tracks) {
        if (deletable) {
            for (int i = 0; i < num_tracks; ++i)
                tracks[i] = new MIDITrack;
        }

        else {
            for (int i = 0; i < num_tracks; ++i)
                tracks[i] = 0;
        }
    }
}

MIDIMultiTrack::~MIDIMultiTrack()
{
    ENTER("MIDIMultiTrack::~MIDIMultiTrack()");

    if (deletable) {
        for (int i = 0; i < num_tracks; ++i)
            delete tracks[i];
    }

    delete[] tracks;
}

void MIDIMultiTrack::Clear()
{
    for (int i = 0; i < num_tracks; ++i) {
        tracks[i]->Clear();
    }
}

void MIDIMultiTrack::SetTrack(int trk, MIDITrack* t)
{
    tracks[trk] = t;
}

MIDITrack* MIDIMultiTrack::GetTrack(int trk)
{
    return tracks[trk];
}

MIDITrack const* MIDIMultiTrack::GetTrack(int trk) const
{
    return tracks[trk];
}

MIDIMultiTrackIteratorState::MIDIMultiTrackIteratorState(int num_tracks_)
{
    num_tracks = num_tracks_;
    cur_event_track = 0;
    next_event_number = new int[num_tracks];
    next_event_time = new MIDIClockTime[num_tracks];
    Reset();
}

MIDIMultiTrackIteratorState::MIDIMultiTrackIteratorState(MIDIMultiTrackIteratorState const& m)
{
    num_tracks = m.num_tracks;
    cur_event_track = m.cur_event_track;
    next_event_number = new int[num_tracks];
    next_event_time = new MIDIClockTime[num_tracks];
    cur_time = m.cur_time;

    for (int i = 0; i < num_tracks; ++i) {
        next_event_number[i] = m.next_event_number[i];
        next_event_time[i] = m.next_event_time[i];
    }
}

MIDIMultiTrackIteratorState::~MIDIMultiTrackIteratorState()
{
    delete[] next_event_number;
    delete[] next_event_time;
}

MIDIMultiTrackIteratorState const& MIDIMultiTrackIteratorState::operator=(
    MIDIMultiTrackIteratorState const& m)
{
    if (num_tracks != m.num_tracks) {
        delete[] next_event_number;
        delete[] next_event_time;
        num_tracks = m.num_tracks;
        next_event_number = new int[num_tracks];
        next_event_time = new MIDIClockTime[num_tracks];
    }

    cur_time = m.cur_time;
    cur_event_track = m.cur_event_track;

    for (int i = 0; i < num_tracks; ++i) {
        next_event_number[i] = m.next_event_number[i];
        next_event_time[i] = m.next_event_time[i];
    }

    return *this;
}

void MIDIMultiTrackIteratorState::Reset()
{
    cur_time = 0;
    cur_event_track = 0;

    for (int i = 0; i < num_tracks; ++i) {
        next_event_number[i] = 0;
        next_event_time[i] = 0xffffffff;
    }
}

int MIDIMultiTrackIteratorState::FindTrackOfFirstEvent()
{
    MIDIClockTime minimum_time = 0xffffffff;
    int minimum_time_track = -1;
    // go through all tracks and find the track with the smallest
    // event time.

    for (int j = 0; j < num_tracks; ++j) {
        int i = (j + cur_event_track + 1) % num_tracks;
        // skip any tracks that have a current event number less than 0 - these are
        // finished already

        if (next_event_number[i] >= 0 && next_event_time[i] < minimum_time) {
            minimum_time = next_event_time[i];
            minimum_time_track = i;
        }
    }

    // set cur_event_track to -1 if there are no more events left
    cur_event_track = minimum_time_track;
    cur_time = minimum_time;
    return cur_event_track;
}

MIDIMultiTrackIterator::MIDIMultiTrackIterator(MIDIMultiTrack* mlt)
    : multitrack(mlt)
    , state(mlt->GetNumTracks())

{}

MIDIMultiTrackIterator::~MIDIMultiTrackIterator()
{}

void MIDIMultiTrackIterator::GoToTime(MIDIClockTime time)
{
    // start at time 0
    state.Reset();
    // transfer info from the first events in each track in the
    // multitrack object to our current state.

    for (int i = 0; i < multitrack->GetNumTracks(); ++i) {
        MIDITrack* track = multitrack->GetTrack(i);
        // default: set the next_event_number for this track to -1
        // to signify end of track
        state.next_event_number[i] = -1;

        // are there any events in this track?
        if (track && track->GetNumEvents() > 0) {
            // yes, extract the time of the first event
            MIDITimedBigMessage* msg = track->GetEventAddress(0);

            if (msg) {
                // found the first message of the track. Keep track
                // of the event number and the event time.
                state.next_event_number[i] = 0;
                state.next_event_time[i] = msg->GetTime();
            }
        }
    }

    // are there any events at all? find the track with the
    // earliest event

    if (state.FindTrackOfFirstEvent() != -1) {
        // yes
        // iterate through all the events until we find a time >= the requested time
        while (state.GetCurrentTime() < time) {
            // did not get to the requested time yet.
            // go to the next chronological event on all tracks
            if (!GoToNextEvent()) {
                // there is no more events to go to
                break;
            }
        }
    }
}

bool MIDIMultiTrackIterator::GetCurEventTime(MIDIClockTime* t) const
{
    // if there is a next event, then set *t to the time of the event and return true
    if (state.GetCurEventTrack() != -1) {
        *t = state.GetCurrentTime();
        return true;
    }

    else {
        return false;
    }
}

bool MIDIMultiTrackIterator::GetCurEvent(int* track, MIDITimedBigMessage** msg) const
{
    int t = state.GetCurEventTrack();

    if (t != -1) {
        if (track) {
            *track = t;
        }

        if (msg) {
            int num = state.next_event_number[t];

            if (num >= 0) {
                *msg = multitrack->GetTrack(t)->GetEventAddress(state.next_event_number[t]);
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

bool MIDIMultiTrackIterator::GoToNextEvent()
{
    // find the next event in the multitrack list
    // and return it
    // if there is no event left, return false
    if (state.cur_event_track == -1) {
        // no tracks left - all tracks are at end
        return false;
    }

    // update the current event for the current track to the
    // next event on the same track.
    GoToNextEventOnTrack(state.cur_event_track);
    // now find out which track now has the earliest event

    if (state.FindTrackOfFirstEvent() == -1) {
        // No tracks do. all tracks are at the end. return false.
        return false;
    }

    // ok, now state.cur_event_track has a valid track # of the next event
    return true;
}

bool MIDIMultiTrackIterator::GoToNextEventOnTrack(int track_num)
{
    // Get the track that we are dealing with
    MIDITrack* track = multitrack->GetTrack(track_num);
    // Get ptr to the current event number for this track
    int* event_num = &state.next_event_number[track_num];
    // skip this track if this event number is <0 - This track has hit end already.

    if (*event_num < 0) {
        return false;  // at end of track
    }

    // increment *event_num to next event on track
    (*event_num) += 1;

    // are we at end of track?
    if (*event_num >= track->GetNumEvents()) {
        // yes, set *event_num to -1
        *event_num = -1;
        return false;  // at end of track
    }

    else {
        // not at end of track yet - get the time of the event
        MIDITimedBigMessage* msg;
        msg = track->GetEventAddress(*event_num);
        state.next_event_time[track_num] = msg->GetTime();
    }

    return true;
}

}  // namespace jdksmidi
