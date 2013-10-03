/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

//
// Modified by N. Cassetta ncassetta@tiscali.it
//

#ifndef JDKSMIDI_MANAGER_H
#define JDKSMIDI_MANAGER_H

#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/driver.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/tick.h"


namespace jdksmidi
{

extern unsigned long jdks_get_system_time_ms();

///
/// This class manages MIDI playback, picking MIDI messages from a MIDISequencer and sending them to a
/// MIDIDriver (and then to MIDI ports).
/// It inherits from pure virtual MIDITick, i.e. a class with a callback method TimeTick() to be called at every
/// timer tick: when sequencer is playing the MIDIManager uses it for moving MIDI messages from the sequencer to
/// the driver. For effective playback you must have a MIDISequencer, a MIDIDriver and a MIDIManager: the
/// AdvancedSequencer is an all-in-one class embedding all these. See example files for effective using.
///

class MIDIManager : public MIDITick
{
public:
    /// The constructor.
    /// \param drv the MIDIDriver to whom send MIDI messages
    /// \param n a MIDISequencerGUIEventNotifier (the class can send MIDISequencerGUIEvent to the GUI)
    /// \param seq_ the sequencer that provides MIDI messages
    ///
    MIDIManager (
        MIDIDriver *drv,
        MIDISequencerGUIEventNotifier *n = 0,
        MIDISequencer *seq_ = 0
    );

    /// The destructor doesn't free any memory
    virtual ~MIDIManager()
    {
    }

    /// Stops playing and resets the MIDIManager. The notifier sends to the GUI a GROUP_ALL (reset) message
    void Reset();

    /// Sets current sequencer. The notifier sends to the GUI a GROUP_ALL (reset) message
    void SetSeq ( MIDISequencer *seq );

    /// Returns current sequencer
    MIDISequencer *GetSeq()
    {
        return sequencer;
    }

    const MIDISequencer *GetSeq() const
    {
        return sequencer;
    }

    /// Returns the driver that we use
    MIDIDriver *GetDriver()
    {
        return driver;
    }


    /// Sets the system time offset, i.e.\ the delta time between system and manager time.\ You must always
    /// set this immediately before the sequencer starts.
    /// \param off the current system time in msecs. You can get it with the OS independent
    /// jdks_get_system_time_ms()
    // note to Jeff by NC: I see no utility for this function alone, so I think it would be best to integrate
    // it in SeqPlay(): there should be no problems of compatibility, as you must always call this before SeqPlay
    void SetTimeOffset ( unsigned long off )
    {
        sys_time_offset = off;
    }

    /// Returns the system time offset
    unsigned long GetTimeOffset() const
    {
        return sys_time_offset;
    }

    /// Returns the time (in msecs) elapsed from the sequencer start (0 if the sequencer is not playing)
    unsigned long GetCurrentTimeInMs() const
    {
        if ( play_mode )
        {
            return jdks_get_system_time_ms() + seq_time_offset - sys_time_offset;
        }
        else
        {
            return 0;
        }
    }


    /// Sets the sequencer time offset, i.e.\ the sequencer starting time.\ You must always
    /// set this before the sequencer starts.
    /// \param seqoff the sequencer 'now' time in msecs. You can get it with MIDISequencer::GetCurrentTimeInMs()
    // note to Jeff by NC: as above
    void SetSeqOffset ( unsigned long seqoff )
    {
        seq_time_offset = seqoff;
    }

    /// Returns the sequencer time offset in msecs
    unsigned long GetSeqOffset() const
    {
        return seq_time_offset;
    }

    /// Starts the sequencer playback.\ The notifier sends to the GUI a GROUP_TRANSPORT_MODE nessage.
    /// Plsyback stops automatically at end of song
    void SeqPlay();

    /// Stops the sequencer.\ The notifier sends to the GUI a GROUP_TRANSPORT_MODE nessage
    void SeqStop();

    /// Sets internal paramenters for repeated (loop) playing.\ Doesn't starts the playback
    /// \param flag on/off
    /// \param start_measure, end_measure first and last measures of the loop
    void SetRepeatPlay (
        bool flag,
        unsigned long start_measure,
        unsigned long end_measure
    );


    /// Returns *true* if the sequencer is playing
    bool IsSeqPlay() const
    {
        return play_mode;
    }

    /// Returns *true* if the sequencer is not playing
    bool IsSeqStop() const
    {
        return stop_mode;
    }

    /// Returns *true* if repeat playing is on
    bool IsSeqRepeat() const
    {
        return repeat_play_mode && play_mode;
    }


    /// This is the callback function inherited from MIDITick class. It only calls TimeTickPlayMode or TimeTickStopMode
    virtual void TimeTick ( unsigned long sys_time );

protected:

    /// This is the callback function that at every timer tick picks MIDI messages from the sequencer and
    /// sends them to the driver (and then to the MIDI out port)
    virtual void TimeTickPlayMode ( unsigned long sys_time_ );

    /// Currently this does nothing
    virtual void TimeTickStopMode ( unsigned long sys_time_ );

    MIDIDriver *driver;                 ///< The driver

    MIDISequencer *sequencer;           ///< The sequencer

    unsigned long sys_time_offset;      ///< Delta time between system and manager. Used by TimeTickPlayMode()
    unsigned long seq_time_offset;      ///< Start time of the sequencer in msecs

    volatile bool play_mode;            ///< *true* if the sequencer is playing
    volatile bool stop_mode;            ///< *true* if the sequencer is not playing

    MIDISequencerGUIEventNotifier *notifier;    ///< the notifier thar sends messages to the GUI

    volatile bool repeat_play_mode;     ///< *true* if the loop mode is on
    long repeat_start_measure;          ///< first measure of the loop
    long repeat_end_measure;            ///< last measure of the loop


};


}

#endif
