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
#include "jdksmidi/driver.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sequencer.h"

#include <cstdint>

namespace jdksmidi {

MIDIManager::MIDIManager(MIDIDriver* drv, MIDISequencerGUIEventNotifier* n, MIDISequencer* seq_)
    : _driver(drv)
    , _sequencer(seq_)
    , _sys_time_offset(0)
    , _seq_time_offset(0)
    , _play_mode(false)
    , _stop_mode(true)
    , _notifier(n)
    , _repeat_play_mode(false)
    , _repeat_start_measure(0)
    , _repeat_end_measure(0)
{
    _driver->set_tick_proc(this);
}

MIDIManager::~MIDIManager()
{}

void MIDIManager::reset()
{
    seq_stop();
    _sys_time_offset = 0;
    _seq_time_offset = 0;
    _play_mode = false;
    _stop_mode = true;

    if (_notifier) {
        _notifier->notify(_sequencer, MIDISequencerGUIEvent(MIDISequencerGUIEvent::GROUP_ALL));
    }
}

// to set and get the current sequencer
void MIDIManager::set_seq(MIDISequencer* seq)
{
    if (_notifier) {
        _notifier->notify(_sequencer, MIDISequencerGUIEvent(MIDISequencerGUIEvent::GROUP_ALL));
    }

    _sequencer = seq;
}

MIDISequencer* MIDIManager::get_seq()
{
    return _sequencer;
}

MIDISequencer const* MIDIManager::get_seq() const
{
    return _sequencer;
}

// to set and get the system time offset
void MIDIManager::set_time_offset(std::uint32_t off)
{
    _sys_time_offset = off;
}

std::uint32_t MIDIManager::get_time_offset()
{
    return _sys_time_offset;
}

// to set and get the sequencer time offset
void MIDIManager::set_seq_offset(std::uint32_t seqoff)
{
    _seq_time_offset = seqoff;
}

std::uint32_t MIDIManager::get_seq_offset()
{
    return _seq_time_offset;
}

// to manage the playback of the sequencer
void MIDIManager::seq_play()
{
    _stop_mode = false;
    _play_mode = true;

    if (_notifier) {
        _notifier->notify(
            _sequencer,
            MIDISequencerGUIEvent(
                MIDISequencerGUIEvent::GROUP_TRANSPORT,
                0,
                MIDISequencerGUIEvent::GROUP_TRANSPORT_MODE));
    }
}

// to manage the repeat playback of the sequencer
void MIDIManager::set_repeat_play(bool flag, std::uint32_t start_measure, std::uint32_t end_measure)
{
    // shut off repeat play while we muck with values
    _repeat_play_mode = false;
    _repeat_start_measure = start_measure;
    _repeat_end_measure = end_measure;
    // set repeat mode flag to how we want it.
    _repeat_play_mode = flag;
}

void MIDIManager::seq_stop()
{
    _play_mode = false;
    _stop_mode = true;

    if (_notifier) {
        _notifier->notify(
            _sequencer,
            MIDISequencerGUIEvent(
                MIDISequencerGUIEvent::GROUP_TRANSPORT,
                0,
                MIDISequencerGUIEvent::GROUP_TRANSPORT_MODE));
    }
}

// status request functions
bool MIDIManager::is_seq_play() const
{
    return _play_mode;
}

bool MIDIManager::is_seq_stop() const
{
    return _stop_mode;
}

bool MIDIManager::is_seq_repeat() const
{
    return _repeat_play_mode && _play_mode;
}

void MIDIManager::time_tick(std::uint32_t sys_time_)
{
    if (_play_mode) {
        time_tick_play_mode(sys_time_);
    }

    else if (_stop_mode) {
        time_tick_stop_mode(sys_time_);
    }
}

void MIDIManager::time_tick_play_mode(std::uint32_t sys_time_)
{
    double sys_time = (double)sys_time_ - (double)_sys_time_offset;
    float next_event_time = 0.0;
    int ev_track;
    MIDITimedBigMessage ev;

    // if we are in repeat mode, repeat if we hit end of the repeat region
    if (_repeat_play_mode && _sequencer->get_current_measure() >= _repeat_end_measure) {
        // yes we hit the end of our repeat block
        // shut off all notes on
        _driver->all_notes_off();
        // now move the sequencer to our start position
        _sequencer->go_to_measure(_repeat_start_measure);
        // our current raw system time is now the new system time offset
        _sys_time_offset = sys_time_;
        sys_time = 0;
        // the sequencer time offset now must be reset to the
        // time in milliseconds of the sequence start point
        _seq_time_offset = (std::uint32_t)_sequencer->get_current_time_in_ms();
    }

    // find all events that exist before or at this time,
    // but only if we have space in the output queue to do so!
    // also limit ourselves to 100 midi events max.
    int output_count = 100;

    while (_sequencer->get_next_event_time_ms(&next_event_time) &&
           (next_event_time - _seq_time_offset) <= sys_time && _driver->can_output_message() &&
           (--output_count) > 0) {
        // found an event! get it!
        if (_sequencer->get_next_event(&ev_track, &ev)) {
            // ok, tell the driver the send this message now
            _driver->output_message(ev);
        }
    }

    // auto stop at end of sequence

    if (!_sequencer->get_next_event_time_ms(&next_event_time)) {
        // no events left
        _stop_mode = true;
        _play_mode = false;

        if (_notifier) {
            _notifier->notify(
                _sequencer,
                MIDISequencerGUIEvent(
                    MIDISequencerGUIEvent::GROUP_TRANSPORT,
                    0,
                    MIDISequencerGUIEvent::GROUP_TRANSPORT_MODE));
            _notifier->notify(
                _sequencer,
                MIDISequencerGUIEvent(
                    MIDISequencerGUIEvent::GROUP_TRANSPORT,
                    0,
                    MIDISequencerGUIEvent::GROUP_TRANSPORT_ENDOFSONG));
        }
    }
}

void MIDIManager::time_tick_stop_mode(std::uint32_t sys_time_)
{}
}  // namespace jdksmidi
