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
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

//
// MODIFIED by N. Cassetta
// search /* NC */ for modifies
//

#ifndef JDKSMIDI_SEQUENCER_H
#define JDKSMIDI_SEQUENCER_H

#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/tempo.h"
#include "jdksmidi/matrix.h"
#include "jdksmidi/process.h"

namespace jdksmidi
{

class MIDISequencerGUIEvent;
class MIDISequencerGUIEventNotifier;
class MIDISequencerTrackState;
class MIDISequencer;

class MIDISequencerGUIEvent
{
public:
    MIDISequencerGUIEvent()
    {
        bits = 0;
    }

    MIDISequencerGUIEvent ( unsigned long bits_ )
        : bits ( bits_ )
    {
    }

    MIDISequencerGUIEvent ( const MIDISequencerGUIEvent &e )
        : bits ( e.bits )
    {
    }

    MIDISequencerGUIEvent ( int group, int subgroup, int item = 0 )
    {
        bits = ( ( group & 0xff ) << 24 )
               | ( ( subgroup & 0xfff ) << 12 )
               | ( ( item & 0xfff ) << 0 );
    }

    operator unsigned long () const
    {
        return bits;
    }

    void SetEvent ( int group, int subgroup = 0, int item = 0 )
    {
        bits = ( ( group & 0xff ) << 24 )
               | ( ( subgroup & 0xfff ) << 12 )
               | ( ( item & 0xfff ) << 0 );
    }

    int GetEventGroup() const
    {
        return ( int ) ( ( bits >> 24 ) & 0xff );
    }

    int GetEventSubGroup() const
    {
        return ( int ) ( ( bits >> 12 ) & 0xfff );
    }

    int GetEventItem() const
    {
        return ( int ) ( ( bits >> 0 ) & 0xfff );
    }

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
        GROUP_CONDUCTOR_TIMESIG,
        GROUP_CONDUCTOR_KEYSIG,             /* NC */    // added two items to conductor group
        GROUP_CONDUCTOR_MARKER              /* NC */
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

    virtual void Notify ( const MIDISequencer *seq, MIDISequencerGUIEvent e ) = 0;
    virtual bool GetEnable() const = 0;
    virtual void SetEnable ( bool f ) = 0;
};


class MIDISequencerGUIEventNotifierText :
    public MIDISequencerGUIEventNotifier
{
public:
    MIDISequencerGUIEventNotifierText ( FILE *f );

    virtual ~MIDISequencerGUIEventNotifierText();

    virtual void Notify ( const MIDISequencer *seq, MIDISequencerGUIEvent e );
    virtual bool GetEnable() const;
    virtual void SetEnable ( bool f );

private:

    FILE *f;
    bool en;
};

class MIDISequencerTrackNotifier : public MIDIProcessor
{
public:
    MIDISequencerTrackNotifier (
        const MIDISequencer *seq_,
        int trk,
        MIDISequencerGUIEventNotifier *n
    );

    virtual ~MIDISequencerTrackNotifier();

    void SetNotifier (
        MIDISequencer *seq_,
        int trk,
        MIDISequencerGUIEventNotifier *n
    )
    {
        seq = seq_;
        track_num = trk;
        notifier = n;
    }


    void Notify ( int item );
    void NotifyConductor ( int item );      // NOTE by NC: this is now unneeded: could be eliminated

private:
    const MIDISequencer *seq;
    int track_num;
    MIDISequencerGUIEventNotifier *notifier;
};

class MIDISequencerTrackProcessor : public MIDIProcessor
{
public:
    MIDISequencerTrackProcessor();
    virtual ~MIDISequencerTrackProcessor();

    virtual void Reset();
    virtual bool Process ( MIDITimedBigMessage *msg );

    bool mute;     // track is muted
    bool solo;     // track is solod
    int velocity_scale;   // current velocity scale value for note ons, 100=normal
    int rechannel;    // rechannelization value, or -1 for none
    int transpose;    // amount to transpose note values

    MIDIProcessor *extra_proc; // extra midi processing for this track
};

class MIDISequencerTrackState : public MIDISequencerTrackNotifier
{
public:

    MIDISequencerTrackState (
        const MIDISequencer *seq_,
        int trk,
        MIDISequencerGUIEventNotifier *n
    );
    virtual ~MIDISequencerTrackState();

    virtual void GoToZero();
    virtual void Reset();
    virtual bool Process ( MIDITimedBigMessage *msg );

    //float tempobpm;    // current tempo in beats per minute               /* NC */    // moved to MIDISequencerState
    int pg;      // current program change, or -1
    int volume;     // current volume controller value
    //int timesig_numerator;  // numerator of current time signature        /* NC */    // moved to MIDISequencerState
    //int timesig_denominator; // denominator of current time signature     /* NC */    // moved to MIDISequencerState
    int bender_value;   // last seen bender value
    char track_name[256];  // track name
    bool got_good_track_name; // true if we dont want to use generic text events for track name

    bool notes_are_on;   // true if there are any notes currently on
    MIDIMatrix note_matrix;  // to keep track of all notes on
};


class MIDISequencerState
{
public:
    MIDISequencerState ( const MIDISequencer *s,
                         const MIDIMultiTrack *multitrack_,
                         MIDISequencerGUIEventNotifier *n );

    MIDISequencerState ( const MIDISequencerState &s );
    ~MIDISequencerState();

    const MIDISequencerState & operator = ( const MIDISequencerState &s );

    void Reset();                                       /* NC */    // new
    bool Process( MIDITimedBigMessage* msg );           /* NC */    // new
    void Notify( int group, int item = 0 );             /* NC */    // new

    MIDISequencerGUIEventNotifier *notifier;
    const MIDISequencer* seq;                           /* NC for notifying */
    const MIDIMultiTrack *multitrack;
    int num_tracks;

    MIDISequencerTrackState *track_state[64];
    MIDIMultiTrackIterator iterator;
    MIDIClockTime cur_clock;
    float cur_time_ms;
    int cur_beat;
    int cur_measure;
    MIDIClockTime next_beat_time;
    float tempobpm;           // current tempo in beats per minute      /* NC */ moved from MIDISequencerTrackState
    char timesig_numerator;  // numerator of current time signature     /* NC */ moved from MIDISequencerTrackState
    char timesig_denominator;// denominator of current time signature   /* NC */ moved from MIDISequencerTrackState
    char keysig_sharpflat;   // current key signature accidents         /* NC */ new
    char keysig_mode;        // major/minor                             /* NC */ new
    char marker_name[40];    //current marker name                      /* NC */ new
    int last_event_track;   // used internally by Process()             /* NC */ new
};

class MIDISequencer
{
public:

    MIDISequencer (
        const MIDIMultiTrack *m,
        MIDISequencerGUIEventNotifier *n = 0
    );

    virtual ~MIDISequencer();

    void ResetTrack ( int trk );
    void ResetAllTracks();

    MIDIClockTime GetCurrentMIDIClockTime() const;
    double GetCurrentTimeInMs() const;
    int GetCurrentBeat() const;
    int GetCurrentMeasure() const;

    double GetCurrentTempoScale() const;
    double GetCurrentTempo() const;

    MIDISequencerState *GetState();
    const MIDISequencerState *GetState() const;

    void SetState ( MIDISequencerState * );

    MIDISequencerTrackState * GetTrackState ( int trk );
    const MIDISequencerTrackState * GetTrackState ( int trk ) const;

    MIDISequencerTrackProcessor * GetTrackProcessor ( int trk );
    const MIDISequencerTrackProcessor * GetTrackProcessor ( int trk ) const;

    int GetNumTracks() const
    {
        return state.num_tracks;
    }

    bool GetSoloMode() const;

    void SetCurrentTempoScale ( float scale );
    void SetSoloMode ( bool m, int trk = -1 );

    void GoToZero();
    bool GoToTime ( MIDIClockTime time_clk );
    bool GoToTimeMs ( float time_ms );
    bool GoToMeasure ( int measure, int beat = 0 );

    bool GetNextEventTimeMs ( float *t );
    bool GetNextEventTimeMs ( double *t );
    bool GetNextEventTime ( MIDIClockTime *t );
    bool GetNextEvent ( int *tracknum, MIDITimedBigMessage *msg );

    void ScanEventsAtThisTime();

    // end of music is the time of last not end of track midi event!
    double GetMisicDurationInSeconds();

protected:

    MIDITimedBigMessage beat_marker_msg;

    bool solo_mode;
    int tempo_scale;

    int num_tracks;
    MIDISequencerTrackProcessor *track_processors[64];

    MIDISequencerState state;
} ;

}

#endif
