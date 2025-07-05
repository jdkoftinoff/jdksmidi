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

#ifndef JDKSMIDI_MULTITRACK_H
#define JDKSMIDI_MULTITRACK_H

#include "jdksmidi/track.h"

#include <memory>
#include <vector>

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
    [[nodiscard]] MIDITrack* get_track(int trk);
    [[nodiscard]] MIDITrack const* get_track(int trk) const;
    [[nodiscard]] int get_num_tracks() const { return _num_tracks; }

    void clear();

    [[nodiscard]] int get_clks_per_beat() const { return _clks_per_beat; }

    void set_clks_per_beat(int c) { _clks_per_beat = c; }

  protected:
    std::vector<MIDITrack*> _tracks;
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

    [[nodiscard]] int get_num_tracks() const { return _num_tracks; }
    [[nodiscard]] int get_cur_event_track() const { return _cur_event_track; }
    [[nodiscard]] MIDIClockTime get_current_time() const { return _cur_time; }

    void reset();
    [[nodiscard]] int find_track_of_first_event();

  protected:
    friend class MIDIMultiTrackIterator;

    MIDIClockTime _cur_time;
    int _cur_event_track;
    int _num_tracks;
    std::vector<int> _next_event_number;
    std::vector<MIDIClockTime> _next_event_time;
};

class MIDIMultiTrackIterator
{
  public:
    MIDIMultiTrackIterator(MIDIMultiTrack* mlt);
    virtual ~MIDIMultiTrackIterator();

    void go_to_time(MIDIClockTime time);

    [[nodiscard]] bool get_cur_event_time(MIDIClockTime* t) const;
    [[nodiscard]] bool get_cur_event(int* track, MIDITimedBigMessage** msg) const;
    [[nodiscard]] bool go_to_next_event();

    [[nodiscard]] bool go_to_next_event_on_track(int track);

    [[nodiscard]] MIDIMultiTrackIteratorState const& get_state() const { return state; }

    [[nodiscard]] MIDIMultiTrackIteratorState& get_state() { return state; }

    void set_state(MIDIMultiTrackIteratorState const& s) { state = s; }

    [[nodiscard]] MIDIMultiTrack* get_multi_track() { return multitrack; }
    [[nodiscard]] MIDIMultiTrack const* get_multi_track() const { return multitrack; }

  protected:
    MIDIMultiTrack* multitrack;
    MIDIMultiTrackIteratorState state;
};

}  // namespace jdksmidi

#endif
