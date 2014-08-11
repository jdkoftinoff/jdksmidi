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

#include "jdksmidi/world.h"
#include "jdksmidi/track.h"

#include "jdksmidi/matrix.h"

#ifndef DEBUG_MDTRACK
#define DEBUG_MDTRACK 0
#endif

#if DEBUG_MDTRACK
#undef DBG
#define DBG( a ) a
#endif

namespace jdksmidi
{

const MIDITimedBigMessage *MIDITrackChunk::GetEventAddress( int event_num ) const
{
    return &buf[event_num];
}

MIDITimedBigMessage *MIDITrackChunk::GetEventAddress( int event_num )
{
    return &buf[event_num];
}

int MIDITrack::ins_mode = INSMODE_INSERT_OR_REPLACE; /* NEW BY NC */

MIDITrack::MIDITrack( int size )
{
    buf_size = 0;
    num_events = 0;

    for ( int i = 0; i < MIDIChunksPerTrack; ++i )
        chunk[i] = 0;

    if ( size )
    {
        Expand( size );
    }
}

MIDITrack::MIDITrack( const MIDITrack &t )
{
    buf_size = 0;
    num_events = 0;

    for ( int i = 0; i < t.GetNumEvents(); ++i )
    {
        const MIDITimedBigMessage *src;
        src = t.GetEventAddress( i );
        PutEvent( *src ); // it execute Expand()
    }
}

MIDITrack::~MIDITrack()
{
    for ( int i = 0; i < buf_size / MIDITrackChunkSize; ++i )
    {
        jdks_safe_delete_object( chunk[i] );
    }
}

void MIDITrack::Clear()
{
    num_events = 0;
    Shrink(); // added by NC
}

/* NEW BY NC */
bool MIDITrack::IsTrackEnded() const
{
    if ( num_events == 0 )
    {
        return false;
    }
    if ( !GetLastEvent()->IsEndOfTrack() )
    {
        return false;
    }
    return true;
}

bool MIDITrack::SetEndTime( MIDIClockTime time )
{
    if ( IsTrackEnded() )
    {
        if ( num_events > 1 && GetEvent( num_events - 2 )->GetTime() > time )
        { // we are trying to insert an EOT before last event
            return false;
        }
        GetLastEvent()->SetTime( time );
    }
    else
    {
        if ( num_events > 0 && GetEvent( num_events - 1 )->GetTime() > time )
        { // as above
            return false;
        }
        MIDITimedBigMessage msg;
        msg.SetDataEnd();
        msg.SetTime( time );
        PutEvent( msg );
    }
    return true;
}

/* END OF NEW */

bool MIDITrack::EventsOrderOK() const
{
    if ( num_events < 2 )
        return true;

    MIDIClockTime time0 = GetEventAddress( 0 )->GetTime();

    for ( int i = 1; i < num_events; ++i )
    {
        MIDIClockTime time1 = GetEventAddress( i )->GetTime();
        if ( time0 > time1 )
            return false;
        time0 = time1;
    }

    return true;
}

void MIDITrack::SortEventsOrder()
{
    std::vector<Event_time> et( num_events );

    int n;
    for ( n = 0; n < num_events; ++n )
    {
        et[n].event_number = n;
        et[n].time = GetEventAddress( n )->GetTime();
    }

    std::stable_sort( et.begin(), et.end(), Event_time::less );

    MIDITrack trk( num_events );

    for ( n = 0; n < num_events; ++n )
    {
        int event_num = et[n].event_number;
        const MIDITimedBigMessage *src;
        src = GetEventAddress( event_num );
        trk.PutEvent( *src ); // add src event to trk
    }

    *this = trk;
}

int MIDITrack::RemoveIdenticalEvents( int max_distance_between_identical_events )
{
    int removed = 0;

    for ( int n = 0; n < num_events; ++n )
    {
        MIDITimedBigMessage *mn = GetEvent( n );

        for ( int i = 1; i < max_distance_between_identical_events; ++i )
        {
            if ( ( n + i ) >= num_events )
                break;

            MIDITimedBigMessage *mni = GetEvent( n + i );
            if ( *mn == *mni )
            {
                ++removed;
                MakeEventNoOp( n );
                break;
            }
        }
    }

    return removed;
}

const MIDITrack &MIDITrack::operator=( const MIDITrack &src )
{
    if ( num_events == src.num_events )
    {
        for ( int n = 0; n < num_events; ++n )
        {
            const MIDITimedBigMessage *msg = src.GetEventAddress( n );
            SetEvent( n, *msg );
        }
    }
    else
    {
        this->~MIDITrack();

        buf_size = 0;
        num_events = 0;

        for ( int i = 0; i < src.GetNumEvents(); ++i )
        {
            const MIDITimedBigMessage *msg = src.GetEventAddress( i );
            PutEvent( *msg ); // it execute Expand()
        }
    }

    return *this;
}

void MIDITrack::ClearAndMerge( const MIDITrack *src1, const MIDITrack *src2 )
{
    Clear();
    const MIDITimedBigMessage *ev1;
    int cur_trk1ev = 0;
    int num_trk1ev = src1->GetNumEvents();
    const MIDITimedBigMessage *ev2;
    int cur_trk2ev = 0;
    int num_trk2ev = src2->GetNumEvents();
    MIDIClockTime last_data_end_time = 0;

    while ( cur_trk1ev < num_trk1ev || cur_trk2ev < num_trk2ev )
    {
        // skip any NOPs on track 1
        ev1 = src1->GetEventAddress( cur_trk1ev );
        ev2 = src2->GetEventAddress( cur_trk2ev );
        bool has_ev1 = ( cur_trk1ev < num_trk1ev ) && ev1;
        bool has_ev2 = ( cur_trk2ev < num_trk2ev ) && ev2;

        if ( has_ev1 && ev1->IsNoOp() )
        {
            cur_trk1ev++;
            continue;
        }

        // skip any NOPs on track 2

        if ( has_ev2 && ev2->IsNoOp() )
        {
            cur_trk2ev++;
            continue;
        }

        // skip all data end

        if ( has_ev1 && ev1->IsDataEnd() )
        {
            if ( ev1->GetTime() > last_data_end_time )
            {
                last_data_end_time = ev1->GetTime();
            }

            cur_trk1ev++;
            continue;
        }

        if ( has_ev2 && ev2->IsDataEnd() )
        {
            if ( ev2->GetTime() > last_data_end_time )
            {
                last_data_end_time = ev2->GetTime();
            }

            cur_trk2ev++;
            continue;
        }

        if ( ( has_ev1 && !has_ev2 ) )
        {
            // nothing left on trk 2
            if ( !ev1->IsNoOp() )
            {
                if ( ev1->GetTime() > last_data_end_time )
                {
                    last_data_end_time = ev1->GetTime();
                }

                PutEvent( *ev1 );
                ++cur_trk1ev;
            }
        }

        else if ( ( !has_ev1 && has_ev2 ) )
        {
            // nothing left on trk 1
            if ( !ev2->IsNoOp() )
            {
                PutEvent( *ev2 );
                ++cur_trk2ev;
            }
        }

        else if ( has_ev1 && has_ev2 )
        {
            int trk = 1;

            if ( ( ev1->GetTime() <= ev2->GetTime() ) )
            {
                trk = 1;
            }

            else
            {
                trk = 2;
            }

            if ( trk == 1 )
            {
                if ( ev1->GetTime() > last_data_end_time )
                {
                    last_data_end_time = ev1->GetTime();
                }

                PutEvent( *ev1 );
                ++cur_trk1ev;
            }

            else
            {
                if ( ev2->GetTime() > last_data_end_time )
                {
                    last_data_end_time = ev2->GetTime();
                }

                PutEvent( *ev2 );
                ++cur_trk2ev;
            }
        }
    }

    // put single final data end event
    MIDITimedBigMessage dataend;
    dataend.SetTime( last_data_end_time );
    dataend.SetDataEnd();
    PutEvent( dataend );
}

/* NEW FUNCTIONS BY NC */

void MIDITrack::SetInsertMode( int m )
{
    if ( m != INSMODE_DEFAULT )
        ins_mode = m;
}

/* OLD
bool MIDITrack::InsertEvent(const MIDITimedBigMessage& msg) {
    if (msg.IsDataEnd()) return false;                  // DATA_END only auto managed

    if ( !IsTrackEnded() )                              // add DATA_END if needed
    {
        SetEndTime( GetLastEventTime() );
    }

    if (GetLastEventTime() <= msg.GetTime()) {          // insert as last event
        SetEndTime(msg.GetTime());                      // adjust DATA_END
        return PutEvent(msg, num_events-1);             // insert just before DATA_END
    }
                                                        // binary search
    int event_num;
    FindEventNumber(msg.GetTime(), &event_num);         // must return true

    while ( MIDITimedBigMessage::CompareEventsForInsert( msg, *GetEvent(event_num) ) == 1 )
        event_num++;
    return PutEvent( msg, event_num );
}
*/

bool MIDITrack::InsertEvent( const MIDITimedBigMessage &msg, int _ins_mode )
{
    bool ret = false;

    if ( msg.IsDataEnd() )
        return false; // DATA_END only auto managed

    if ( !IsTrackEnded() ) // add DATA_END if needed
    {
        SetEndTime( GetLastEventTime() );
    }

    if ( GetLastEventTime() < msg.GetTime() )
    {                                           // insert as last event
        SetEndTime( msg.GetTime() );            // adjust DATA_END
        return PutEvent( msg, num_events - 1 ); // insert just before DATA_END
    }

    int event_num;
    FindEventNumber( msg.GetTime(), &event_num ); // must return true
    int old_event_num = event_num;

    if ( _ins_mode == INSMODE_DEFAULT )
    {
        _ins_mode = ins_mode; // set inert mode to default behaviour
    }

    switch ( _ins_mode )
    {
    case INSMODE_INSERT: // always insert the event
        while ( MIDITimedBigMessage::CompareEventsForInsert( msg, *GetEvent( event_num ) ) == 1 )
            event_num++;
        ret = PutEvent( msg, event_num );
        break;

    case INSMODE_REPLACE: // replace a same kind event, or do nothing
        while ( event_num < num_events && GetEvent( event_num )->GetTime() == msg.GetTime() )
        { // search for a same kind event
            if ( MIDITimedBigMessage::IsSameKind( *GetEvent( event_num ), msg ) )
            {
                ret = SetEvent( event_num, msg ); // replace if found
                break;
            }
            event_num++;
        }
        break;

    case INSMODE_INSERT_OR_REPLACE:          // replace a same kind, or insert
    case INSMODE_INSERT_OR_REPLACE_BUT_NOTE: // (always insert notes)
        while ( event_num < num_events && GetEvent( event_num )->GetTime() == msg.GetTime() )
        {
            if ( MIDITimedBigMessage::IsSameKind( *GetEvent( event_num ), msg )
                 && ( _ins_mode == INSMODE_INSERT_OR_REPLACE || !msg.IsNote() ) )
            {
                ret = SetEvent( event_num, msg ); // replace if found
                break;
            }
            event_num++;
        }
        if ( event_num == num_events || msg.GetTime() != GetEvent( event_num )->GetTime() )
        {
            event_num = old_event_num;
            while ( MIDITimedBigMessage::CompareEventsForInsert( msg, *GetEvent( event_num ) ) == 1 )
                event_num++;
            ret = PutEvent( msg, event_num ); // else insert
        }
        break;

    default:
        break;
    }
    return ret;
}

bool MIDITrack::InsertNote( const MIDITimedBigMessage &msg, MIDIClockTime len, int _ins_mode )
{
    bool ret = false;
    if ( !msg.IsNoteOn() )
        return false;

    MIDITimedBigMessage msgoff( msg ); // set our NOTE_OFF message
    msgoff.SetType( NOTE_OFF );
    msgoff.SetTime( msg.GetTime() + len );

    int event_num;

    if ( _ins_mode == INSMODE_DEFAULT )
    {
        _ins_mode = ins_mode; // set insert mode to default behaviour
    }

    switch ( _ins_mode )
    {
    case INSMODE_INSERT: // always insert the event
    case INSMODE_INSERT_OR_REPLACE_BUT_NOTE:
        ret = InsertEvent( msg, _ins_mode );
        ret &= InsertEvent( msgoff, _ins_mode );
        break;

    case INSMODE_REPLACE: // replace a same kind event, or do nothing
        if ( FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
        {                                    // search for a note on event (with same note)
            RemoveEvent( event_num );        // remove it
            while ( event_num < num_events ) // search for the note off
            {
                MIDITimedBigMessage *msgp = GetEvent( event_num );
                if ( msgp->IsNoteOff() && msgp->GetNote() == msg.GetNote() )
                {
                    RemoveEvent( event_num ); // and remove
                    break;
                }
                event_num++;
            }
            ret = InsertEvent( msg, _ins_mode );     // insert note on
            ret &= InsertEvent( msgoff, _ins_mode ); // insert note off
        }
        break;

    case INSMODE_INSERT_OR_REPLACE: // replace a same kind, or insert
        if ( FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
        {                                    // search for a note on event (with same note)
            RemoveEvent( event_num );        // remove it
            while ( event_num < num_events ) // search for the note off
            {
                MIDITimedBigMessage *msgp = GetEvent( event_num );
                if ( msgp->IsNoteOff() && msgp->GetNote() == msg.GetNote() )
                {
                    RemoveEvent( event_num ); // and remove
                    break;
                }
                event_num++;
            }
        }
        ret = InsertEvent( msg, INSMODE_INSERT );     // insert note on
        ret &= InsertEvent( msgoff, INSMODE_INSERT ); // insert note off
        break;

    default:
        break;
    }
    // NOTE: this calls InsertEvent and RemoveEvent always with correct arguments, so they should return true
    // In the case of a memory allocation error, this would leave the track in an inconsistent state (notes erased
    // without corresponding note inserted). The solution would be a backup of the whole track at the beginning, but
    // this would be very expensive (for example in a MIDI format 0 file with all events in a track)
    return ret;
}

bool MIDITrack::DeleteEvent( const MIDITimedBigMessage &msg )
{
    if ( msg.IsDataEnd() )
    {
        return false;
    }
    int event_num;
    if ( !FindEventNumber( msg, &event_num ) )
    {
        return false;
    }
    return RemoveEvent( event_num );
}

bool MIDITrack::DeleteNote( const MIDITimedBigMessage &msg )
{
    bool ret = false;
    if ( !msg.IsNoteOn() )
        return false;

    int event_num;
    if ( !FindEventNumber( msg, &event_num ) )
    {
        return false;
    }
    ret = RemoveEvent( event_num );
    while ( event_num < num_events )
    {
        MIDITimedBigMessage *msgp = GetEvent( event_num );
        if ( msgp->IsNoteOff() && msgp->GetNote() == msg.GetNote() )
        {
            RemoveEvent( event_num );
            break;
        }
        event_num++;
    }
    return ret;
}

#if 0

bool  MIDITrack::Delete ( int start_event, int num )
{
    // TO DO: Delete
    return true;
}

void MIDITrack::QSort ( int left, int right )
{
    int i, j;
    MIDITimedBigMessage *x, y;
    i = left;
    j = right;
    // search for a non NOP message for our median
    int pos = ( left + right ) / 2;

    for ( ; pos <= right; ++pos )
    {
        x = GetEventAddress ( pos );

        if ( x && !x->IsNoOp() )
            break;
    }

    if ( GetEventAddress ( pos )->IsNoOp() )
    {
        for ( pos = ( left + right ) / 2; pos >= left; --pos )
        {
            x = GetEventAddress ( pos );

            if ( x && !x->IsNoOp() )
                break;
        }
    }

    if ( x && x->IsNoOp() )
        return;

    do
    {
        while ( MIDITimedMessage::CompareEvents ( *GetEventAddress ( i ), *x ) == 2 &&
                i < right )
            ++i;

        while ( MIDITimedMessage::CompareEvents ( *x, *GetEventAddress ( j ) ) == 2 &&
                j > left )
            --j;

        if ( i <= j )
        {
            y = *GetEventAddress ( i );
            *GetEventAddress ( i ) = *GetEventAddress ( j );
            *GetEventAddress ( j ) = y;
            ++i;
            --j;
        }
    }
    while ( i <= j );

    if ( left < j )
    {
        QSort ( left, j );
    }

    if ( i < right )
    {
        QSort ( i, right );
    }
}


void MIDITrack::Sort()
{
//
// A simple single buffer sorting algorithm.
//
// first, see if we need sorting by checking each element
// with the next. they should all be in order.
//
// if not, do qsort algorithm
    unsigned int i;
    unsigned int first_out_of_order_item = 0;

    for ( i = 0; i < num_events - 1; ++i )
    {
        first_out_of_order_item = i + 1;

        if ( MIDITimedMessage::CompareEvents (
                    *GetEventAddress ( i ),
                    *GetEventAddress ( first_out_of_order_item )
                ) == 1 )
            break;
    }

    if ( first_out_of_order_item >= num_events - 1 )
    {
//  return;  // no need for sort
    }

    QSort ( 0, num_events - 1 );
}

#endif

void MIDITrack::Shrink()
{
    int num_chunks_used = (int)( ( num_events / MIDITrackChunkSize ) + 1 );
    int num_chunks_alloced = (int)( buf_size / MIDITrackChunkSize );

    if ( num_chunks_used < num_chunks_alloced )
    {
        for ( int i = num_chunks_used; i < num_chunks_alloced; ++i )
        {
            jdks_safe_delete_object( chunk[i] );
        }

        buf_size = num_chunks_used * MIDITrackChunkSize;
    }
}

bool MIDITrack::Expand( int increase_amount )
{
    int num_chunks_to_expand = (int)( ( increase_amount / MIDITrackChunkSize ) + 1 );
    int num_chunks_alloced = (int)( buf_size / MIDITrackChunkSize );
    int new_last_chunk_num = (int)( num_chunks_to_expand + num_chunks_alloced );

    if ( new_last_chunk_num >= MIDIChunksPerTrack )
    {
        return false;
    }

    for ( int i = num_chunks_alloced; i < new_last_chunk_num; ++i )
    {
        chunk[i] = new MIDITrackChunk;

        if ( !chunk[i] )
        {
            buf_size = ( i - 1 ) * MIDITrackChunkSize;
            return false;
        }
    }

    buf_size = new_last_chunk_num * MIDITrackChunkSize;
    return true;
}

MIDITimedBigMessage *MIDITrack::GetEventAddress( int event_num )
{
    return chunk[event_num / ( MIDITrackChunkSize )]->GetEventAddress( ( event_num % MIDITrackChunkSize ) );
}

const MIDITimedBigMessage *MIDITrack::GetEventAddress( int event_num ) const
{
    return chunk[event_num / ( MIDITrackChunkSize )]->GetEventAddress( ( event_num % MIDITrackChunkSize ) );
}

bool MIDITrack::PutEvent( const MIDITimedBigMessage &msg, int n )
{
    if ( n == -1 ) // default value: put as last event
    {
        n = num_events;
    }
    if ( n > num_events || n < 0 ) // invalid value
    {
        return false;
    }
    if ( num_events >= buf_size )
    {
        if ( !Expand() )
            return false;
    }

    for ( int i = num_events; i > n; i-- )
    {
        GetEventAddress( i )->Copy( *GetEventAddress( i - 1 ) );
    }

    GetEventAddress( n )->Copy( msg );
    num_events++;
    return true;
}

bool MIDITrack::PutEvent( const MIDIDeltaTimedBigMessage &msg )
{
    const MIDIBigMessage msg2( msg );
    MIDITimedBigMessage m = msg2;
    MIDIClockTime t = msg.GetDeltaTime() + GetLastEventTime();
    m.SetTime( t );
    return PutEvent( m );
}

bool MIDITrack::PutEvent2( MIDITimedBigMessage &msg )
{
    if ( PutEvent( msg ) )
    {
        MIDIClockTime t = msg.GetTime();
        msg.Clear();
        msg.SetTime( t );
        return true;
    }
    return false;
}

bool MIDITrack::PutEvent( const MIDITimedMessage &msg, const MIDISystemExclusive *sysex )
{
    MIDITimedBigMessage m( msg, sysex );
    return PutEvent( m );
}

bool MIDITrack::PutTextEvent( MIDIClockTime time, int meta_event_type, const char *text, int length )
{
    MIDITimedMessage msg;
    msg.SetTime( time );
    msg.SetMetaEvent( meta_event_type, 0 );

    if ( length == 0 )
        length = (int)strlen( text );

    MIDISystemExclusive sysex( length );

    for ( int i = 0; i < length; ++i )
        sysex.PutSysByte( text[i] );

    return PutEvent( msg, &sysex );
}

bool MIDITrack::RemoveEvent( int event_num )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    for ( int i = event_num; i < num_events - 1; i++ )
    {
        GetEvent( i )->Copy( *GetEvent( i + 1 ) );
    }
    GetEvent( num_events - 1 )->Clear();
    num_events--;
    Shrink();
    return true;
}

bool MIDITrack::GetEvent( int event_num, MIDITimedBigMessage *msg ) const
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    else
    {
        msg->Copy( *GetEventAddress( event_num ) );
        return true;
    }
}

bool MIDITrack::SetEvent( int event_num, const MIDITimedBigMessage &msg )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    else
    {
        GetEventAddress( event_num )->Copy( msg );
        return true;
    }
}

bool MIDITrack::MakeEventNoOp( int event_num )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    else
    {
        MIDITimedBigMessage *ev = GetEventAddress( event_num );
        if ( ev )
            ev->SetNoOp();
        return true;
    }
}

bool MIDITrack::FindEventNumber( const MIDITimedBigMessage &msg, int *event_num, int _comp_mode ) const
{
    if ( num_events == 0 || msg.GetTime() > GetLastEventTime() )
    {
        *event_num = -1; // returns -1 in event_num
        return false;
    }

    int min = 0, // binary search with deferred detection algorithm
        max = num_events - 1;
    while ( min < max )
    {
        unsigned int mid = min + ( max - min ) / 2;
        if ( GetEvent( mid )->GetTime() < msg.GetTime() )
            min = mid + 1;
        else
            max = mid;
    }
    // the algorithm stops on 1st msg with time <= msg.GetTime()
    *event_num = min; // if event_num != -1 this is the last ev with time <= msg.GetTime()
    // if event_num != -1 this is the last ev with time <= t

    if ( _comp_mode == COMPMODE_TIME )                      // find first message with same time
        return GetEvent( min )->GetTime() == msg.GetTime(); // true if there is a message

    while ( min < num_events && GetEvent( min )->GetTime() == msg.GetTime() )
    {
        if ( _comp_mode == COMPMODE_EQUAL ) // find equal events
        {
            if ( *GetEvent( min ) == msg ) // element found
            {
                *event_num = min;
                return true;
            }
        }
        else if ( _comp_mode == COMPMODE_SAMEKIND ) // find same kind events
        {
            if ( MIDITimedBigMessage::IsSameKind( *GetEvent( min ), msg ) ) // element found
            {
                *event_num = min;
                return true;
            }
        }
        min++;
    }

    return false; // element not found
}

bool MIDITrack::FindEventNumber( MIDIClockTime time, int *event_num ) const
{
    if ( num_events == 0 || time > GetLastEventTime() )
    {
        *event_num = -1; // returns -1 in event_num
        return false;
    }

    int min = 0, // binary search with deferred detection algorithm
        max = num_events - 1;
    while ( min < max )
    {
        unsigned int mid = min + ( max - min ) / 2;
        if ( GetEvent( mid )->GetTime() < time )
            min = mid + 1;
        else
            max = mid;
    }
    // the algorithm stops on 1st msg with time <= msg.GetTime()
    *event_num = min; // if event_num != -1 this is the last ev with time <= t
    if ( GetEvent( min )->GetTime() == time )
        return true; // found event with time == t

    return false; // found event with time < t
}

MIDIClockTime MIDITrack::NoteLength( const MIDITimedBigMessage &msg ) const
{
    if ( !msg.IsNoteOn() )
    {
        return 0;
    }
    int event_num;
    const MIDITimedBigMessage *msgp;
    if ( !FindEventNumber( msg, &event_num, COMPMODE_EQUAL ) )
    {
        return 0;
    }
    for ( int i = event_num; i < num_events; i++ )
    {
        msgp = GetEvent( i );
        if ( msgp->IsNoteOff() && msgp->GetChannel() == msg.GetChannel() && msgp->GetNote() == msg.GetNote() )
        {
            return msgp->GetTime() - msg.GetTime();
        }
    }
    return GetLastEventTime() - msg.GetTime();
}

const MIDITimedBigMessage *MIDITrack::GetEvent( int event_num ) const
{
    if ( !IsValidEventNum( event_num ) )
    {
        return 0;
    }
    else
    {
        return GetEventAddress( event_num );
    }
}

MIDITimedBigMessage *MIDITrack::GetEvent( int event_num )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return 0;
    }
    else
    {
        return GetEventAddress( event_num );
    }
}

void MIDITrack::CloseOpenEvents( MIDIClockTime t )
{
    if ( t == 0 )
        return; // there aren't open events at time 0

    MIDIMatrix matrix;
    MIDITimedBigMessage msg;

    int ev_num = 0;
    while ( ev_num < num_events && ( msg = *GetEvent( ev_num ) ).GetTime() < t )
    {
        matrix.Process( msg );
        ev_num++;
    }
    while ( ev_num < num_events && ( msg = *GetEvent( ev_num ) ).GetTime() == t )
    {
        if ( msg.IsNoteOff() || msg.IsPedalOff() )
        {
            matrix.Process( msg );
            ev_num++;
        }
    } // ev_num is now the index of 1st event with timw > t

    for ( int ch = 0; ch < 16; ch++ ) // for every channel ...
    {
        int note_count = matrix.GetChannelCount( ch ); // get the number of notes on of the channel
        for ( int note = 0; note < 0x7f && note_count > 0; note++ )
        { // search which notes are on
            for ( int i = matrix.GetNoteCount( ch, note ); i > 0; i-- )
            { // if the note is on ... (i should normally be 1)
                msg.SetNoteOff( ch, note, 0 );
                msg.SetTime( t );
                InsertEvent( msg ); // ... insert a note off at time t
                ev_num++;           // and adjust ev_num
                for ( int j = ev_num; j < num_events; j++ )
                {
                    msg = *GetEvent( j ); // search the corresponding note off after t ...
                    if ( msg.IsNoteOff() && msg.GetChannel() == ch && msg.GetNote() == note )
                    {
                        DeleteEvent( msg ); // ... and delete it
                        break;
                    }
                }
                note_count--;
            }
        }

        if ( matrix.GetHoldPedal( ch ) )
        {
            msg.SetControlChange( ch, C_DAMPER, 0 );
            msg.SetTime( t );
            InsertEvent( msg );
            ev_num++;
            for ( int i = ev_num; i < num_events; i++ )
            {
                msg = *GetEvent( i ); // search the corresponding note off after t ...
                if ( msg.IsPedalOff() && msg.GetChannel() == ch )
                {
                    DeleteEvent( msg ); // ... and delete it
                    break;
                }
            }
        }
    }
}

/*
    if (iter.NotesOn()) {                   // there are notes on at time t
        for (int i = 0; i < 0x7f; i++) {
            msg.SetNoteOn(ch, i, 100);
            if (iter.IsNoteOn(i) && !iter.EventIsNow(msg)) {
                if (iter.FindNoteOff(i, &msgp))
                    Delete(*msgp);          // delete original note OFF
                msg.SetNoteOff(ch, i, 0);
                msg.SetTime(t - 1);
                Insert(msg);            // insert a note OFF at time t-1
                msg.SetTime(t);
            }
        }
    }
    msg.SetControlChange(ch, C_DAMPER, 127);
    if (iter.IsPedalOn()&& !iter.EventIsNow(msg))   {               // pedal (CTRL 64) on at time t
        iter.GoToTime(t);               // TODO: ia this necessary? I don't remember
        if (iter.FindPedalOff(&msgp))
            Delete(*msgp);                  // delete original pedal OFF
        msg.SetControlChange(ch, C_DAMPER, 0);
        msg.SetTime(t - 1);
        Insert(msg);                    // insert a pedal OFF at time t-1
        msg.SetTime(t);
    }
    msg.SetPitchBend(ch, 0);
    if (iter.GetBender()) {                 // pitch bend not 0 at time t
        iter.GoToTime(t);                   // updates iter
        while (iter.GetCurEvent(&msgp)) {  // chase and delete all subsequent bender messages,
            if (msgp->IsPitchBend()) {      // until bender == 0
                short val = msgp->GetBenderValue(); // remember bender value
                Delete(*msgp);
                if (val == 0) break;
            }
        }
        Insert(msg);                        // insert a new pitch bend = 0 at time t
    }
}

*/

/* ***** FROM N. CASSETTA




void MIDITrack::ChangeChannel(int ch) {
    ch &= 0x0f;
    for (int i = 0; i < num_events; i++) {
        if (GetEventAddress(i)->IsChannelMsg())
            GetEventAddress(i)->SetChannel(ch);
    }
}


// WARNING! THESE ARE DIFFERENT FROM ANALOGUE IN int_track.cpp:
//  1) IN A MIDI TRACK WE MUST CLOSE OPEN EVENTS WHEN COPYING OR DELETING INTERVALS
//  2) DEALING WITH META EVENTS (timesigs, keysig, text, markers, etc) IS TOO DIFFICULT AT
//     THIS (low) LEVEL, SO ONLY sysex DATA IS COPIED (if requested). FOR ADJUSTING META DATA
//     IS MUCH EASIER RECOMPOSE TRACK 0 BY THE MasterTrackComposer



void MIDITrack::InsertInterval(MIDIClockTime start, MIDIClockTime length, bool sysex, const MIDITrack* src) {
    if (length == 0) return;

    CloseOpenEvents(start);                         // truncate notes, pedal, bender at start
    int start_n = FindEventNumber(start);
    if (start_n != -1) {                            // there are events after start time
        for (int i = start_n; i < num_events; i++)  // moves these events
            GetEventAddress(i)->AddTime(length);
    }
    if(!src) return;                                // we want only move events

    for(int i = 0; i < src->GetNumEvents(); i++) {  // else inserts other events
        MIDITimedBigMessage msg(src->GetEvent(i));
                // msg is created and destroyed every time so no memory leaks with sysex (hope!)
        if (msg.GetTime() >= length) break;         // edit is too long
        if (!msg.IsSysEx() || (msg.IsSysEx() && sysex)) {
            msg.AddTime(start);
            Insert(msg);
        }
    }
    CloseOpenEvents(start + length);                // truncate at end
}


MIDITrack* MIDITrack::MakeInterval(MIDIClockTime start, MIDIClockTime end, MIDITrack* interval) const {
    if (end <= start) return interval;
    MIDIClockTime length = end - start;
    MIDITrack edittrack(*this);      // copy original track to make edits on it;

    interval->Clear();               // clear
    interval->SetEndTime(length);
    if (start > 0)
        edittrack.CloseOpenEvents(start);
                                    // truncate open events BEFORE start (so delete note off, etc
                                    // in edit track. If start == 0, no need to truncate
    edittrack.CloseOpenEvents(end); // truncate at end
    int start_n = edittrack.FindEventNumber(start);
                                    // now copy edittrack events to interval
    if(start_n != -1) {             // there are events
        for (int i = start_n; edittrack.GetEventAddress(i)->GetTime() < end; i++) {
            MIDITimedBigMessage msg(edittrack.GetEvent(i));
                                    // msg is created and destroyed every time so no memory leaks with sysex (hope!)
            if (msg.IsDataEnd()) break;
            if (msg.IsChannelMsg()) {
                msg.SubTime(start); // only channel messages are copied
                interval->Insert(msg);
            }
        }
    }
    return interval;
}


void MIDITrack::DeleteInterval(MIDIClockTime start, MIDIClockTime end) {
    if (end <= start) return;
    CloseOpenEvents(start);
    CloseOpenEvents(end);
    int start_n = FindEventNumber(start);   // first event with time >= start
    if (start_n == -1) return;              // no events after start: nothing to do
    MIDIClockTime length = end - start;
    while (GetEvent(start_n).GetTime() < end && !GetEvent(start_n).IsDataEnd())
                // DataEnd is not deleted by DelEvent, so we could enter in an infinite loop
        DelEvent(start_n);                  // deletes events between start and end
    for (int i = start_n; i < num_events; i++)
        GetEvent(i).SubTime(length);        // shifts subsequents
    Shrink();
}


void MIDITrack::ClearInterval(MIDIClockTime start, MIDIClockTime end) {
    if (end <= start) return;
    CloseOpenEvents(start);
    CloseOpenEvents(end);
    int start_n = FindEventNumber(start);
    if (start_n == -1) return;
    while (GetEventAddress(start_n)->GetTime() < end && !GetEventAddress(start_n)->IsDataEnd())
                // DataEnd is not deleted by DelEvent, so we could enter in an infinite loop
        DelEvent(start_n);
    Shrink();
}


void MIDITrack::ReplaceInterval(MIDIClockTime start, MIDIClockTime length, bool sysex, const MIDITrack* src) {
    if (length == 0) return;

    ClearInterval(start, start + length);           // deletes all in the interval
    for(int i = 0; i < src->GetNumEvents(); i++) {  // inserts events
        MIDITimedBigMessage msg(src->GetEvent(i));
                // msg is created and destroyed every time so no memory leaks with sysex (hope!)
        if (msg.GetTime() >= length) break;         // edit is too long
        msg.AddTime(start);
        Insert(msg);
    }
    CloseOpenEvents(start + length);                // truncate at end
}


void MIDITrack::CloseOpenEvents(MIDIClockTime t) {
    if (t == 0 || t > GetEndTime()) return;
                                            // there aren't open events at time 0 or after INT_END
    MIDITrackIterator iter(this);
    char ch  = iter.GetMIDIChannel();       // assumes all channel messages with the same channel!
    MIDITimedBigMessage* msgp;
    MIDITimedBigMessage msg;

    iter.GoToTime(t);
    msg.SetTime(t);                         // set right time for msg
    if (iter.NotesOn()) {                   // there are notes on at time t
        for (int i = 0; i < 0x7f; i++) {
            msg.SetNoteOn(ch, i, 100);
            if (iter.IsNoteOn(i) && !iter.EventIsNow(msg)) {
                if (iter.FindNoteOff(i, &msgp))
                    Delete(*msgp);          // delete original note OFF
                msg.SetNoteOff(ch, i, 0);
                msg.SetTime(t - 1);
                Insert(msg);            // insert a note OFF at time t-1
                msg.SetTime(t);
            }
        }
    }
    msg.SetControlChange(ch, C_DAMPER, 127);
    if (iter.IsPedalOn()&& !iter.EventIsNow(msg))   {               // pedal (CTRL 64) on at time t
        iter.GoToTime(t);               // TODO: ia this necessary? I don't remember
        if (iter.FindPedalOff(&msgp))
            Delete(*msgp);                  // delete original pedal OFF
        msg.SetControlChange(ch, C_DAMPER, 0);
        msg.SetTime(t - 1);
        Insert(msg);                    // insert a pedal OFF at time t-1
        msg.SetTime(t);
    }
    msg.SetPitchBend(ch, 0);
    if (iter.GetBender()) {                 // pitch bend not 0 at time t
        iter.GoToTime(t);                   // updates iter
        while (iter.GetCurEvent(&msgp)) {  // chase and delete all subsequent bender messages,
            if (msgp->IsPitchBend()) {      // until bender == 0
                short val = msgp->GetBenderValue(); // remember bender value
                Delete(*msgp);
                if (val == 0) break;
            }
        }
        Insert(msg);                        // insert a new pitch bend = 0 at time t
    }
}

*********/
}
