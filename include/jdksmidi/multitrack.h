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
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

#ifndef JDKSMIDI_MULTITRACK_H
#define JDKSMIDI_MULTITRACK_H

#include "jdksmidi/track.h"

namespace jdksmidi
{

class MIDIMultiTrack;
class MIDIMultiTrackIteratorState;
class MIDIMultiTrackIterator;

class MIDIMultiTrack
{
private:

    // delete old multitrack, construct new
    bool CreateObject ( int num_tracks_, bool deletable_ );

public:

    MIDIMultiTrack ( int max_num_tracks_ = 64, bool deletable_ = true );
    virtual ~MIDIMultiTrack();

    void SetTrack ( int track_num, MIDITrack *track )
    {
        tracks[track_num] = track;
    }

    MIDITrack *GetTrack ( int track_num )
    {
        return tracks[track_num];
    }
    const MIDITrack *GetTrack ( int track_num ) const
    {
        return tracks[track_num];
    }

    int GetNumTracks() const
    {
        return number_of_tracks;
    }

    // return number of tracks with events, last tracks have no events
    int GetNumTracksWithEvents() const;

    // test and sort events temporal order in all tracks
    void SortEventsOrder();

    // delete all tracks and remake multitrack with new amount of empty tracks
    bool ClearAndResize ( int num_tracks );

    // store src track and remake multitrack object with 17 tracks (src track can be a member of multitrack obiect),
    // move src track channel events to tracks 1-16, and all other types of events to track 0
    bool AssignEventsToTracks ( const MIDITrack *src );

    // the same as previous, but argument is track number of multitrack object himself
    bool AssignEventsToTracks ( int track_num = 0 )
    {
        return AssignEventsToTracks( GetTrack( track_num ) );
    }

    void Clear();

    int GetClksPerBeat() const
    {
        return clks_per_beat;
    }
    void SetClksPerBeat ( int cpb )
    {
        clks_per_beat = cpb;
    }

    int GetNumEvents() const
    {
        int num_events = 0;
        for ( int i = 0; i < number_of_tracks; ++i )
            num_events += tracks[i]->GetNumEvents();
        return num_events;
    }

protected:

    MIDITrack **tracks;
    int number_of_tracks;
    bool deletable;

    int clks_per_beat;
};

class MIDIMultiTrackIteratorState
{
public:

    MIDIMultiTrackIteratorState ( int num_tracks_ = 64 );
    MIDIMultiTrackIteratorState ( const MIDIMultiTrackIteratorState &m );
    virtual ~MIDIMultiTrackIteratorState();

    const MIDIMultiTrackIteratorState & operator = ( const MIDIMultiTrackIteratorState &m );

    int GetNumTracks() const
    {
        return num_tracks;
    }
    int GetCurEventTrack() const
    {
        return cur_event_track;
    }
    MIDIClockTime GetCurrentTime() const
    {
        return cur_time;
    }

    void Reset();
    int FindTrackOfFirstEvent();

    MIDIClockTime cur_time;
    int cur_event_track;
    int num_tracks;
    int *next_event_number;
    MIDIClockTime *next_event_time;
};

class MIDIMultiTrackIterator
{
public:

    MIDIMultiTrackIterator ( const MIDIMultiTrack *mlt );
    virtual ~MIDIMultiTrackIterator();


    void GoToTime ( MIDIClockTime time );

    bool GetCurEventTime ( MIDIClockTime *t ) const;
    bool GetCurEvent ( int *track, const MIDITimedBigMessage **msg ) const;
    bool GoToNextEvent();

    bool GoToNextEventOnTrack ( int track );

    const MIDIMultiTrackIteratorState &GetState() const
    {
        return state;
    }

    MIDIMultiTrackIteratorState &GetState()
    {
        return state;
    }

    void SetState ( const MIDIMultiTrackIteratorState &s )
    {
        state = s;
    }

    const MIDIMultiTrack * GetMultiTrack() const
    {
        return multitrack;
    }

protected:

    const MIDIMultiTrack *multitrack;
    MIDIMultiTrackIteratorState state;
};

}

#endif


