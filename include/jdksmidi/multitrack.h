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

//
// MODIFIED by N. Cassetta  ncassetta@tiscali.it
// search /* NC */ for modifies
//

#ifndef JDKSMIDI_MULTITRACK_H
#define JDKSMIDI_MULTITRACK_H

#include "jdksmidi/track.h"

namespace jdksmidi
{

class MIDIMultiTrack;
class MIDIMultiTrackIteratorState;
class MIDIMultiTrackIterator;


///
/// This class holds an array of pointers to MIDITrack objects to be played simultaneously. Every track contains
/// MIDITimedBigMessage objects representing MIDI events, and all tracks share the same timing (i.e. the events are
/// temporized according to the same MIDI clock per beat). Tipically track 0 is the master track and contains only
/// non-channel MIDI events (time, tempo, markers ...) while  other tracks contain the channel events.
/// You need to embed this into a MIDISequencer for playing the tracks. Moreover, a MIDIMultiTrackIterator class
/// is supplied for moving along events in temporal order regardless the track number
///

class MIDIMultiTrack
{
private:

    // delete old multitrack, construct new
    bool CreateObject ( int num_tracks_, bool deletable_ );

public:

    /// The constructor. If deletable_ = true it allocates memory for the tracks and deletes it in the destructor,
    /// otherwise the user must manually assign tracks to the array.
    MIDIMultiTrack ( int max_num_tracks_ = 64, bool deletable_ = true );

    /// The destructor frees the track pointers if they were allocated by the constructor.
    virtual ~MIDIMultiTrack();

    /// Assigns the MIDITrack pointed by _track_ to the _track_num_ position in the array. You may want to use
    /// this function if you want manually assign already initialized MIDITracks objects to the track numbers
    /// (in this case, construct the MIDIMultiTrack with the _deletable__ parameter = _false_, and it will not
    /// own its tracks). If you don't need this, the constructor with default parameter will give you all the
    /// tracks already allocated.
    /// \warning This function doesn't check if the previously assigned track was allocated, and eventually
    /// doesn't free its memory.
    void SetTrack ( int track_num, MIDITrack *track )
    {
        tracks[track_num] = track;
    }

    /// Returns a pointer to the track _track_num_
    MIDITrack *GetTrack ( int track_num )
    {
        // assert( track_num < number_of_tracks );
        return tracks[track_num];
    }
    const MIDITrack *GetTrack ( int track_num ) const
    {
        assert( track_num < number_of_tracks );
        return tracks[track_num];
    }

    /// Returns the number of allocated tracks (this is the maximum number of tracks, and not the number of
    /// currently used tracks).
    int GetNumTracks() const
    {
        return number_of_tracks;
    }

    /// Returns the number of tracks with events. This is probably the number of tracks effectively used by
    /// your MIDIMultiTrack.
    int GetNumTracksWithEvents() const;

    /// Test and sort events temporal order in all tracks.
    void SortEventsOrder();

    /// Delete all tracks and remake the MIDIMultiTrack with _num_tracks_ empty tracks.
    bool ClearAndResize ( int num_tracks );

    /// This function is useful in dealing with MIDI format 0 files (with all events in an unique track). It remake
    /// the MIDIMultiTrack object with 17 tracks (src track can be a member of multitrack obiect), moves _src_
    /// track channel events to tracks 1-16 according their channel, and all other types of events to track 0
    bool AssignEventsToTracks ( const MIDITrack *src );

    /// The same as previous, but argument is track number of multitrack object himself
    bool AssignEventsToTracks ( int track_num = 0 )
    {
        return AssignEventsToTracks( GetTrack( track_num ) );
    }

    /// Erases all events from the tracks, leaving them empty.
    void Clear();

    /// Returns the MIDI clocks per beat of all tracks (i.e.\ the number of MIDI ticks in a quarter note).
    int GetClksPerBeat() const
    {
        return clks_per_beat;
    }

    /// Sets the MIDI clocks per beat of all tracks (i.e.\ the number of MIDI ticks in a quarter note).
    /// \warning Currently this function only sets the multitrack internal parameter, and doesn't
    /// perform any time conversion on the MIDITrack objects: so its use is limited to an empty multitrack.
    void SetClksPerBeat ( int cpb )
    {
        clks_per_beat = cpb;
    }

    /// Gets the total number of MIDI events in the multitrack.
    int GetNumEvents() const
    {
        int num_events = 0;
        for ( int i = 0; i < number_of_tracks; ++i )
            num_events += tracks[i]->GetNumEvents();
        return num_events;
    }

    /* NEW BY NC */

    /// Returns **true** if track _n_ is allocated.
    bool IsValidTrackNum ( int n )
    {
        return ( n >= 0 && n < number_of_tracks && tracks[n] != 0 );
        // the latter may happen if !deletable;
    }

    /// Inserts the event _msg_ in the track _trk_. See MIDITrack::InsertEvent() for details.
    bool InsertEvent( int trk,  const MIDITimedBigMessage& msg, int _ins_mode = INSMODE_DEFAULT );

    /// Inserts a Note On and a Note Off event into the track. See MIDITrack::InsertNote() for details.
    bool InsertNote( int trk, const MIDITimedBigMessage& msg, MIDIClockTime len, int _ins_mode = INSMODE_DEFAULT );

    /// Deletes the event _msg_ from the track _trk_. See MIDITrack::DeleteEvent() for details.
    bool DeleteEvent( int trk,  const MIDITimedBigMessage& msg );

    /// Deletes the note _msg_ (_msg_ must be a Note On) from the track _trk_. See MIDITrack::DeleteNote() for details.
    bool DeleteNote( int trk, const MIDITimedBigMessage& msg );

    /* END OF NEW BY NC */

protected:

    MIDITrack **tracks;                     ///< The array of pointers to the MIDITrack objects
    int number_of_tracks;                   ///< The number of allocated tracks
    bool deletable;                         ///< If **true** tracks are owned by the multitrack

    int clks_per_beat;                      ///< The common clock per beat timing parameter
};


/// This class is used by the MIDIMultiTrackIterator to keep track of the current state of the iterator. You
/// usually don't need to deal with it, and the only useful thing is getting and restoring a state for faster
/// processing (see MIDIMultiTrackk::SetStatus()).

class MIDIMultiTrackIteratorState
{
public:

    /// The constructor creates a MIDIMultiTrackIteratorState with a given number of tracks.
    MIDIMultiTrackIteratorState ( int num_tracks_ = 64 );

    /// The copy constructor.
    MIDIMultiTrackIteratorState ( const MIDIMultiTrackIteratorState &m );

    /// The destructor.
    virtual ~MIDIMultiTrackIteratorState();

    /// The equal operator
    const MIDIMultiTrackIteratorState & operator = ( const MIDIMultiTrackIteratorState &m );

    /// Returns the numver of tracks
    int GetNumTracks() const
    {
        return num_tracks;
    }

    /// Returns the track of current event
    int GetCurEventTrack() const
    {
        return cur_event_track;
    }

    /// Returns the current time
    MIDIClockTime GetCurrentTime() const
    {
        return cur_time;
    }

    /// Resets the state at the time 0.
    void Reset();

    /// Finds the tracks of the first event. Used internally by the MIDIMultiTrackIterator
    int FindTrackOfFirstEvent();

    MIDIClockTime cur_time;                     ///< The current time
    int cur_event_track;                        ///< The track of current event
    int num_tracks;                             ///< The number of tracks
    int *next_event_number;                     ///< An array of integers (the number of next event in every track)
    MIDIClockTime *next_event_time;             ///< An array of MIDIClockTime (the time of the next event in every track)
};

///
/// This class is a forward iterator for moving along a MIDIMultiTrack. It defines a current event (initially
/// the first event), and you can get it, move to the next event (in temporal order) in the multitrack, or move
/// to the first event with a given time. When the iterator reaches the end of the multitrack the current event
/// become undefined, and the get methods will return **false**.
///

class MIDIMultiTrackIterator
{
public:

    /// The constructor.
    MIDIMultiTrackIterator ( const MIDIMultiTrack *mlt );

    /// The destructor.
    virtual ~MIDIMultiTrackIterator();

    /// Sets as current the first event with time equal to _time_ (or less, if there aren't events at the
    /// given time).
    void GoToTime ( MIDIClockTime time );

    /// Gets the MIDI time of the current event.
    /// \return **true** if there is effectively a current event, **false** if we are at the end of the multitrack.
    bool GetCurEventTime ( MIDIClockTime *t ) const;

    /// Gets the current event, returning its track and a pointer to it.
    /// \param [out] *track the track of the current event
    /// \param [out] **msg a pointer to the current event
    /// \return **true** if there is effectively a current event, **false** if we are at the end of the multitrack
    /// \warning this function doesn't move the iterator to the next event; you must call GoToNextEvent().
    bool GetCurEvent ( int *track, const MIDITimedBigMessage **msg ) const;

    /// Moves the iterator to the next event.
    /// \return **true** if the operation was successful (the current event was defined and wasn't the last
    /// event), **false** otherwise.
    bool GoToNextEvent();

    /// Moves the iterator to the next event on track _track_.
    /// \return **true** if the operation was successful (the current event was defined and wasn't the last
    /// event on the track), **false** otherwise.
    bool GoToNextEventOnTrack ( int track );

    //@{
    /// Gets the state of the iterator (see MultiTrackIteratorState).
    const MIDIMultiTrackIteratorState &GetState() const
    {
        return state;
    }

    MIDIMultiTrackIteratorState &GetState()
    {
        return state;
    }
    //@}

    /// Sets the state of the iterator (see MultiTrackIteratorState). If you want perform something with
    /// the iterator and then return to the initial situation you can save your state with GetState(), do
    /// what you want and then restore the saved state with this.
    void SetState ( const MIDIMultiTrackIteratorState &s )
    {
        state = s;
    }

    /// Gets the associated MIDIMultiTrack.
    const MIDIMultiTrack * GetMultiTrack() const
    {
        return multitrack;
    }

protected:

    const MIDIMultiTrack *multitrack;           ///< The associated multitrack
    MIDIMultiTrackIteratorState state;          ///< The state of the iterator
};

}

#endif


