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
//


#ifndef JDKSMIDI_TRACK_H
#define JDKSMIDI_TRACK_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

namespace jdksmidi
{


/* NEW BY NC */

/// Defines the default behaviour of the methods MIDITrack::InsertEvent() and MIDITrack::InsertNote()
/// when inserting events.
/// If they are trying to insert an event into a track and find an equal or similar event at same MIDI time
/// (see MIDITimedBigMessage::IsSameKind()) they can replace it with the new event or insert it
/// without deleting the older. This is deternined by a static attribute of the class MIDITrack and can
/// be changed by the MIDITrack::SetInsertMode() method (the default is INSMODE_INSERT_OR_REPLACE).
/// When the above methods are called with default argoment *_ins_mode* they follow the default behaviour,
/// this can be overriden giving them one of the other values as last parameter.
enum
{
    INSMODE_DEFAULT,    ///< follow the default behaviour (only used as default argument in methods MIDITrack::InsertEvent() and MIDITrack::InsertNote()
    INSMODE_INSERT,     ///< always insert events, if a same kind event was found  duplicate it.
    INSMODE_REPLACE,    ///< replace if a same kind event was found, otherwise do nothing.
    INSMODE_INSERT_OR_REPLACE,          ///< replace if a same kind event was found, otherwise insert.
    INSMODE_INSERT_OR_REPLACE_BUT_NOTE  ///< as above, but allow two same note events at same time (don't replace, insert a new note).
};

/// Defines the behaviour of the method MIDITrack::FindEventNunber() when searching events.
enum
{
    COMPMODE_EQUAL,     ///< the method searches for an event matching equal operator.
    COMPMODE_SAMEKIND,  ///< the nethod searches for an event matching the MIDITimedBigMessage::IsSameKind() method.
    COMPMODE_TIME       ///< the method searches for the first event with time equal to the event time.
};

/* END OF NEW */


///
/// MIDITrackChunkSize is a constant which specifies how many events are in one MIDITrackChunk.
///

const int MIDITrackChunkSize = 512;


///
/// A MIDITrack's events are allocated in these chunks in order to avoid memory fragmentation
/// in embedded systems or other systems lacking an MMU. Every item within a MIDITrackChunk
/// object is a MIDITimedBigMessage. To avoid unnecessary copied of big events, access
/// to these events is only done via the GetEventAddress() method.
///

class  MIDITrackChunk
{
public:

    /// Returns the address of the MIDITimedBigMessage referred to by *event_num*
    /// @param event_num an integer specifying an event number in the range 0 to MIDITrackChunkSize
    /// @returns The const pointer to the requested event.
    const MIDITimedBigMessage * GetEventAddress ( int event_num ) const;

    /// Returns the address of the MIDITimedBigMessage referred to by *event_num*
    /// @param event_num an integer specifying an event number in the range 0 to MIDITrackChunkSize
    /// @returns The non-const pointer to the requested event.
    MIDITimedBigMessage * GetEventAddress ( int event_num );

protected:

private:
    MIDITimedBigMessage buf[MIDITrackChunkSize];
};

///
/// The MIDIChunksPerTrack constant specifies the maximum number of MIDITrackChunks that can be in one track.
/// The value MIDIChunksPerTrack * MIDITrackChunkSize is the total number of events.
/// This is a constant in order to avoid memory fragmentation in embedded systems or systems without an MMU
///

const int MIDIChunksPerTrack = 512;


///
/// The MIDITrack class is a container that manages an array of MIDIChunk objects and provides an
/// interface to the user that is useful for managing a list of MIDITimedBigMessages. It internally
/// stores MIDITimedBigMessage objects. There is a fixed maximum number of events that it can store,
/// which is defined by MIDIChunksPerTrack * MIDITrackChunkSize.
///

class  MIDITrack
{
public:

    /// Construct a MIDITrack object with the specified number of events
    /// @param size The number of events, defaults to 0
    MIDITrack ( int size = 0 );

    /// Copy Constructor for a MIDITrack object
    /// @param t The reference to the MIDITrack object to copy
    MIDITrack ( const MIDITrack &t );

    /// The MIDITrack Destructor, frees all chunks and referenced MIDITimedBigMessage's
    ~MIDITrack();

    /// Sets the number of active events in the track to 0 and frees any any unused MIDITrackChunk
    /// object and associated MIDITimedBigMessage events.
    void Clear();

    /// Frees any unused MIDITrackChunk objects and associated MIDITimedBigMessage events.
    /// @note the memory is autonomally managed by the MIDITrack, so this have no utility for
    /// the user and could become protected in the future.
    void Shrink();

    /// Allows you to merge the events in two separate tracks into a third track.
    /// @param src1 Pointer to first track
    /// @param src2 Pointer to second track
    /// ClearAndMerge() assumes all events in both tracks are already ordered by time.
    void ClearAndMerge ( const MIDITrack *src1, const MIDITrack *src2 );

    /* NEW BY NC */

    /// Sets the default behaviour for the methods InsertEvent() and InsertNote(). This can be overriden
    /// by them by mean of the last (default) parameter.
    /// @param m one of @ref INSMODE_INSERT, @ref INSMODE_REPLACE, @ref INSMODE_INSERT_OR_REPLACE,
    /// @ref INSMODE_INSERT_OR_REPLACE_BUT_NOTE; default is INSMODE_INSERT_OR_REPLACE.
    /// @note @ref INSMODE_DEFAULT is used only in the insert methods (as default argument) and
    /// does nothing if given as parameter here.
    static void SetInsertMode( int m );

    /// Inserts a single event into the track. It could be used for imserting Note On and Note Off events,
    /// but this is better done by InsertNote() which inserts both with a single call. The method handles
    /// automatically the EndOfTrack message, inserting or moving it if needed, so you must not insert
    /// the EndOfTrack by yourself. It also determines the correct position of events with same MIDI time,
    /// using the MIDITimedBigMessage::CompareEventsForInsert() method for comparison.
    /// @param msg the event to insert.
    /// @param _ins_mode this determines the behaviour of the method if it finds an equal or similar event with
    /// same MIDI time in the track: it may replace the event or insert a new event anyway. If you leave the
    /// default parameter (INSMODE_DEFAULT) the method will follow the behaviour set by the static
    /// method SetInsertMode(), otherwise you may override it giving the last parameter. For details see
    /// @ref INSMODE_DEFAULT (default), @ref INSMODE_REPLACE, @ref INSMODE_INSERT_OR_REPLACE,
    /// @ref INSMODE_INSERT_OR_REPLACE_BUT_NOTE.
    /// @returns **false** in some situations in which the method cannot insert:
    /// + _msg_ was an EndOfTrack (you cannot insert it)
    /// + __ins_mode_ was INSMODE_REPLACE but there is no event to replace
    /// + a memory error occurred
    /// otherwise **true**.
    bool InsertEvent( const MIDITimedBigMessage& msg, int _ins_mode = INSMODE_DEFAULT );

    /// Inserts a Note On and a Note Off event into the track. Use this method for inserting note messages as
    /// InsertEvent() could be dangerous in some situations. It handles automatically the EndOfTrack message,
    /// inserting or moving it if needed, so you must not insert the EndOfTrack by yourself. It also determines
    /// the correct position of events with same MIDI time, using the MIDITimedBigMessage::CompareEventsForInsert()
    /// method for comparison (so a Note Off always preceed a Note On with same time).
    /// @param msg must be a Note On event.
    /// @param len the length of the note: the method will create a Note Off event and put it after _len_ MIDI
    /// clocks in the track.
    /// @param _ins_mode the same for InsertEvent() (when replacing a note, the method finds and deletes both
    /// the old Note On and Note Off events).
    /// @returns **false** in some situations in which the method cannot insert:
    /// + _msg_ was not a Note On event
    /// + _ins_mode_ was INSMODE_REPLACE but there is no event to replace
    /// + a memory error occurred
    /// otherwise **true**.
    /// @bug In the latter case the method could leave the track in an inconsistent state (a Note On without
    /// corresponding Note Off or viceversa).
    bool InsertNote( const MIDITimedBigMessage& msg, MIDIClockTime len, int _ins_mode = INSMODE_DEFAULT );

    /// Deletes an event from the track. Use DeleteNote() for safe deleting both Note On and Note Off. You cannot
    /// delete the EndOfTrack event.
    /// @param msg a copy of the event to delete.
    /// @returns **false** if an exact copy of the event was not found, or if a memory error occurred, otherwise **true**.
    bool DeleteEvent( const MIDITimedBigMessage& msg );

    /// Deletes a Note On and corresponding Note Off events from the track. Don't use DeleteEvent() for deleting
    /// notes.
    /// @param msg a copy of the Note On event to delete.
    /// @returns **false** if an exact copy of the event was not found, or if a memory error occurred, otherwise **true**.
    /// @bug In the latter case the method could leave the track in an inconsistent state (a Note On without
    /// corresponding Note Off or viceversa).
    bool DeleteNote( const MIDITimedBigMessage& msg );


    /* END OF NEW */


//  void  Sort();

    /// The equal operator
    const MIDITrack & operator = ( const MIDITrack & src );

    /// Increase the size of the internal buffer of *increase_amount* events.
    /// @note the size of the buffer is autonomally managed by the track, so this have no utility for the
    /// user and could become protected in the future.
    bool Expand ( int increase_amount = ( MIDITrackChunkSize ) );

    /// The same as GetEvent(). It's included only for compatibility with older versions and could be removed
    /// in the future.
    MIDITimedBigMessage * GetEventAddress ( int event_num );
    const MIDITimedBigMessage * GetEventAddress ( int event_num ) const;

    /// Returns a pointer to the *event_num* message stored in the track. If *event_num* is not a valid event
    /// number returns 0.
    MIDITimedBigMessage *GetEvent ( int event_num );
    const MIDITimedBigMessage *GetEvent ( int event_num ) const;

    /// Returns a pointer to the last event in the track. If the track is correctly termined it
    /// should be a EndOfData event. See IsTrackEnded().
    const MIDITimedBigMessage *GetLastEvent() const
    {
        return GetEvent( GetNumEvents() - 1 );
    }

    MIDITimedBigMessage *GetLastEvent()         // NEW BY NC: there was only the const version
    {
        return GetEvent( GetNumEvents() - 1 );
    }

    /// Returns the time of the last event in the track. If the track is correctly termined it
    /// should be a EndOfData event (see IsTrackEnded()). This is also the total time of the track.
    MIDIClockTime GetLastEventTime() const
    {
        const MIDITimedBigMessage *msg = GetLastEvent();
        return ( msg == 0 )? 0 : msg->GetTime();
    }

    /// Copies in *msg* the *event_num* message of the track. Returns **true** if *event_num* is a valid
    /// event number (if not, *msg* will be left unchanged).
    bool GetEvent ( int event_num, MIDITimedBigMessage *msg ) const;

    /// Copies the event *msg* at the place *n* in the track (default: as last event in the track).
    /// @note this is a low level function and may leave the track in an inconsistent state (for ex. with
    /// events after the EndOfData). You should use InsertEvent(), InsertNote() for safe editing.
    bool PutEvent ( const MIDITimedBigMessage &msg, int n = -1  );

    bool PutEvent ( const MIDIDeltaTimedMessage &msg )
    {
        return PutEvent ( MIDIDeltaTimedBigMessage (msg) );
    }

    bool PutEvent ( const MIDIDeltaTimedBigMessage &msg);

    // put event and clear msg, exclude its time, keep time unchanged!
    bool PutEvent2 ( MIDITimedBigMessage &msg );
    bool PutEvent ( const MIDITimedMessage &msg, const MIDISystemExclusive *sysex );
    bool SetEvent ( int event_num, const MIDITimedBigMessage &msg );

    // put text message with known length (w/o ending NULL), or evaluate it if zero length
    bool PutTextEvent ( MIDIClockTime time, int meta_event_type, const char *text, int length = 0 );

    /// Removes the event *event_num* from the track (it does nothing if *event_num* is not a valid event number).
    /// @note this is a low level function and may leave the track in an inconsistent state (for ex. with
    /// events after the EndOfData). You should use DeleteEvent(), DeleteNote() for safe editing.
    bool RemoveEvent ( int event_num );

    bool MakeEventNoOp ( int event_num );

    /* NEW BY NC */

    /// Finds an event in the track matching a given event.
    /// @param[in] msg the event to look for
    /// @param[out] event_num contains the event number in the track if the event was found; otherwise it contains
    /// **-1** if *event time* was invalid, or the number of the first event with the same event time.
    /// @param[in] _comp_mode (compare mode) an enum value with the following meaning.
    /// + @ref COMPMODE_EQUAL : the event must be equal to *msg*.
    /// + @ref COMPMODE_SAMEKIND : the event is a same kind event (see MIDITimedBigMessage::IsSameKind()).
    /// + @ref COMPMODE_TIME : the behaviour is the same of FindEventNumber(time, event_num).
    /// @returns **true** if an event with given time was found, *false* otherwise.
    bool FindEventNumber( const MIDITimedBigMessage& msg, int *event_num, int _comp_mode = COMPMODE_EQUAL) const;

    /// Finds the first event in the track with the given time.
    /// @param[in] time the time to look for.
    /// @param[out] event_num contains the event number in the track if an event was found; otherwise it contains
    /// **-1** if *time* was invalid, or the number of the last event before *time*.
    /// @returns **true** if an event with given time was found, **false** otherwise.
    bool FindEventNumber ( MIDIClockTime time, int *event_num ) const;

    /// Returns the length in MIDI clocks of the given note. _msg_ must be a Note On event present in the track
    /// (otherwise the function will return 0).
    MIDIClockTime NoteLength ( const MIDITimedBigMessage& msg ) const;

    /* END OF NEW */

    /// Returns the buffer size (the maximum number of events that can be stored in the track.
    /// @note the size of the buffer is managed autonomally by the track, so this have no utility for the
    /// user and could become protected in the future.
    int GetBufferSize() const
    {
        return buf_size;
    }

    /// Returns the number of events in the track.
    int GetNumEvents() const
    {
        return num_events;
    }

    /// Returns **true** if the event *event_num* exists in the track.
    bool IsValidEventNum( int event_num ) const
    {
        return ( 0 <= event_num && event_num < num_events );
    }

    /// Returns **true** if the track has no events.
    bool IsTrackEmpty() const
    {
        return num_events == 0;
    }

    /* NEW BY NC */
    /// Returns **true** if the track is termined by a correct EndOfData event.
    bool IsTrackEnded() const;

    /// Sets the end time of the track. If the track is not termined by an EndOfData event inserts it. Returns
    /// **true** if the operation was succesful (you can't set end time before any non EndOfTime event).
    bool SetEndTime( MIDIClockTime time );
    /* END OF NEW */

    /// Test events temporal order, return false if events out of order
    bool EventsOrderOK() const;

    /// Sort events temporal order
    void SortEventsOrder();

    // remove events with identical time and all other data, return number of such events
    int RemoveIdenticalEvents( int max_distance_between_identical_events = 32 );

    /// Cuts note and pedal events at the time t. All sounding notes and held pedals are truncated, the
    /// corresponding off events are properly managed.
    void CloseOpenEvents( MIDIClockTime t );

private:

// void  QSort( int left, int right );

    MIDITrackChunk * chunk[MIDIChunksPerTrack];

    int buf_size;
    int num_events;

    struct Event_time
    {
        int event_number;
        MIDIClockTime time;
        static bool less( Event_time t1, Event_time t2 )
        {
            return ( t1.time < t2.time );
        }
    };

    static int ins_mode;        /* NEW BY NC */

};

}

#endif

