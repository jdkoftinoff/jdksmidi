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

#include "jdksmidi/world.h"
#include "jdksmidi/track.h"

#ifndef DEBUG_MDTRACK
# define DEBUG_MDTRACK 0
#endif

#if DEBUG_MDTRACK
# undef DBG
# define DBG(a) a
#endif

namespace jdksmidi
{

const MIDITimedBigMessage * MIDITrackChunk::GetEventAddress ( int event_num ) const
{
    return &buf[event_num];
}

MIDITimedBigMessage * MIDITrackChunk::GetEventAddress ( int event_num )
{
    return &buf[event_num];
}





MIDITrack::MIDITrack ( int size )
{
    buf_size = 0;
    num_events = 0;

    for ( int i = 0; i < MIDIChunksPerTrack; ++i )
        chunk[i] = 0;

    if ( size )
    {
        Expand ( size );
    }
}

MIDITrack::MIDITrack ( const MIDITrack &t )
{
    buf_size = 0;
    num_events = 0;

    for ( int i = 0; i < t.GetNumEvents(); ++i )
    {
        const MIDITimedBigMessage *src;
        src = t.GetEventAddress ( i );
        PutEvent ( *src ); // it execute Expand()
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
}

bool MIDITrack::EventsOrderOK() const
{
    if ( num_events < 2 )
        return true;

    MIDIClockTime time0 = GetEventAddress(0)->GetTime();

    for ( int i = 1; i < num_events; ++i )
    {
        MIDIClockTime time1 = GetEventAddress(i)->GetTime();
        if ( time0 > time1 )
            return false;
        time0 = time1;
    }

    return true;
}

void MIDITrack::SortEventsOrder()
{
    std::vector< Event_time > et( num_events );

    int n;
    for ( n = 0; n < num_events; ++n )
    {
        et[n].event_number = n;
        et[n].time = GetEventAddress(n)->GetTime();
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

        for (int i = 1; i < max_distance_between_identical_events; ++i)
        {
            if ( (n+i) >= num_events )
                break;

            MIDITimedBigMessage *mni = GetEvent( n+i );
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

const MIDITrack & MIDITrack::operator = ( const MIDITrack & src )
{
    if ( num_events == src.num_events )
    {
        for ( int n = 0; n < num_events; ++n )
        {
            const MIDITimedBigMessage * msg = src.GetEventAddress( n );
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
            const MIDITimedBigMessage *msg = src.GetEventAddress ( i );
            PutEvent ( *msg ); // it execute Expand()
        }
    }

    return *this;
}

void MIDITrack::ClearAndMerge (
    const MIDITrack *src1,
    const MIDITrack *src2
)
{
    Clear();
    const MIDITimedBigMessage *ev1;
    int cur_trk1ev = 0;
    int num_trk1ev = src1->GetNumEvents();
    const MIDITimedBigMessage *ev2;
    int cur_trk2ev = 0;
    int num_trk2ev = src2->GetNumEvents();
    MIDIClockTime last_data_end_time = 0;

    while (
        cur_trk1ev < num_trk1ev
        || cur_trk2ev < num_trk2ev
    )
    {
        // skip any NOPs on track 1
        ev1 = src1->GetEventAddress ( cur_trk1ev );
        ev2 = src2->GetEventAddress ( cur_trk2ev );
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

                PutEvent ( *ev1 );
                ++cur_trk1ev;
            }
        }

        else if ( ( !has_ev1 && has_ev2 ) )
        {
            // nothing left on trk 1
            if ( !ev2->IsNoOp() )
            {
                PutEvent ( *ev2 );
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

                PutEvent ( *ev1 );
                ++cur_trk1ev;
            }

            else
            {
                if ( ev2->GetTime() > last_data_end_time )
                {
                    last_data_end_time = ev2->GetTime();
                }

                PutEvent ( *ev2 );
                ++cur_trk2ev;
            }
        }
    }

    // put single final data end event
    MIDITimedBigMessage dataend;
    dataend.SetTime ( last_data_end_time );
    dataend.SetDataEnd();
    PutEvent ( dataend );
}

#if 0
bool MIDITrack::Insert ( int start_event, int num )
{
    // TO DO: Insert
    return true;
}

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
    int num_chunks_used = ( int ) ( ( num_events / MIDITrackChunkSize ) + 1 );
    int num_chunks_alloced = ( int ) ( buf_size / MIDITrackChunkSize );

    if ( num_chunks_used < num_chunks_alloced )
    {
        for ( int i = num_chunks_used; i < num_chunks_alloced; ++i )
        {
            jdks_safe_delete_object( chunk[i] );
        }

        buf_size = num_chunks_used * MIDITrackChunkSize;
    }
}

bool MIDITrack::Expand ( int increase_amount )
{
    int num_chunks_to_expand = ( int ) ( ( increase_amount / MIDITrackChunkSize ) + 1 );
    int num_chunks_alloced = ( int ) ( buf_size / MIDITrackChunkSize );
    int new_last_chunk_num = ( int ) ( num_chunks_to_expand + num_chunks_alloced );

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

MIDITimedBigMessage * MIDITrack::GetEventAddress ( int event_num )
{
    return chunk[ event_num/ ( MIDITrackChunkSize ) ]->GetEventAddress (
               ( event_num % MIDITrackChunkSize ) );
}

const MIDITimedBigMessage * MIDITrack::GetEventAddress ( int event_num ) const
{
    return chunk[ event_num/ ( MIDITrackChunkSize ) ]->GetEventAddress (
               ( event_num % MIDITrackChunkSize ) );
}

bool MIDITrack::PutEvent ( const MIDITimedBigMessage &msg )
{
    if ( num_events >= buf_size )
    {
        if ( !Expand() )
            return false;
    }

    GetEventAddress ( num_events++ )->Copy ( msg );
    return true;
}

bool MIDITrack::PutEvent ( const MIDIDeltaTimedBigMessage &msg )
{
    const MIDIBigMessage msg2( msg );
    MIDITimedBigMessage m = msg2;
    MIDIClockTime t = msg.GetDeltaTime() + GetLastEventTime();
    m.SetTime( t );
    return PutEvent( m );
}

bool MIDITrack::PutEvent2 ( MIDITimedBigMessage &msg )
{
    if ( PutEvent ( msg ) )
    {
        MIDIClockTime t = msg.GetTime();
        msg.Clear();
        msg.SetTime( t );
        return true;
    }
    return false;
}

bool MIDITrack::PutEvent ( const MIDITimedMessage &msg, const MIDISystemExclusive *sysex )
{
    MIDITimedBigMessage m ( msg, sysex );
    return PutEvent ( m );
}

bool MIDITrack::PutTextEvent ( MIDIClockTime time, int meta_event_type, const char *text, int length )
{
    MIDITimedMessage msg;
    msg.SetTime( time );
    msg.SetMetaEvent( meta_event_type , 0 );

    if ( length == 0 )
        length = (int) strlen( text );

    MIDISystemExclusive sysex( length );

    for ( int i = 0; i < length; ++i )
        sysex.PutSysByte ( text[i] );

    return PutEvent( msg, &sysex );
}

bool MIDITrack::GetEvent ( int event_num, MIDITimedBigMessage *msg ) const
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    else
    {
        msg->Copy ( *GetEventAddress ( event_num ) );
        return true;
    }
}

bool MIDITrack::SetEvent ( int event_num, const MIDITimedBigMessage &msg )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    else
    {
        GetEventAddress ( event_num )->Copy ( msg );
        return true;
    }
}

bool MIDITrack::MakeEventNoOp ( int event_num )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return false;
    }
    else
    {
        MIDITimedBigMessage *ev = GetEventAddress ( event_num );
        if ( ev )
            ev->SetNoOp();
        return true;
    }
}

bool MIDITrack::FindEventNumber ( MIDIClockTime time, int *event_num ) const
{
    ENTER ( "MIDITrack::FindEventNumber( int , int * )" );
    // TO DO: try make this a binary search

    for ( int i = 0; i < num_events; ++i )
    {
        const MIDITimedBigMessage *msg = GetEventAddress ( i );

        if ( msg->GetTime() >= time )
        {
            *event_num = i;
            return true;
        }
    }

    *event_num = num_events;
    return false;
}

const MIDITimedBigMessage *MIDITrack::GetEvent ( int event_num ) const
{
    if ( !IsValidEventNum( event_num ) )
    {
        return 0;
    }
    else
    {
        return GetEventAddress ( event_num );
    }
}

MIDITimedBigMessage *MIDITrack::GetEvent ( int event_num )
{
    if ( !IsValidEventNum( event_num ) )
    {
        return 0;
    }
    else
    {
        return GetEventAddress ( event_num );
    }
}

}
