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
#include "jdksmidi/manager.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

MIDIManager::MIDIManager(MIDIDriver* drv, MIDISequencerGUIEventNotifier* n, MIDISequencer* seq_)
    : driver(drv)
    , sequencer(seq_)
    , sys_time_offset(0)
    , seq_time_offset(0)
    , play_mode(false)
    , stop_mode(true)
    , notifier(n)
    , repeat_play_mode(false)
    , repeat_start_measure(0)
    , repeat_end_measure(0)
{
    driver->set_tick_proc(this);
}

MIDIManager::~MIDIManager()
{}

void MIDIManager::reset()
{
    SeqStop();
    sys_time_offset = 0;
    seq_time_offset = 0;
    play_mode = false;
    stop_mode = true;

    if (notifier) {
        notifier->Notify(sequencer, MIDISequencerGUIEvent(MIDISequencerGUIEvent::GROUP_ALL));
    }
}

// to set and get the current sequencer
void MIDIManager::SetSeq(MIDISequencer* seq)
{
    if (notifier) {
        notifier->Notify(sequencer, MIDISequencerGUIEvent(MIDISequencerGUIEvent::GROUP_ALL));
    }

    sequencer = seq;
}

MIDISequencer* MIDIManager::GetSeq()
{
    return sequencer;
}

MIDISequencer const* MIDIManager::GetSeq() const
{
    return sequencer;
}

// to set and get the system time offset
void MIDIManager::SetTimeOffset(unsigned long off)
{
    sys_time_offset = off;
}

unsigned long MIDIManager::GetTimeOffset()
{
    return sys_time_offset;
}

// to set and get the sequencer time offset
void MIDIManager::SetSeqOffset(unsigned long seqoff)
{
    seq_time_offset = seqoff;
}

unsigned long MIDIManager::GetSeqOffset()
{
    return seq_time_offset;
}

// to manage the playback of the sequencer
void MIDIManager::SeqPlay()
{
    stop_mode = false;
    play_mode = true;

    if (notifier) {
        notifier->Notify(
            sequencer,
            MIDISequencerGUIEvent(
                MIDISequencerGUIEvent::GROUP_TRANSPORT,
                0,
                MIDISequencerGUIEvent::GROUP_TRANSPORT_MODE));
    }
}

// to manage the repeat playback of the sequencer
void MIDIManager::SetRepeatPlay(bool flag, unsigned long start_measure, unsigned long end_measure)
{
    // shut off repeat play while we muck with values
    repeat_play_mode = false;
    repeat_start_measure = start_measure;
    repeat_end_measure = end_measure;
    // set repeat mode flag to how we want it.
    repeat_play_mode = flag;
}

void MIDIManager::SeqStop()
{
    play_mode = false;
    stop_mode = true;

    if (notifier) {
        notifier->Notify(
            sequencer,
            MIDISequencerGUIEvent(
                MIDISequencerGUIEvent::GROUP_TRANSPORT,
                0,
                MIDISequencerGUIEvent::GROUP_TRANSPORT_MODE));
    }
}

// status request functions
bool MIDIManager::IsSeqPlay() const
{
    return play_mode;
}

bool MIDIManager::IsSeqStop() const
{
    return stop_mode;
}

bool MIDIManager::IsSeqRepeat() const
{
    return repeat_play_mode && play_mode;
}

void MIDIManager::time_tick(unsigned long sys_time_)
{
    if (play_mode) {
        TimeTickPlayMode(sys_time_);
    }

    else if (stop_mode) {
        TimeTickStopMode(sys_time_);
    }
}

void MIDIManager::TimeTickPlayMode(unsigned long sys_time_)
{
    double sys_time = (double)sys_time_ - (double)sys_time_offset;
    float next_event_time = 0.0;
    int ev_track;
    MIDITimedBigMessage ev;

    // if we are in repeat mode, repeat if we hit end of the repeat region
    if (repeat_play_mode && sequencer->get_current_measure() >= repeat_end_measure) {
        // yes we hit the end of our repeat block
        // shut off all notes on
        driver->all_notes_off();
        // now move the sequencer to our start position
        sequencer->go_to_measure(repeat_start_measure);
        // our current raw system time is now the new system time offset
        sys_time_offset = sys_time_;
        sys_time = 0;
        // the sequencer time offset now must be reset to the
        // time in milliseconds of the sequence start point
        seq_time_offset = (unsigned long)sequencer->get_current_time_in_ms();
    }

    // find all events that exist before or at this time,
    // but only if we have space in the output queue to do so!
    // also limit ourselves to 100 midi events max.
    int output_count = 100;

    while (sequencer->get_next_event_time_ms(&next_event_time) &&
           (next_event_time - seq_time_offset) <= sys_time && driver->can_output_message() &&
           (--output_count) > 0) {
        // found an event! get it!
        if (sequencer->get_next_event(&ev_track, &ev)) {
            // ok, tell the driver the send this message now
            driver->output_message(ev);
        }
    }

    // auto stop at end of sequence

    if (!sequencer->get_next_event_time_ms(&next_event_time)) {
        // no events left
        stop_mode = true;
        play_mode = false;

        if (notifier) {
            notifier->Notify(
                sequencer,
                MIDISequencerGUIEvent(
                    MIDISequencerGUIEvent::GROUP_TRANSPORT,
                    0,
                    MIDISequencerGUIEvent::GROUP_TRANSPORT_MODE));
            notifier->Notify(
                sequencer,
                MIDISequencerGUIEvent(
                    MIDISequencerGUIEvent::GROUP_TRANSPORT,
                    0,
                    MIDISequencerGUIEvent::GROUP_TRANSPORT_ENDOFSONG));
        }
    }
}

void MIDIManager::TimeTickStopMode(unsigned long sys_time_)
{}
}  // namespace jdksmidi
