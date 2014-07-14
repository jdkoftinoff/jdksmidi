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
/*
** Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

//
// MODIFIED by N. Cassetta ncassetta@tiscali.it
//


#ifndef JDKSMIDI_ADVANCEDSEQUENCER_H
#define JDKSMIDI_ADVANCEDSEQUENCER_H

#include "jdksmidi/world.h"     // ISSUE: (NC) see the source file
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/driver.h"


#ifdef _WIN32
#include "jdksmidi/driverwin32.h"
#else
#include "jdksmidi/driverdump.h"
#endif // _WIN32



#include <vector>

// #define MAX_WARP_POSITIONS (128)
// #define MEASURES_PER_WARP (4)    NC this is now a static const class attribute

namespace jdksmidi
{

///
/// A high level, all-in-one object capable to load an play MIDI files.
/// Its features include:
/// - Loading of MIDI files with a single call to a class method
/// - Play, Repeat Play (i.e. loop) and Stop commands
/// - Allows jumping from one time to another (even during playback) with correct responding
/// to wheel, patch, controls and sysex changes
/// - Individual solo, mute, velocity scale, transpose and rechannelize for every track
/// - Global tempo scale
/// - MIDI thru: you can play along with your MIDI instrument while the sequencer is playing
///
/// This class embeds many jdksmidi objects: a MIDISequencer (with its MIDIMultiTrack) for
/// storing MIDI data, a MIDIDriver to communicate with hardware MIDI ports, a MIDIManager for
/// handling sequencer playing, some MIDIProcessor for transposing, rechannelizing, etc.
/// At current time, the class can manage MIDI playback only on WIN32; for other OS it prints to
/// the console a dump of sent messages.
///

class AdvancedSequencer
{
public:
    // AdvancedSequencer(); OLD
    // new by NC: the user can now set his own notifier (text, GUI, or no notifier ...)

    /// This first form of the constructor creates autonomally all the underlying objects (MIDIMultiTrack,
    /// MIDISequencer, MIDIDriver and MIDIManager) that are owned by the class. The MIDIMultiTrack will have
    /// 17 tracks. You can specify a notifier to communicate with the GUI (this is not owned by the class).
    /// If you want to edit the MIDI events, get the MIDIMultiTrack address with the GetMultiTrack() method.
    AdvancedSequencer(MIDISequencerGUIEventNotifier *n = 0);

    /// The second form of the constructor lets the user specify the MIDIMultiTrack (and eventually the notifier).
    /// Use this if you need a MIDIMultiTrack with more of 17 tracks.
    AdvancedSequencer(MIDIMultiTrack* mlt, MIDISequencerGUIEventNotifier *n = 0);

    /// The third form of the constructor allows the user to specify the underlying objects, for polimorphysm
    /// purposes. You must create by yourself a MIDIMultiTrack, a MIDISequencer, a MIDIDriver, an (eventual)
    /// MIDISequencerGUIEventNotifier and a MIDIManager and pass the latter address to the constructor.
    /// It doesn't check if the MIDIManager is correctly built, so be careful with this function. Objects
    /// will not be owned by the class, so the user is responsible for freeing them.
    AdvancedSequencer(MIDIManager* mg);

    /// The destructor frees underlying objects if they were allocated by the constructor (the notifier is
    /// never owned by the class)
    virtual ~AdvancedSequencer();

    // bool OpenMIDI ( int in_port, int out_port, int timer_resolution = DEFAULT_TIMER_RESOLUTION );
    // void CloseMIDI();
    // NOTE by NC: these are now protected

    /// Sets the MIDI out port. For now only the same port for all the tracks is allowed. You can get the list of
    /// all MIDI ports by the static MIDIDriverWin32 functions
    /// \see MIDIDriverWin32::GetNumMIDIOutDevs() , MIDIDriverWin32::GetMIDIOutDevName()
    void SetOutputPort( int p);

    /// Returns the currently open MIDI out port
    int GetOutputPort() const
    {
        return out_port;
    }

    /// Sets the MIDI in port for MIDI thru. You can get the list of
    /// all MIDI ports by the static MIDIDriverWin32 functions
    /// \see MIDIDriverWin32::GetNumMIDIInDevs() , MIDIDriverWin32::GetMIDIInDevName()
    void SetInputPort( int p);

    /// Returns the currently open MIDI in port
    int GetInputPort() const
    {
        return in_port;
    }

    /// Sets the MIDI thru enable (_f_ = on/off). The user can play along via the MIDI in port while the
    /// sequencer is playing
    void SetMIDIThruEnable ( bool f )
    {
        driver->SetThruEnable ( f );
    }

    /// Returns the MIDI thru status (on/off)
    bool GetMIDIThruEnable() const
    {
        return driver->GetThruEnable();
    }

    /// Sets the MIDI thru channel for receiving MIDI messages
    void SetMIDIThruChannel ( int chan );

    /// Returns the MIDI out channel for receiving MIDI messages
    int GetMIDIThruChannel() const
    {
        return thru_rechannelizer.GetRechanMap ( 0 );
    }

    /// Allows the transposition of MIDI thru incoming data of _val_ semitones
    void SetMIDIThruTranspose ( int val );

    /// Returns the MIDI thru transposition amount
    int GetMIDIThruTranspose() const
    {
        return thru_transposer.GetTransposeChannel ( 0 );
    }

    /// Loads a MIDI File into the sequencer. Returns **true** if loading was successful
    bool Load ( const char *fname );

    /// Unloads the sequencer contents, leaving it empty
    void UnLoad ();

    /// Resets mute, solo, tempo scale, velocity scale, rechannel and loop play
    void Reset();

    /* NEW BY NC */
    /// This function gives access to the internal MIDIMultiTrack, allowing the user to edit it.
    /// When a change is done in the MIDIMultiTrack, the user must call SetChanged() to mantain
    /// internal coherence.
    MIDIMultiTrack* GetMultiTrack()
    {
        return tracks;
    }

    const MIDIMultiTrack* GetMUltiTrack() const
    {
        return tracks;
    }

    /// Sets the 'now' time to the beginning of the song, upgrading the internal status.
    /// Notifies the GUI a GROUP_ALL notifier event to signify a GUI reset
    void GoToZero();    /* NEW by NC */

    /// Sets the 'now' time to _measure_ and _beat_, upgrading the internal status.
    /// \return see GoToTime()
    void GoToMeasure ( int measure, int beat = 0 );

    /// Sets the 'now' time to the MIDI time _t_, upgrading the internal status.
    /// Notifies the GUI a GROUP_ALL notifier event to signify a GUI reset
    /// \return **true** if the time _t_ is effectively reached, **false** otherwise (for ex. if _t_ is after
    /// the end of the song)
    void GoToTime ( MIDIClockTime t );

    /// Starts MIDI playing from current time position.
    /// The notifier (if enabled) sends a GROUP_TRANSPORT_MODE to the GUI. During playback it will send
    /// GROUP_TRANSPORT_BEAT and GROUP_TRANSPORT_MEASURE messages at every beat and measure, and finally a
    /// GROUP_TRANSPORT_MODE or GROUP_TRANSPORT_ENDOFSONG. The GUI can examine the sequencer status for upgrading himself.
    void Play();
    // void RepeatPlay ( bool enable, int start_measure, int end_measure );
    // renamed SetRepeatPlay: this doesn't trigger Play but only sets parameters

    /// Stops sequencer playing. Sends a GROUP_TRANSPORT_MODE message to the GUI
    void Stop();

    /// Sends a MIDI message to the currently open MIDI port (time is ignored).
    /// This function is useful if you want to change MIDI values during playback
    void OutputMessage(MIDITimedBigMessage &msg);  /* NEW BY NC */

    /// Sets repeated play, i.e.\ loop playing. Note that this doesn't start playing!
    /// \param enable loop on/off
    /// \param start_measure, end_measure first and last measures of the loop. (If start>= end disables loop)
    void SetRepeatPlay ( bool enable, int start_measure, int end_measure );

    /// Returns **true** if the sequencer is playing
    bool IsPlay()
    {
        return mgr->IsSeqPlay();
    }

    /// Soloes track _trk_. Only one track at a time can be soloed
    void SoloTrack ( int trk );

    /// Unsoloes the soloed track
    void UnSoloTrack();

    /// Returns **true** if track _trk_ is soloed
    bool GetTrackSolo( int trk )          /* NEW BY NC */
    {
        return seq->GetTrackProcessor (trk)->solo;
    }

    /// Set track muting (_f_ = on/off) for track _trk_
    void SetTrackMute ( int trk, bool f );

    /// Returns **true** if track _trk_ is muted
    bool GetTrackMute( int trk )          /* NEW BY NC */
    {
        return seq->GetTrackProcessor (trk)->mute;
    }

    /// Resets all tracks as not muted
    void UnmuteAllTracks();

    /// Sets tempo scaling (1.00 = no scaling, 2.00 = twice faster, etc.)
    void SetTempoScale ( double scale );

    /// Returns curremt tempo (BPM) without tempo scale
    double GetTempoWithoutScale() const
    {
        return seq->GetCurrentTempo();
    }

    /// Returns current tempo (BPM) with scale, i.e.\ efffective playing tempo
    double GetTempoWithScale() const
    {
        return seq->GetCurrentTempo() * seq->GetCurrentTempoScale();
    }

    /// Returns 'now' MIDI clock time.
    /// It is effective even during playback
    unsigned long GetCurrentMIDIClockTime() const; /* NEW BY NC */

    /// Returns 'now' time in milliseconds.
    /// When playing or jumping from one time to another, you can use this to feed a SMPTE
    unsigned long GetCurrentTimeInMs() const; /* NEW BY NC */

    /// Set MIDI ticks per beat (quarter note).
    /// \return **true** if clocks per beat are effectively changed
    /// \note  Currently the user is allowed to change this only when the sequencer is empty; default value is
    /// 120 clocks per quarter beat. However, LoadFile() can change this according to the file clock, and Unload()
    /// resets it to 120
    bool SetClksPerBeat ( unsigned int cpb );

    /// Returns the base MIDI ticks per beat of the internal MIDIMultiTrack
    int GetClksPerBeat() const  /* NEW BY NC */
    {
        return tracks->GetClksPerBeat();
    }

    /// Returns the number of tracks of the sequencer
    int GetNumTracks() const
    /* NOTE BY NC: actually this always returns 17, the number of tracks of the MIDIMultiTrack
     * TODO: modify the MIDIMultiTrack class to keep track of tracks actually used and implement
     * a function GetUsedTracks()
     */
    {
        return seq->GetNumTracks();
    }

    /// Returns the number of measures of currently loaded song
    int GetNumMeasures() const
    {
        return num_measures;
    }

    /// Returns current measure number (1st measure = 0).
    /// When playing or jumping from one time to another, you can use this to refresh the GUI
    int GetMeasure() const;

    /// Returns current beat number (1st beat = 0)
    /// When playing or jumping from one time to another, you can use this to refresh the GUI
    int GetBeat() const;

    /// Returns the current timesig numerator
    int GetTimeSigNumerator() const;

    /// Returns the current timesig denominator
    int GetTimeSigDenominator() const;

    /// Returns the number of notes currently playing on track _trk_ (always 0 if the sequencer is not playing)
    int GetTrackNoteCount ( int trk ) const;

    /// Returns the MIDI name of track _trk_
    const char *GetTrackName ( int trk ) const;

    /// Returns the MIDI volume of track _trk_
    int GetTrackVolume ( int trk ) const;

    /// Returns the MIDI program (patch) of track _trk_. Currently banks are not supported
    int GetTrackProgram ( int trk ) const;      /* NEW BY NC */

    /// Set the velocity scale for track _trk_ (1.00 = no scaling)
    void SetTrackVelocityScale ( int trk, double scale );

    /// Returns the current velocity scale
    double GetTrackVelocityScale ( int trk ) const;

    /// Rechannelize track _trk_ sending its event to MIDI channel _chan_ (_chan = 0 ... 15)
    void SetTrackRechannelize ( int trk, int chan );

    /// Returns the rechannelize setting (0 ... 15) for track _trk_. (-1 if no rechannelize)
    int GetTrackRechannelize ( int trk ) const;

    /// Transpose track _trk_ of _trans_ semitones
    void SetTrackTranspose ( int trk, int trans );

    /// Returns the transpose amount for track _trk_
    int GetTrackTranspose ( int trk ) const;

    // void ExtractMarkers ( std::vector< std::string > *list );
    // int GetCurrentMarker() const;
    /* NOTE BY NC: I think these could be abandoned: now the current marker is given by the
     * sequencer state. However, for now I mantain them commented and substitute the second with
     * this
     */

    /// Returns current MIDI marker
    const char* GetCurrentMarker() const;

    /// Returns the channel of the first channel MIDI message on the track _trk_. MIDI files have every track
    /// assigned to a channel, so you can guess all other messages are in the returned channel.
    /// The range is 0 ... 15, or -1 if there aren't channel messages on the track
    int FindFirstChannelOnTrack ( int trk );

    /// Upgrades internal status when the MIDIMultiTrack is changed.
    /// If you edit the MIDI content stored in the MIDIMultiTrack (insert, delete or change messages), you must
    /// call this before calling any class method, to update internal status.
    void SetChanged();

    /* NOTE BY NC: abandoned feature
    bool IsChainMode() const
    {
        return chain_mode;
    }
    */

/* note by NC: these are now protected:
 * rearranged and modified the order of attributes: in particular notifier and driver are now pointers
 * to allow polimorphysm
 */

protected:
    static const int MEASURES_PER_WARP = 4;         ///< Used internally by ExtractWarpPositions() method
    static const int DEFAULT_CLK_PER_BEAT = 120;    ///< The default clock per beat rate

    /// Opens the hardware MIDI in and out ports. Currently ports are opened by the constructor
    /// and closed by the destructor, to give the ability of MIDI thru.
    /// \param in_port, out_port the internal OS id number of the port; if the port is already open
    /// the function does nothing
    /// \param timer_resolution required by the OS, you can leave default
    bool OpenMIDI ( int in_port, int out_port, int timer_resolution = MIDIDriver::DEFAULT_TIMER_RESOLUTION );

    /// Closes the open MIDI ports
    void CloseMIDI();

    /// Saves the sequencer status every DEFAULT_WARP_MEASURES (curreently 4) measures in a C++ vector.
    /// Used internally for speeding time jumping
    void ExtractWarpPositions();

    /// Calls CatchEventsBefore for every track
    void CatchEventsBefore();   /* NEW BY NC */

    /// Examines previous events, adjusting patch, controls, wheels and sysex according to them.
    /// Used when sequencer starts playing in the middle of a song
    void CatchEventsBefore( int trk );


    MIDIDriver* driver;                 ///< The MIDIDriver that sends messages to the hardware ports
    MIDISequencerGUIEventNotifier* notifier;    ///< the MIDISequencerGUIEventNotifier (if it is 0, no notifying)

    // NOTE by NC: these are now pointers to allow polimorphysm
    MIDIMultiTrack* tracks;             ///< The MIDIMultiTrack that holds MIDI data
    MIDISequencer* seq;                 ///< The MIDISequencer
    MIDIManager* mgr;                   ///< The MIDIManager that handles playing

    MIDIMultiProcessor thru_processor;              ///< The thru processor
    MIDIProcessorTransposer thru_transposer;        ///< The thru transposer
    MIDIProcessorRechannelizer thru_rechannelizer;  ///< The rechannelizer

    //MIDIClockTime marker_times[1024];
    //int num_markers;

    int num_measures;                   ///< the number of measures of the song

    long repeat_start_measure;          ///< loop start measure
    long repeat_end_measure;            ///< loop end measure
    bool repeat_play_mode;              ///< loop on/off

    std::vector<MIDISequencerState> warp_positions; ///< internal vector of MIDISequencerState for fast time jumping
    /* NOTE by NC: I realized that in an editing contest an array of pointers wasted much time
     * allocating and deallocating memory: so it's now a vector of objects
     */

    bool file_loaded;                   ///< true if the MIDIMultiTrack is not empty
    /* NOTE by NC: I mantained this name, but its meaning is now different:
     * it is nonzero if tracks is nonempty (maybe by an user editing)
     */

    int in_port;                        ///< the OS id of the open MIDI in port
    int out_port;                       ///< the OS id of the open MIDI out port

    // bool chain_mode; OLD
    // NOTE BY NC: I abandoned this feature, commenting every line referring to it
private:
    enum { CTOR_1, CTOR_2, CTOR_3 };
    int ctor_type;
};

}

#endif
