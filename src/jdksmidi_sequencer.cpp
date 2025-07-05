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
#include "jdksmidi/matrix.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/process.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/track.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

namespace jdksmidi {

static void FixQuotes(char* s_)
{
    std::uint8_t* s = (std::uint8_t*)s_;

    while (*s) {
        if (*s == 0xd2 || *s == 0xd3) {
            *s = '"';
        }

        else if (*s == 0xd5) {
            *s = '\'';
        }

        else if (*s >= 0x80) {
            *s = ' ';
        }

        s++;
    }
}

////////////////////////////////////////////////////////////////////////////

MIDISequencerGUIEventNotifier::MIDISequencerGUIEventNotifier() = default;

MIDISequencerGUIEventNotifier::~MIDISequencerGUIEventNotifier() = default;

////////////////////////////////////////////////////////////////////////////

MIDISequencerGUIEventNotifierText::MIDISequencerGUIEventNotifierText(FILE* f_)
    : f(f_)
    , en(true)
{}

MIDISequencerGUIEventNotifierText::~MIDISequencerGUIEventNotifierText() = default;

void MIDISequencerGUIEventNotifierText::notify(MIDISequencer const* seq, MIDISequencerGUIEvent e)
{
    if (en) {
        fprintf(
            f,
            "GUI EVENT: G=%d, SG=%d, ITEM=%d\n",
            e.get_event_group(),
            e.get_event_sub_group(),
            e.get_event_item());

        if (e.get_event_group() == MIDISequencerGUIEvent::GROUP_TRANSPORT) {
            if (e.get_event_item() == MIDISequencerGUIEvent::GROUP_TRANSPORT_BEAT) {
                fprintf(
                    f,
                    "MEAS %3d BEAT %3d\n",
                    seq->get_current_measure() + 1,
                    seq->get_current_beat() + 1);
            }
        }

        else if (e.get_event_group() == MIDISequencerGUIEvent::GROUP_CONDUCTOR) {
            if (e.get_event_item() == MIDISequencerGUIEvent::GROUP_CONDUCTOR_TIMESIG) {
                fprintf(
                    f,
                    "TIMESIG: %d/%d\n",
                    seq->get_track_state(0)->timesig_numerator,
                    seq->get_track_state(0)->timesig_denominator);
            }

            if (e.get_event_item() == MIDISequencerGUIEvent::GROUP_CONDUCTOR_TEMPO) {
                fprintf(f, "TEMPO: %3.2f\n", seq->get_track_state(0)->tempobpm);
            }
        }
    }
}

bool MIDISequencerGUIEventNotifierText::get_enable() const
{
    return en;
}

void MIDISequencerGUIEventNotifierText::set_enable(bool f)
{
    en = f;
}

/////////////////////////////////////////////////////////////////////////////

MIDISequencerTrackNotifier::MIDISequencerTrackNotifier(
    MIDISequencer* seq_, int trk, MIDISequencerGUIEventNotifier* n)
    : seq(seq_)
    , track_num(trk)
    , notifier(n)
{}

MIDISequencerTrackNotifier::~MIDISequencerTrackNotifier() = default;

void MIDISequencerTrackNotifier::notify(int item)
{
    if (notifier) {
        notifier->notify(
            seq, MIDISequencerGUIEvent(MIDISequencerGUIEvent::GROUP_TRACK, track_num, item));
    }
}

void MIDISequencerTrackNotifier::notify_conductor(int item)
{
    // only notify conductor if we are track #0
    if (notifier && track_num == 0) {
        notifier->notify(
            seq, MIDISequencerGUIEvent(MIDISequencerGUIEvent::GROUP_CONDUCTOR, 0, item));
    }
}

/////////////////

MIDISequencerTrackProcessor::MIDISequencerTrackProcessor()
    : mute(false)
    , solo(false)
    , velocity_scale(100)
    , rechannel(-1)
    , transpose(0)
    , extra_proc(nullptr)
{}

MIDISequencerTrackProcessor::~MIDISequencerTrackProcessor() = default;

void MIDISequencerTrackProcessor::reset()
{
    mute = false;
    solo = false;
    velocity_scale = 100;
    rechannel = -1;
    transpose = 0;
}

bool MIDISequencerTrackProcessor::process(MIDITimedBigMessage* msg)
{
    // are we muted?
    if (mute) {
        // yes, ignore event.
        return false;
    }

    // is the event a NoOp?

    if (msg->is_no_op()) {
        // yes, ignore event.
        return false;
    }

    // pass the event to our extra_proc if we have one

    if (extra_proc && extra_proc->process(msg) == false) {
        // extra_proc wanted to ignore this event
        return false;
    }

    // is it a normal MIDI channel message?
    if (msg->is_channel_msg()) {
        // yes, are we to re-channel it?
        if (rechannel != -1) {
            msg->set_channel(static_cast<std::uint8_t>(rechannel));
        }

        // is it a note on message?
        if (msg->is_note_on() && msg->get_velocity() > 0) {
            // yes, scale the velocity value as required
            int vel = static_cast<int>(msg->get_velocity());
            vel = vel * velocity_scale / 100;
            // make sure velocity is never less than 0

            if (vel < 0) {
                vel = 0;
            }

            // rewrite the velocity
            msg->set_velocity(static_cast<std::uint8_t>(vel));
        }

        // is it a type of event that needs to be transposed?

        if (msg->is_note_on() || msg->is_note_off() || msg->is_poly_pressure()) {
            int new_note = static_cast<int>(msg->get_note()) + transpose;

            if (new_note >= 0 && new_note <= 127) {
                // set new note number
                msg->set_note(static_cast<std::uint8_t>(new_note));
            }

            else {
                // otherwise delete this note - transposed value is out of range
                return false;
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////

MIDISequencerTrackState::MIDISequencerTrackState(
    MIDISequencer* seq_, int trk, MIDISequencerGUIEventNotifier* n)
    : MIDISequencerTrackNotifier(seq_, trk, n)
    , tempobpm(120.0)
    , volume(100)
    , timesig_numerator(4)
    , timesig_denominator(4)
    , bender_value(0)
    , got_good_track_name(false)
    , notes_are_on(false)
    , note_matrix()
{
    *track_name = '\0';
}

MIDISequencerTrackState::~MIDISequencerTrackState()
{}

void MIDISequencerTrackState::go_to_zero()
{
    tempobpm = 120.0;
    timesig_numerator = 4;
    timesig_denominator = 4;
    bender_value = 0;
    note_matrix.clear();
}

void MIDISequencerTrackState::reset()
{
    tempobpm = 120.0;
    volume = 100;
    notes_are_on = false;
    timesig_numerator = 4;
    timesig_denominator = 4;
    bender_value = 0;
    *track_name = '\0';
    note_matrix.clear();
    got_good_track_name = false;
}

bool MIDISequencerTrackState::process(MIDITimedBigMessage* msg)
{
    // is the event a NoOp?
    if (msg->is_no_op()) {
        // yes, ignore event.
        return false;
    }

    // is it a normal MIDI channel message?
    if (msg->is_channel_msg()) {
        if (msg->get_type() == PITCH_BEND)  // is it a bender event?
        {
            // yes
            // remember the bender wheel value
            bender_value = msg->get_bender_value();
        }

        else if (msg->is_control_change())  // is it a control change event?
        {
            // yes
            // is it a volume change event?
            if (msg->get_controller() == C_MAIN_VOLUME) {
                // yes, store the current volume level
                volume = msg->get_controller_value();
                notify(MIDISequencerGUIEvent::GROUP_TRACK_VOLUME);
            }
        }

        else if (msg->is_program_change())  // is it a program change event?
        {
            // yes
            // update the current program change value
            pg = msg->get_pg_value();
            notify(MIDISequencerGUIEvent::GROUP_TRACK_PG);
        }
    }

    else {
        // event is not a channel message. is it a meta-event?
        if (msg->is_meta_event()) {
            // yes, is it a tempo event
            if (msg->is_tempo()) {
                // yes get the current tempo
                tempobpm = static_cast<float>(msg->get_tempo32()) * (1.0f / 32.0f);

                if (tempobpm < 1) {
                    tempobpm = 120.0;
                }

                notify_conductor(MIDISequencerGUIEvent::GROUP_CONDUCTOR_TEMPO);
            }

            else  // is it a time signature event?
                if (msg->get_meta_type() == META_TIMESIG) {
                    // yes, extract the current numerator and denominator
                    timesig_numerator = msg->get_time_sig_numerator();
                    timesig_denominator = msg->get_time_sig_denominator();
                    notify_conductor(MIDISequencerGUIEvent::GROUP_CONDUCTOR_TIMESIG);
                }

                else  // is it a track name event?
                    if ((msg->get_meta_type() == META_TRACK_NAME ||
                         msg->get_meta_type() == META_INSTRUMENT_NAME ||
                         (!got_good_track_name && msg->get_meta_type() == META_GENERIC_TEXT &&
                          msg->get_time() == 0)) &&
                        msg->get_sys_ex()) {
                        got_good_track_name = true;
                        // yes, copy the track name
                        int len = msg->get_sys_ex()->get_length();

                        if (len > static_cast<int>(sizeof(track_name)) - 1)
                            len = static_cast<int>(sizeof(track_name)) - 1;

                        memcpy(track_name, msg->get_sys_ex()->get_buf(), len);
                        track_name[len] = '\0';
                        FixQuotes(track_name);
                        notify(MIDISequencerGUIEvent::GROUP_TRACK_NAME);
                    }
        }
    }

    // pass the message to our note matrix to keep track of all notes on
    // on this track

    if (note_matrix.process(*msg)) {
        // did the "any notes on" status change?
        if ((notes_are_on && note_matrix.get_total_count() == 0) ||
            (!notes_are_on && note_matrix.get_total_count() > 0)) {
            // yes, toggle our notes_are_on flag
            notes_are_on = !notes_are_on;
            // and notify the gui about the activity on this track
            notify(MIDISequencerGUIEvent::GROUP_TRACK_NOTE);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////

MIDISequencerState::MIDISequencerState(
    MIDISequencer* s, MIDIMultiTrack* m, MIDISequencerGUIEventNotifier* n)
    : notifier(n)
    , multitrack(m)
    , num_tracks(m->get_num_tracks())
    , iterator(m)
    , cur_clock(0)
    , cur_time_ms(0)
    , cur_beat(0)
    , cur_measure(0)
    , next_beat_time(0)
{
    for (int i = 0; i < num_tracks; ++i) {
        track_state[i] = std::make_unique<MIDISequencerTrackState>(s, i, notifier);
    }
}

MIDISequencerState::MIDISequencerState(MIDISequencerState const& s)
    : notifier(s.notifier)
    , multitrack(s.multitrack)
    , num_tracks(s.num_tracks)
    , iterator(s.iterator)
    , cur_clock(s.cur_clock)
    , cur_time_ms(s.cur_time_ms)
    , cur_beat(s.cur_beat)
    , cur_measure(s.cur_measure)
    , next_beat_time(s.next_beat_time)
{
    for (int i = 0; i < num_tracks; ++i) {
        track_state[i] = std::make_unique<MIDISequencerTrackState>(*s.track_state[i]);
    }
}

MIDISequencerState::~MIDISequencerState()
{}

MIDISequencerState const& MIDISequencerState::operator=(MIDISequencerState const& s)
{
    if (num_tracks != s.num_tracks) {
        {
            for (int i = 0; i < num_tracks; ++i) {
                track_state[i].reset();
            }
        }
        num_tracks = s.num_tracks;
        {
            for (int i = 0; i < num_tracks; ++i) {
                track_state[i] = std::make_unique<MIDISequencerTrackState>(*s.track_state[i]);
            }
        }
    }

    iterator = s.iterator;
    cur_clock = s.cur_clock;
    cur_time_ms = s.cur_time_ms;
    cur_beat = s.cur_beat;
    cur_measure = s.cur_measure;
    next_beat_time = s.next_beat_time;
    return *this;
}

////////////////////////////////////////////////////////////////////////////

MIDISequencer::MIDISequencer(MIDIMultiTrack* m, MIDISequencerGUIEventNotifier* n)
    : solo_mode(false)
    , tempo_scale(100)
    , num_tracks(m->get_num_tracks())
    , state(this, m, n)  // TODO: fix this hack
{
    for (int i = 0; i < num_tracks; ++i) {
        track_processors[i] = std::make_unique<MIDISequencerTrackProcessor>();
    }
}

MIDISequencer::~MIDISequencer()
{}

void MIDISequencer::reset_track(int trk)
{
    state.track_state[trk]->reset();
    track_processors[trk]->reset();
}

void MIDISequencer::reset_all_tracks()
{
    for (int i = 0; i < num_tracks; ++i) {
        state.track_state[i]->reset();
        track_processors[i]->reset();
    }
}

MIDISequencerState* MIDISequencer::get_state()
{
    return &state;
}

MIDISequencerState const* MIDISequencer::get_state() const
{
    return &state;
}

void MIDISequencer::set_state(MIDISequencerState* s)
{
    state = *s;
}

MIDIClockTime MIDISequencer::get_current_midi_clock_time() const
{
    return state.cur_clock;
}

double MIDISequencer::get_current_time_in_ms() const
{
    return state.cur_time_ms;
}

int MIDISequencer::get_current_beat() const
{
    return state.cur_beat;
}

int MIDISequencer::get_current_measure() const
{
    return state.cur_measure;
}

double MIDISequencer::get_current_tempo_scale() const
{
    return static_cast<double>(tempo_scale) * 0.01;
}

double MIDISequencer::get_current_tempo() const
{
    return state.track_state[0]->tempobpm;
}

MIDISequencerTrackState* MIDISequencer::get_track_state(int trk)
{
    return state.track_state[trk].get();
}

MIDISequencerTrackState const* MIDISequencer::get_track_state(int trk) const
{
    return state.track_state[trk].get();
}

MIDISequencerTrackProcessor* MIDISequencer::get_track_processor(int trk)
{
    return track_processors[trk].get();
}

MIDISequencerTrackProcessor const* MIDISequencer::get_track_processor(int trk) const
{
    return track_processors[trk].get();
}

bool MIDISequencer::get_solo_mode() const
{
    return solo_mode;
}

void MIDISequencer::set_current_tempo_scale(float scale)
{
    tempo_scale = static_cast<int>(scale * 100);
}

void MIDISequencer::set_solo_mode(bool m, int trk)
{
    int i;
    solo_mode = m;

    for (i = 0; i < num_tracks; ++i) {
        if (i == trk) {
            track_processors[i]->solo = true;
        }

        else {
            track_processors[i]->solo = false;
        }
    }
}

void MIDISequencer::go_to_zero()
{
    // go to time zero
    for (int i = 0; i < num_tracks; ++i) {
        state.track_state[i]->go_to_zero();
    }

    state.iterator.go_to_time(0);
    state.cur_time_ms = 0.0;
    state.cur_clock = 0;
    // state.next_beat_time = state.multitrack->get_clks_per_beat();
    state.next_beat_time =
        state.multitrack->get_clks_per_beat() * 4 / (state.track_state[0]->timesig_denominator);
    // examine all the events at this specific time
    // and update the track states to reflect this time
    scan_events_at_this_time();
}

bool MIDISequencer::go_to_time(MIDIClockTime time_clk)
{
    // temporarily disable the gui notifier
    bool notifier_mode = false;

    if (state.notifier) {
        notifier_mode = state.notifier->get_enable();
        state.notifier->set_enable(false);
    }

    if (time_clk < state.cur_clock || time_clk == 0) {
        // start from zero if desired time is before where we are
        for (int i = 0; i < state.num_tracks; ++i) {
            state.track_state[i]->go_to_zero();
        }

        state.iterator.go_to_time(0);
        state.cur_time_ms = 0.0;
        state.cur_clock = 0;
        //  state.next_beat_time = state.multitrack->get_clks_per_beat();
        state.next_beat_time =
            state.multitrack->get_clks_per_beat() * 4 / (state.track_state[0]->timesig_denominator);
        state.cur_beat = 0;
        state.cur_measure = 0;
    }

    MIDIClockTime t = 0;
    int trk;
    MIDITimedBigMessage ev;

    while (get_next_event_time(&t) && t <= time_clk && get_next_event(&trk, &ev)) {
        // Continue processing events up to and including the target time
        ;
    }

    // examine all the events at this specific time
    // and update the track states to reflect this time
    scan_events_at_this_time();

    // re-enable the gui notifier if it was enabled previously
    if (state.notifier) {
        state.notifier->set_enable(notifier_mode);
        // cause a full gui refresh now
        state.notifier->notify(this, MIDISequencerGUIEvent::GROUP_ALL);
    }

    return true;
}

bool MIDISequencer::go_to_time_ms(float time_ms)
{
    // temporarily disable the gui notifier
    bool notifier_mode = false;

    if (state.notifier) {
        notifier_mode = state.notifier->get_enable();
        state.notifier->set_enable(false);
    }

    if (time_ms < state.cur_time_ms || time_ms == 0.0) {
        // start from zero if desired time is before where we are
        for (int i = 0; i < state.num_tracks; ++i) {
            state.track_state[i]->go_to_zero();
        }

        state.iterator.go_to_time(0);
        state.cur_time_ms = 0.0;
        state.cur_clock = 0;
        //  state.next_beat_time = state.multitrack->get_clks_per_beat();
        state.next_beat_time =
            state.multitrack->get_clks_per_beat() * 4 / (state.track_state[0]->timesig_denominator);
        state.cur_beat = 0;
        state.cur_measure = 0;
    }

    float t = 0;
    int trk;
    MIDITimedBigMessage ev;

    while (get_next_event_time_ms(&t) && t < time_ms && get_next_event(&trk, &ev)) {
        ;
    }

    // examine all the events at this specific time
    // and update the track states to reflect this time
    // scan_events_at_this_time();

    // re-enable the gui notifier if it was enabled previously
    if (state.notifier) {
        state.notifier->set_enable(notifier_mode);
        // cause a full gui refresh now
        state.notifier->notify(this, MIDISequencerGUIEvent::GROUP_ALL);
    }

    return true;
}

bool MIDISequencer::go_to_measure(int measure, int beat)
{
    // temporarily disable the gui notifier
    bool notifier_mode = false;

    if (state.notifier) {
        notifier_mode = state.notifier->get_enable();
        state.notifier->set_enable(false);
    }

    if (measure < state.cur_measure || measure == 0) {
        for (int i = 0; i < state.num_tracks; ++i) {
            state.track_state[i]->go_to_zero();
        }

        state.iterator.go_to_time(0);
        state.cur_time_ms = 0.0;
        state.cur_clock = 0;
        state.cur_beat = 0;
        state.cur_measure = 0;
        //  state.next_beat_time = state.multitrack->get_clks_per_beat();
        state.next_beat_time =
            state.multitrack->get_clks_per_beat() * 4 / (state.track_state[0]->timesig_denominator);
    }

    MIDIClockTime t = 0;
    int trk;
    MIDITimedBigMessage ev;
    // iterate thru all the events until cur-measure and cur_beat are
    // where we want them.

    while (get_next_event_time(&t) && get_next_event(&trk, &ev) && state.cur_measure <= measure) {
        if (state.cur_measure == measure && state.cur_beat >= beat) {
            break;
        }
    }

    // examine all the events at this specific time
    // and update the track states to reflect this time
    scan_events_at_this_time();

    // re-enable the gui notifier if it was enabled previously
    if (state.notifier) {
        state.notifier->set_enable(notifier_mode);
        // cause a full gui refresh now
        state.notifier->notify(this, MIDISequencerGUIEvent::GROUP_ALL);
    }

    // return true if we actually found the measure requested
    return state.cur_measure == measure && state.cur_beat == beat;
}

bool MIDISequencer::get_next_event_time_ms(float* t)
{
    MIDIClockTime ct;
    bool f = get_next_event_time(&ct);

    if (f) {
        // calculate delta time from last event time
        double delta_clocks = static_cast<double>(ct - state.cur_clock);
        // calculate tempo in milliseconds per clock
        double clocks_per_sec =
            ((state.track_state[0]->tempobpm * (static_cast<double>(tempo_scale) * 0.01) *
              (1.0f / 60.0f)) *
             state.multitrack->get_clks_per_beat());

        if (clocks_per_sec > 0) {
            float ms_per_clock = 1000.0f / static_cast<float>(clocks_per_sec);
            // calculate delta time in milliseconds
            float delta_ms = float(delta_clocks * ms_per_clock);
            // return it added with the current time in ms.
            *t = delta_ms + state.cur_time_ms;
        }

        else {
            f = false;
        }
    }

    return f;
}

bool MIDISequencer::get_next_event_time(MIDIClockTime* t)
{
    // ask the iterator for the current event time
    bool f = state.iterator.get_cur_event_time(t);

    if (f) {
        // if we have an event in the future, check to see if it is
        // further in time than the next beat marker
        if ((*t) >= state.next_beat_time) {
            // ok, the next event is a beat - return the next beat time
            *t = state.next_beat_time;
        }
    }

    return f;
}

bool MIDISequencer::get_next_event(int* tracknum, MIDITimedBigMessage* msg)
{
    MIDIClockTime t;

    // ask the iterator for the current event time
    if (state.iterator.get_cur_event_time(&t)) {
        // move current time forward one event
        MIDIClockTime new_clock;
        float new_time_ms = 0.0f;
        get_next_event_time(&new_clock);
        get_next_event_time_ms(&new_time_ms);
        // must set cur_clock AFTER GetnextEventTimeMs() is called
        // since get_next_event_time_ms() uses cur_clock to calculate
        state.cur_clock = new_clock;
        state.cur_time_ms = new_time_ms;
        // is the next beat marker before this event?

        if (state.next_beat_time <= t) {
            // yes, this is a beat event now.
            // say this event came on track 0, the conductor track
            *tracknum = 0;
            // put current info into beat marker message
            beat_marker_msg.set_beat_marker();
            beat_marker_msg.set_time(state.next_beat_time);
            *msg = beat_marker_msg;
            // update our beat count
            int new_beat = state.cur_beat + 1;
            int new_measure = state.cur_measure;
            // do we need to update the measure number?

            if (new_beat >= state.track_state[0]->timesig_numerator) {
                // yup
                new_beat = 0;
                ++new_measure;
            }

            // update our next beat time
            // denom=4  (16) ---> 4/16 midi file beats per symbolic beat
            // denom=3  (8)  ---> 4/8 midi file beats per symbolic beat
            // denom=2  (4)  ---> 4/4 midi file beat per symbolic beat
            // denom=1  (2)  ---> 4/2 midi file beats per symbolic beat
            // denom=0  (1)  ---> 4/1 midi file beats per symbolic beat
            state.next_beat_time += state.multitrack->get_clks_per_beat() * 4 /
                (state.track_state[0]->timesig_denominator);
            state.cur_beat = new_beat;
            state.cur_measure = new_measure;

            // now notify the GUI that the beat number changed
            if (state.notifier) {
                state.notifier->notify(
                    this,
                    MIDISequencerGUIEvent(
                        MIDISequencerGUIEvent::GROUP_TRANSPORT,
                        0,
                        MIDISequencerGUIEvent::GROUP_TRANSPORT_BEAT));
            }

            // if the new beat number is 0 then the measure changed too
            if (state.cur_beat == 0 && state.notifier) {
                state.notifier->notify(
                    this,
                    MIDISequencerGUIEvent(
                        MIDISequencerGUIEvent::GROUP_TRANSPORT,
                        0,
                        MIDISequencerGUIEvent::GROUP_TRANSPORT_MEASURE));
            }

            // give the beat marker event to the conductor track to process
            state.track_state[*tracknum]->process(msg);
            return true;
        }

        else  // this event comes before the next beat
        {
            MIDITimedBigMessage* msg_ptr;

            if (state.iterator.get_cur_event(tracknum, &msg_ptr)) {
                int trk = *tracknum;
                // copy the event so Process can modify it
                *msg = *msg_ptr;
                bool allow_msg = true;
                // are we in solo mode?

                if (solo_mode) {
                    // yes, only allow this message thru if
                    // the track is either track 0
                    // or it is explicitly solod.
                    if (trk == 0 || track_processors[trk]->solo) {
                        allow_msg = true;
                    }

                    else {
                        allow_msg = false;
                    }
                }

                if (!(allow_msg && track_processors[trk]->process(msg) &&
                      state.track_state[trk]->process(msg))) {
                    // the message is not allowed to come out!
                    // erase it
                    msg->set_no_op();
                }

                // go to the next event on the multitrack
                state.iterator.go_to_next_event();
                return true;
            }
        }
    }

    return false;
}

void MIDISequencer::scan_events_at_this_time()
{
    // save the current iterator state
    MIDIMultiTrackIteratorState istate(state.iterator.get_state());
    int prev_measure = state.cur_measure;
    int prev_beat = state.cur_beat;
    // process all messages up to and including this time only
    MIDIClockTime orig_clock = state.cur_clock;
    double orig_time_ms = state.cur_time_ms;

    // The iterator has a limitation: it only returns one event per timestamp
    // even when multiple tracks have events at the same time.
    // To work around this, we manually scan all tracks for events at the current time.
    // Note: Events in tracks are stored in insertion order, not necessarily sorted by time.
    for (int track_num = 0; track_num < state.num_tracks; ++track_num) {
        auto track = state.multitrack->get_track(track_num);
        if (track) {
            // Scan through ALL events in this track to find ones at the current time
            // We can't assume they're sorted, so we must check every event
            for (int event_idx = 0; event_idx < track->get_num_events(); ++event_idx) {
                auto event = track->get_event(event_idx);
                if (event && event->get_time() == orig_clock) {
                    // Found an event at the current time - process it
                    MIDITimedBigMessage ev = *event;
                    state.track_state[track_num]->process(&ev);
                }
            }
        }
    }

    // restore the iterator state
    state.iterator.set_state(istate);
    // and current time
    state.cur_clock = orig_clock;
    state.cur_time_ms = float(orig_time_ms);
    state.cur_measure = prev_measure;
    state.cur_beat = prev_beat;
}

}  // namespace jdksmidi
