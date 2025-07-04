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
#ifndef JDKSMIDI_MANAGER_H
#define JDKSMIDI_MANAGER_H

#include "jdksmidi/driver.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/tick.h"

namespace jdksmidi {
class MIDIManager : public MIDITick
{
  public:
    MIDIManager(MIDIDriver* drv, MIDISequencerGUIEventNotifier* n = 0, MIDISequencer* seq_ = 0);

    virtual ~MIDIManager();

    void reset();

    // to set and get the current sequencer
    void set_seq(MIDISequencer* seq);
    MIDISequencer* get_seq();
    MIDISequencer const* get_seq() const;

    // to get the driver that we use
    MIDIDriver* get_driver() { return _driver; }

    // to set and get the system time offset
    void set_time_offset(unsigned long off);
    unsigned long get_time_offset();

    // to set and get the sequencer time offset
    void set_seq_offset(unsigned long seqoff);
    unsigned long get_seq_offset();

    // to manage the playback of the sequencer
    void seq_play();
    void seq_stop();
    void set_repeat_play(bool flag, unsigned long start_measure, unsigned long end_measure);

    // status request functions
    bool is_seq_play() const;
    bool is_seq_stop() const;
    bool is_seq_repeat() const;

    // inherited from MIDITick
    virtual void time_tick(unsigned long sys_time);

  protected:
    virtual void time_tick_play_mode(unsigned long sys_time_);
    virtual void time_tick_stop_mode(unsigned long sys_time_);

    MIDIDriver* _driver;

    MIDISequencer* _sequencer;

    unsigned long _sys_time_offset;
    unsigned long _seq_time_offset;

    bool volatile _play_mode;
    bool volatile _stop_mode;

    MIDISequencerGUIEventNotifier* _notifier;

    bool volatile _repeat_play_mode;
    long _repeat_start_measure;
    long _repeat_end_measure;
};

}  // namespace jdksmidi

#endif
