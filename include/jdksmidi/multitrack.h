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

    void set_track(int trk, MIDITrack* t);
    MIDITrack* get_track(int trk);
    MIDITrack const* get_track(int trk) const;
    int get_num_tracks() const { return _num_tracks; }

    void clear();

    int get_clks_per_beat() const { return _clks_per_beat; }

    void set_clks_per_beat(int c) { _clks_per_beat = c; }

  protected:
    MIDITrack** _tracks;
    int const _num_tracks;
    bool _deletable;

    int _clks_per_beat;

  private:
};

class MIDIMultiTrackIteratorState
{
  public:
    MIDIMultiTrackIteratorState(int num_tracks_ = 64);
    MIDIMultiTrackIteratorState(MIDIMultiTrackIteratorState const& m);
    virtual ~MIDIMultiTrackIteratorState();

    MIDIMultiTrackIteratorState const& operator=(MIDIMultiTrackIteratorState const& m);

    int get_num_tracks() const { return _num_tracks; }
    int get_cur_event_track() const { return _cur_event_track; }
    MIDIClockTime get_current_time() const { return _cur_time; }

    void reset();
    int find_track_of_first_event();

  protected:
    friend class MIDIMultiTrackIterator;
    
    MIDIClockTime _cur_time;
    int _cur_event_track;
    int _num_tracks;
    int* _next_event_number;
    MIDIClockTime* _next_event_time;
};

class MIDIMultiTrackIterator
{
  public:
    MIDIMultiTrackIterator(MIDIMultiTrack* mlt);
    virtual ~MIDIMultiTrackIterator();

    void go_to_time(MIDIClockTime time);

    bool get_cur_event_time(MIDIClockTime* t) const;
    bool get_cur_event(int* track, MIDITimedBigMessage** msg) const;
    bool go_to_next_event();

    bool go_to_next_event_on_track(int track);

    MIDIMultiTrackIteratorState const& get_state() const { return state; }

    MIDIMultiTrackIteratorState& get_state() { return state; }

    void set_state(MIDIMultiTrackIteratorState const& s) { state = s; }

    MIDIMultiTrack* get_multi_track() { return multitrack; }
    MIDIMultiTrack const* get_multi_track() const { return multitrack; }

  protected:
    MIDIMultiTrack* multitrack;
    MIDIMultiTrackIteratorState state;
};

}  // namespace jdksmidi

#endif
