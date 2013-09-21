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
// MODIFIED by N. Cassetta
//


#ifndef JDKSMIDI_ADVANCEDSEQUENCER_H
#define JDKSMIDI_ADVANCEDSEQUENCER_H

#include "jdksmidi/world.h"     // ISSUE: (NC) this XXXXX
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/driver.h"

#ifdef WIN32
#include "jdksmidi/driverwin32.h"
#endif // WIN32



#include <string>
#include <vector>

// #define MAX_WARP_POSITIONS (128)
// #define MEASURES_PER_WARP (4)    NC this is now a static const class attribute

namespace jdksmidi
{

class AdvancedSequencer
{
public:
    // AdvancedSequencer(); OLD
    // new by NC: the user can now set his own notifier (text, GUI, or no notifier ...)
    AdvancedSequencer(MIDISequencerGUIEventNotifier *n = 0);
    virtual ~AdvancedSequencer();

    // bool OpenMIDI ( int in_port, int out_port, int timer_resolution = DEFAULT_TIMER_RESOLUTION );
    // void CloseMIDI();
    // NOTE by NC: these are now protected

    void SetOutputPort( int p);
    int GetOutputPort() const;
    void SetInputPort( int p);
    int GetInputPort() const;
    /* NOTE BY NC: these allow the user to set/get the MIDI device input (for thru) and output port
     * of the sequencer: for now only the same port for all tracks: multi port in project!
     */

    void SetMIDIThruEnable ( bool f );
    bool GetMIDIThruEnable() const;

    void SetMIDIThruChannel ( int chan );
    int GetMIDIThruChannel() const;

    void SetMIDIThruTranspose ( int val );
    int GetMIDIThruTranspose() const;

    bool Load ( const char *fname );
    void UnLoad ();     /* NEW BY NC */
    void Reset();

    /* NEW BY NC */
    MIDIMultiTrack& GetMultiTrack();
    const MIDIMultiTrack& GetMultiTrack() const;
    MIDIMultiTrack* GetMultiTrackAddress();
    const MIDIMultiTrack* GetMUltiTrackAddress() const;
    /* NOTE by NC: these give access to the internal MIDIMultiTrack, allowing the user
     * to edit (and saving) it. When a change is done in the MIDIMultiTrack, user
     * must call SetMltChanged() to mantain internal coherence. This also assumes the
     * AdvancedSequencer is loaded
     */


    void GoToZero();    /* NEW by NC */
    void GoToMeasure ( int measure, int beat = 0 );
    void GoToTime ( MIDIClockTime t );
    // void Play ( int clock_offset = 0 );
    void Play();        // NC: always plays from current position
    // void RepeatPlay ( bool enable, int start_measure, int end_measure );
    // renamed SetRepeatPlay: this doesn't trigger Play but only sets parameters

    // void Pause();    unused: the same as Stop
    void Stop();
    void OutputMessage(MIDITimedBigMessage &msg);  /* NEW BY NC */
    // Send a message to the driver: useful if you want to change MIDI values during playback

    void SetRepeatPlay ( bool enable, int start_measure, int end_measure );

    bool IsPlay()
    {
        return mgr.IsSeqPlay();
    }

    void SoloTrack ( int trk );
    void UnSoloTrack();
    bool GetTrackSolo( int trk );         // NEW BY NC */

    void SetTrackMute ( int trk, bool f );
    bool GetTrackMute( int trk );         // NEW BY NC */
    void UnmuteAllTracks();

    void SetTempoScale ( double scale );
    double GetTempoWithoutScale() const;
    double GetTempoWithScale() const;
    unsigned long GetCurrentTimeInMs() const; /* NEW BY NC */
    int GetClksPerBeat() const; /* NEW BY NC */
    int GetNumTracks() const;
    int GetNumMeasures() const;

    int GetMeasure() const;
    int GetBeat() const;

    int GetTimeSigNumerator() const;
    int GetTimeSigDenominator() const;

    int GetTrackNoteCount ( int trk ) const;
    const char *GetTrackName ( int trk ) const;
    int GetTrackVolume ( int trk ) const;
    int GetTrackProgram ( int trk ) const;      /* NEW BY NC */

    void SetTrackVelocityScale ( int trk, int scale );
    int GetTrackVelocityScale ( int trk ) const;

    void SetTrackRechannelize ( int trk, int chan );
    int GetTrackRechannelize ( int trk ) const;

    void SetTrackTranspose ( int trk, int trans );
    int GetTrackTranspose ( int trk ) const;

    // void ExtractMarkers ( std::vector< std::string > *list );
    // int GetCurrentMarker() const;
    /* NOTE BY NC: I think these could be abandoned: now the current marker is given by the
     * sequencer state. However, for now I mantain them commented and substitute the second with
     * this
     */
    const char* GetCurrentMarker() const;


    int FindFirstChannelOnTrack ( int trk );
    // void ExtractWarpPositions(); now protected

    void SetMltChanged();
    /* NEW by NC: see comment to GetMultiTrack() */

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

/* OLD
    MIDIMultiProcessor thru_processor;
    MIDIProcessorTransposer thru_transposer;
    MIDIProcessorRechannelizer thru_rechannelizer;

    MIDIDriverDump driver;

    MIDIMultiTrack tracks;


    MIDISequencerGUIEventNotifierText notifier;

    MIDISequencer seq;

    MIDIClockTime marker_times[1024];
    int num_markers;

    MIDIManager mgr;

    long repeat_start_measure;
    long repeat_end_measure;
    bool repeat_play_mode;

    int num_warp_positions;
    MIDISequencerState *warp_positions[MAX_WARP_POSITIONS];

    bool file_loaded;
    bool chain_mode;
*/
/* NEW BY NC */



protected:
    static const int MEASURES_PER_WARP = 4;
    static const int DEFAULT_TIMER_RESOLUTION = 20;

    bool OpenMIDI ( int in_port, int out_port, int timer_resolution = DEFAULT_TIMER_RESOLUTION );
    void CloseMIDI();
    void ExtractWarpPositions();
    void CatchEventsBefore();   /* NEW BY NC */
    void CatchEventsBefore( int trk );


    MIDIDriver* driver;
    MIDISequencerGUIEventNotifier* notifier;

    MIDIMultiTrack tracks;
    MIDISequencer seq;
    MIDIManager mgr;

    MIDIMultiProcessor thru_processor;
    MIDIProcessorTransposer thru_transposer;
    MIDIProcessorRechannelizer thru_rechannelizer;

    MIDIClockTime marker_times[1024];
    int num_markers;

    int num_measures;

    long repeat_start_measure;
    long repeat_end_measure;
    bool repeat_play_mode;

    std::vector<MIDISequencerState> warp_positions;
    /* NOTE by NC: I realized that in an editing contest an array of pointers wasted much time
     * allocating and deallocating memory: so it's now a vector of objects
     */

    bool file_loaded;
    /* NOTE by NC: I mantained this name, but its meaning is now different:
     * it is nonzero if tracks is nonempty (maybe by an user editing)
     */

    int in_port;
    int out_port;

    // bool chain_mode; OLD
    // NOTE BY NC: I abandoned this feature, commenting every line referring to it
};

}

#endif
