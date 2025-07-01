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

#ifndef JDKSMIDI_MULTITRACK_H
#define JDKSMIDI_MULTITRACK_H

#include "jdksmidi/track.h"

namespace jdksmidi {

class MIDIMultiTrack;
class MIDIMultiTrackIteratorState;
class MIDIMultiTrackIterator;

class MIDIMultiTrack
{
  public:
    MIDIMultiTrack(int max_num_tracks_ = 64, bool deletable_ = true);
    virtual ~MIDIMultiTrack();

    void SetTrack(int trk, MIDITrack* t);
    MIDITrack* GetTrack(int trk);
    MIDITrack const* GetTrack(int trk) const;
    int GetNumTracks() const { return num_tracks; }

    void Clear();

    int GetClksPerBeat() const { return clks_per_beat; }

    void SetClksPerBeat(int c) { clks_per_beat = c; }

  protected:
    MIDITrack** tracks;
    int const num_tracks;
    bool deletable;

    int clks_per_beat;

  private:
};

class MIDIMultiTrackIteratorState
{
  public:
    MIDIMultiTrackIteratorState(int num_tracks_ = 64);
    MIDIMultiTrackIteratorState(MIDIMultiTrackIteratorState const& m);
    virtual ~MIDIMultiTrackIteratorState();

    MIDIMultiTrackIteratorState const& operator=(MIDIMultiTrackIteratorState const& m);

    int GetNumTracks() const { return num_tracks; }
    int GetCurEventTrack() const { return cur_event_track; }
    MIDIClockTime GetCurrentTime() const { return cur_time; }

    void Reset();
    int FindTrackOfFirstEvent();

    MIDIClockTime cur_time;
    int cur_event_track;
    int num_tracks;
    int* next_event_number;
    MIDIClockTime* next_event_time;
};

class MIDIMultiTrackIterator
{
  public:
    MIDIMultiTrackIterator(MIDIMultiTrack* mlt);
    virtual ~MIDIMultiTrackIterator();

    void GoToTime(MIDIClockTime time);

    bool GetCurEventTime(MIDIClockTime* t) const;
    bool GetCurEvent(int* track, MIDITimedBigMessage** msg) const;
    bool GoToNextEvent();

    bool GoToNextEventOnTrack(int track);

    MIDIMultiTrackIteratorState const& GetState() const { return state; }

    MIDIMultiTrackIteratorState& GetState() { return state; }

    void SetState(MIDIMultiTrackIteratorState const& s) { state = s; }

    MIDIMultiTrack* GetMultiTrack() { return multitrack; }
    MIDIMultiTrack const* GetMultiTrack() const { return multitrack; }

  protected:
    MIDIMultiTrack* multitrack;
    MIDIMultiTrackIteratorState state;
};

}  // namespace jdksmidi

#endif
