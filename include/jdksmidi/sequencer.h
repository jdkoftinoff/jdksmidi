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
#ifndef JDKSMIDI_SEQUENCER_H
#define JDKSMIDI_SEQUENCER_H

#include "jdksmidi/matrix.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/process.h"
#include "jdksmidi/tempo.h"
#include "jdksmidi/track.h"

#include <memory>

namespace jdksmidi {

class MIDISequencerGUIEvent;
class MIDISequencerGUIEventNotifier;
class MIDISequencerTrackState;
class MIDISequencer;

class MIDISequencerGUIEvent
{
  public:
    MIDISequencerGUIEvent() { bits = 0; }

    MIDISequencerGUIEvent(unsigned long bits_)
        : bits(bits_)
    {}

    MIDISequencerGUIEvent(MIDISequencerGUIEvent const& e)
        : bits(e.bits)
    {}

    MIDISequencerGUIEvent(int group, int subgroup, int item = 0)
    {
        bits = ((group & 0xff) << 24) | ((subgroup & 0xfff) << 12) | ((item & 0xfff) << 0);
    }

    operator unsigned long() const { return bits; }

    void set_event(int group, int subgroup = 0, int item = 0)
    {
        bits = ((group & 0xff) << 24) | ((subgroup & 0xfff) << 12) | ((item & 0xfff) << 0);
    }

    int GetEventGroup() const { return static_cast<int>((bits >> 24) & 0xff); }

    int GetEventSubGroup() const { return static_cast<int>((bits >> 12) & 0xfff); }

    int GetEventItem() const { return static_cast<int>((bits >> 0) & 0xfff); }

    // main groups
    enum
    {
        GROUP_ALL = 0,
        GROUP_CONDUCTOR,
        GROUP_TRANSPORT,
        GROUP_TRACK
    };

    // items in conductor group
    enum
    {
        GROUP_CONDUCTOR_ALL = 0,
        GROUP_CONDUCTOR_TEMPO,
        GROUP_CONDUCTOR_TIMESIG
    };

    // items in transport group
    enum
    {
        GROUP_TRANSPORT_ALL = 0,
        GROUP_TRANSPORT_MODE,
        GROUP_TRANSPORT_MEASURE,
        GROUP_TRANSPORT_BEAT,
        GROUP_TRANSPORT_ENDOFSONG
    };

    // items in TRACK group
    enum
    {
        GROUP_TRACK_ALL = 0,
        GROUP_TRACK_NAME,
        GROUP_TRACK_PG,
        GROUP_TRACK_NOTE,
        GROUP_TRACK_VOLUME
    };

  private:
    unsigned long bits;
};

class MIDISequencerGUIEventNotifier
{
  public:
    MIDISequencerGUIEventNotifier();

    virtual ~MIDISequencerGUIEventNotifier();

    virtual void Notify(MIDISequencer const* seq, MIDISequencerGUIEvent e) = 0;
    virtual bool GetEnable() const = 0;
    virtual void SetEnable(bool f) = 0;
};

class MIDISequencerGUIEventNotifierText : public MIDISequencerGUIEventNotifier
{
  public:
    MIDISequencerGUIEventNotifierText(FILE* f);

    virtual ~MIDISequencerGUIEventNotifierText();

    virtual void Notify(MIDISequencer const* seq, MIDISequencerGUIEvent e);
    virtual bool GetEnable() const;
    virtual void SetEnable(bool f);

  private:
    FILE* f;
    bool en;
};

class MIDISequencerTrackNotifier : public MIDIProcessor
{
  public:
    MIDISequencerTrackNotifier(MIDISequencer* seq_, int trk, MIDISequencerGUIEventNotifier* n);

    virtual ~MIDISequencerTrackNotifier();

    void set_notifier(MIDISequencer* seq_, int trk, MIDISequencerGUIEventNotifier* n)
    {
        seq = seq_;
        track_num = trk;
        notifier = n;
    }

    void Notify(int item);
    void notify_conductor(int item);

  private:
    MIDISequencer* seq;
    int track_num;
    MIDISequencerGUIEventNotifier* notifier;
};

class MIDISequencerTrackProcessor : public MIDIProcessor
{
  public:
    MIDISequencerTrackProcessor();
    virtual ~MIDISequencerTrackProcessor();

    virtual void reset();
    virtual bool process(MIDITimedBigMessage* msg);

    bool mute;           // track is muted
    bool solo;           // track is solod
    int velocity_scale;  // current velocity scale value for note ons, 100=normal
    int rechannel;       // rechannelization value, or -1 for none
    int transpose;       // amount to transpose note values

    MIDIProcessor* extra_proc;  // extra midi processing for this track
};

class MIDISequencerTrackState : public MIDISequencerTrackNotifier
{
  public:
    MIDISequencerTrackState(MIDISequencer* seq_, int trk, MIDISequencerGUIEventNotifier* n);
    virtual ~MIDISequencerTrackState();

    virtual void go_to_zero();
    virtual void reset();
    virtual bool process(MIDITimedBigMessage* msg);

    float tempobpm;            // current tempo in beats per minute
    int pg;                    // current program change, or -1
    int volume;                // current volume controller value
    int timesig_numerator;     // numerator of current time signature
    int timesig_denominator;   // denominator of current time signature
    int bender_value;          // last seen bender value
    char track_name[256];      // track name
    bool got_good_track_name;  // true if we dont want to use generic text events for track name

    bool notes_are_on;       // true if there are any notes currently on
    MIDIMatrix note_matrix;  // to keep track of all notes on
};

class MIDISequencerState
{
  public:
    MIDISequencerState(
        MIDISequencer* s, MIDIMultiTrack* multitrack_, MIDISequencerGUIEventNotifier* n);

    MIDISequencerState(MIDISequencerState const& s);
    ~MIDISequencerState();

    MIDISequencerState const& operator=(MIDISequencerState const& s);

    MIDISequencerGUIEventNotifier* notifier;
    MIDIMultiTrack* multitrack;
    int num_tracks;

    std::unique_ptr<MIDISequencerTrackState> track_state[64];
    MIDIMultiTrackIterator iterator;
    MIDIClockTime cur_clock;
    float cur_time_ms;
    int cur_beat;
    int cur_measure;
    MIDIClockTime next_beat_time;
};

class MIDISequencer
{
  public:
    MIDISequencer(MIDIMultiTrack* m, MIDISequencerGUIEventNotifier* n = 0);

    virtual ~MIDISequencer();

    void reset_track(int trk);
    void reset_all_tracks();

    MIDIClockTime get_current_midi_clock_time() const;
    double get_current_time_in_ms() const;
    int get_current_beat() const;
    int get_current_measure() const;

    double get_current_tempo_scale() const;
    double get_current_tempo() const;

    MIDISequencerState* get_state();
    MIDISequencerState const* get_state() const;

    void set_state(MIDISequencerState*);

    MIDISequencerTrackState* get_track_state(int trk);
    MIDISequencerTrackState const* get_track_state(int trk) const;

    MIDISequencerTrackProcessor* get_track_processor(int trk);
    MIDISequencerTrackProcessor const* get_track_processor(int trk) const;

    int get_num_tracks() const { return state.num_tracks; }

    bool get_solo_mode() const;

    void set_current_tempo_scale(float scale);
    void set_solo_mode(bool m, int trk = -1);

    void go_to_zero();
    bool go_to_time(MIDIClockTime time_clk);
    bool go_to_time_ms(float time_ms);
    bool go_to_measure(int measure, int beat = 0);

    bool get_next_event_time_ms(float* t);
    bool get_next_event_time(MIDIClockTime* t);
    bool get_next_event(int* tracknum, MIDITimedBigMessage* msg);

    void scan_events_at_this_time();

  protected:
    MIDITimedBigMessage beat_marker_msg;

    bool solo_mode;
    int tempo_scale;

    int num_tracks;
    std::unique_ptr<MIDISequencerTrackProcessor> track_processors[64];

    MIDISequencerState state;
};
}  // namespace jdksmidi

#endif
