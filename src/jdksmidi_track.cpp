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
    Shrink();                   // added by NC
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

bool MIDITrack::SetEndTime ( MIDIClockTime time )
{
    if ( IsTrackEnded() )
    {
        if ( num_events > 1 && GetEvent( num_events - 2 )->GetTime() > time )
        {   // we are trying to insert an EOT before last event
            return false;
        }
        GetLastEvent()->SetTime( time );
    }
    else
    {
        if ( num_events > 0 && GetEvent( num_events - 1 )->GetTime() > time )
        {   // as above
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

bool MIDITrack::Insert(const MIDITimedBigMessage& msg) {
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

bool MIDITrack::Replace(const MIDITimedBigMessage& msg) {
    if (msg.IsDataEnd()) return false;                  // DATA_END only auto managed

    int event_num;
    if ( !FindEventNumber(msg.GetTime(), &event_num) );
        return false;

    while ( GetEvent(event_num)->GetTime() == msg.GetTime() )
    {
        if ( MIDITimedBigMessage::IsSameKind(*GetEvent(event_num), msg) )
        {
            return SetEvent(event_num, msg);
        }
        event_num++;
    }

    return false;
}

bool MIDITrack::Delete ( const MIDITimedBigMessage& msg )
{
    if ( msg.IsDataEnd() )
    {
        return false;
    }
    int ind;
    if ( !FindEventNumber( msg, &ind ) )
    {
        return false;
    }
    for ( int i = ind; i < num_events-1; i++ )
    {
        GetEvent(i)->Copy(*GetEvent(i+1));
    }
    GetEvent(num_events-1)->Clear();
    num_events--;
    Shrink();
    return true;
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

bool MIDITrack::PutEvent ( const MIDITimedBigMessage &msg, int n )
{
    if ( n == -1)       // default value: put as last event
    {
        n = num_events;
    }
    if ( n > num_events || n < 0 )  // invalid value
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
        GetEventAddress( i )->Copy ( *GetEventAddress( i-1 ) );
    }

    GetEventAddress ( n )->Copy ( msg );
    num_events++;
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

bool MIDITrack::FindEventNumber(const MIDITimedBigMessage& msg, int* event_num) const {
    if (num_events == 0) return false;

    int min = 0,                                // binary search
        max = num_events - 1,
        mid = (max + min) / 2;
    while (max - min > 1) {
        if ( GetEvent(mid)->GetTime() == msg.GetTime() )
            break;
        else if ( GetEvent(mid)->GetTime() < msg.GetTime() )
            min = mid;
        else
            max = mid;
        mid = (max + min) / 2;
    }
    if (min == max - 1 && GetEvent(mid)->GetTime() < msg.GetTime()  )
        mid++;                                  // max-min = 1; mid could be one of these

    while ( mid > 0 && GetEvent(mid - 1)->GetTime() == msg.GetTime() )
        mid--;

    while ( GetEvent(mid)->GetTime() == msg.GetTime() )
    {
        if (*GetEvent(mid) == msg)              // element found
        {
            *event_num = mid;
            return true;
        }
        mid++;
    }

    return false;
}


bool MIDITrack::FindEventNumber(MIDIClockTime time, int* event_num) const {
    if (num_events == 0 || time > GetLastEventTime()) return false;

    int min = 0,                            // binary search
        max = num_events - 1,
        mid = (max + min) / 2;
    while (max - min > 1) {
        if ( GetEvent(mid)->GetTime() == time )
            break;
        else if ( GetEvent(mid)->GetTime() < time )
            min = mid;
        else
            max = mid;
        mid = (max + min) / 2;
    }

    if (min == max - 1 && GetEvent(mid)->GetTime() < time )
        mid++;                                  // max-min = 1; mid could be one of these

    while ( mid > 0 && GetEvent(mid - 1)->GetTime() == time )
        mid--;

    *event_num = mid;
    if ( GetEvent(mid)->GetTime() == time )
        return true;
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


/****** FROM N. CASSETTA


MIDITrack::MIDITrack(MIDIClockTime end) : num_events(0), buf_size(0) {
// a track always contains at least the MIDI_END event, so num_events > 0
    Expand();
    MIDITimedBigMessage m;
    m.SetDataEnd();
    m.SetTime(end);
    PutEvent(0, m);
}




bool MIDITrack::Insert(const MIDITimedBigMessage& msg) {
    if (msg.IsDataEnd()) return false;                  // DATA_END only managed by PutEvent

    if (GetEndTime() <= msg.GetTime()) {                // insert as last event
        SetEndTime(msg.GetTime());                      // adjust DATA_END
        return PutEvent(num_events-1, msg);             // insert just before DATA_END
    }
                                                        // binsearch copied from binsearch.h
    int min = 0,
        max = num_events - 1,
        mid = (max + min) / 2;
    while (max - min > 1) {
        if (GetEvent(mid) < msg) min = mid;
        else max = mid;
        mid = (max + min) / 2;
    }
    if (min < max && GetEvent(mid) < msg)       // max-min = 1; mid could be one of these
        mid++;

// WARNING! THIS IS DIFFERENT FROM ANALOGUE IN int_track.cpp AS MIDI MESSAGES AREN'T TOTALLY
// ORDERED (see midi_msg.operator== and midi_msg.operator< )

    min = mid;                                  // dummy, for remembering
    while (!EndOfTrack(mid) && GetEvent(mid).GetTime() == msg.GetTime()) {
        if (GetEvent(mid) == msg)               // element found
            return SetEvent(mid, msg);          // WARNING: I changed this (from false): review!
        mid++;
    }
    return PutEvent(min, msg);
}


bool MIDITrack::Delete(const MIDITimedBigMessage& msg) {
    if (msg.IsDataEnd()) return false;                  // we can't delete MIDI_END event
    int min = 0,                                        // binsearch copied from binsearch.h
        max = num_events - 1,
        mid = (max + min) / 2;
    while (max - min > 1) {
        if (GetEvent(mid) < msg) min = mid;
        else max = mid;
        mid = (max + min) / 2;
    }
    if (min < max && GetEvent(mid) < msg)               // max-min = 1; mid could be one of these
        mid++;

    if (GetEvent(mid) == msg) {                         // mid is the place of msg
        DelEvent(mid);                                  // delete
        return true;
    }
    return false;                                       // not found
}


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


int MIDITrack::FindEventNumber(const MIDITimedBigMessage& m) const {
// returns -1 if not found
    if (num_events == 0) return -1;             // binsearch copied from binsearch.h
    int min = 0,
        max = num_events - 1,
        mid = (max + min) / 2;
    while (max - min > 1) {
        if (GetEvent(mid) < m) min = mid;
        else max = mid;
        mid = (max + min) / 2;
    }
    if (min < max && GetEvent(mid) < m)         // max-min = 1; mid could be one of these
        mid++;
    if (GetEvent(mid) == m)                     // element found
        return mid;
    return -1;
}


int MIDITrack::FindEventNumber(MIDIClockTime time) const {
    if (num_events == 0 || time > GetEndTime()) return -1;
    int min = 0,                                        // binary search
        max = num_events - 1,
        mid = (max + min) / 2;
    while (max - min > 1) {
        if (GetEvent(mid).GetTime() < time) min = mid;
        else max = mid;
        mid = (max + min) / 2;
    }
    if (GetEvent(mid).GetTime() < time)     // mid.GetTime() should be <= time
        return ++mid;
    while(mid && GetEvent(mid-1).GetTime() == time)
        mid--;                              // there are former events with same time
    return mid;
}


//
// low level private functions
//


bool MIDITrack::PutEvent(int ev_num, const MIDITimedBigMessage& m) {
    if (ev_num > num_events) return false;
    if(num_events >= buf_size) {
        if(!Expand()) return false;
    }
    int event_chunk = ev_num / MIDITrackChunkSize;
    int ev_num_in_chunk = ev_num % MIDITrackChunkSize;
    int num_chunks_alloced = buf_size / MIDITrackChunkSize;
    for (int i = num_chunks_alloced-1; i > event_chunk; i--) {
        chunk[i]->Insert(0);
        MIDITrackChunk::LastToFirst(*chunk[i-1], *chunk[i]);
    }
    chunk[event_chunk]->Insert(ev_num_in_chunk);
    GetEvent(ev_num) = m;
    num_events++;
    return true;
}


bool MIDITrack::DelEvent(int ev_num) {
    if (ev_num >= num_events) return false;
    GetEventAddress(ev_num)->Clear();       // deletes sysex
    int event_chunk = ev_num / MIDITrackChunkSize;
    int ev_num_in_chunk = ev_num % MIDITrackChunkSize;
    int num_chunks_alloced = buf_size / MIDITrackChunkSize;
    chunk[event_chunk]->Delete(ev_num_in_chunk);
    for (int i = event_chunk; i < num_chunks_alloced-1; i++) {
        MIDITrackChunk::FirstToLast(*chunk[i+1], *chunk[i]);
        chunk[i+1]->Delete(0);
    }
    num_events--;
    return true;
}


/*

// SEE HEADER!!!!!!!!!!!!!!!!!
bool	MIDITrack::PutEvent( const MIDITimedBigMessage &msg, MIDISysEx *sysex )
  {
    if( num_events >= buf_size )
    {
      if( !Expand() )
        return false;
    }

    MIDITimedBigMessage *e = GetEventAddress( num_events );

    e->Copy( msg );
    e->SetSysEx( sysex );

    ++num_events;

    return true;
  }

*/

/* ***********************************************************************************
/* *                 C L A S S   M I D I T r a c k I t e r a t o r
/* ***********************************************************************************

// this is interely added by me!!!! NC

MIDITrackIterator::MIDITrackIterator(MIDITrack* trk) : track(trk) {
    FindChannel();
    GoToZero();
}


void MIDITrackIterator::SetTrack(MIDITrack* trk) {
        track = trk;
        FindChannel();
        GoToZero();
}



bool MIDITrackIterator::FindNoteOff(uchar note, MIDITimedBigMessage** msg) {
    for (int i = cur_ev_num; i < track->GetNumEvents(); i ++) {
        *msg = track->GetEventAddress(i);
        if ((*msg)->IsNoteOff() && (*msg)->GetNote() == note)
            return true;                //  msg is the correct note off
    }
    return false;
}


bool MIDITrackIterator::FindPedalOff(MIDITimedBigMessage** msg) {
    for (int i = cur_ev_num; i < track->GetNumEvents(); i ++) {
        *msg = track->GetEventAddress(i);
        if ((*msg)->IsControlChange() && (*msg)->GetController() == 64 && (*msg)->GetControllerValue() < 64)
            return true;                //  msg is the correct pedal off
    }
    return false;
}


bool MIDITrackIterator::GetCurEvent(MIDITimedBigMessage** msg, MIDIClockTime end ) {// = 0xffffffff
    if (track->EndOfTrack(cur_ev_num)) return false;
    if (end < cur_time) return false;                       // end lesser than cur_time
    *msg = track->GetEventAddress(cur_ev_num);
    MIDIClockTime new_time = (*msg)->GetTime();
    if (new_time > cur_time) {                              // is new time > cur_time?
        if (new_time > end)
            return false;
        cur_time = new_time;
        ScanEventsAtThisTime();                             // update status processing all messages at this time
    }
    cur_ev_num++;                                           // increment message pointer
    return true;
}


MIDIClockTime MIDITrackIterator::GetCurEventTime() const {
    if (track->EndOfTrack(cur_ev_num)) return 0xffffff;     // we are at the end of track
    return track->GetEventAddress(cur_ev_num)->GetTime();
}


bool MIDITrackIterator::EventIsNow(const MIDITimedBigMessage& msg) {
    MIDITimedBigMessage msg1;

    int ev_num = track->FindEventNumber(cur_time);
    while(!track->EndOfTrack(ev_num) && (msg1 = track->GetEvent(ev_num)).GetTime() == cur_time) {
        if (msg1 == msg) return true;
        ev_num++;
    }
    return false;
}



void MIDITrackIterator::GoToZero() {
    // go to time zero
    cur_time = 0;
    cur_ev_num = 0;

    // reset midi status
    program = 0xff;
    for (int i = 0; i < 128; i++) {
        controls[i] = 0xff;
        notes_on[i] = 0;
    }
    bender_value = 0;
    num_notes_on = 0;
    ScanEventsAtThisTime();                                 // process all messages at this time
}


bool MIDITrackIterator::GoToNextEvent() {
    if (track->EndOfTrack(cur_ev_num+1)) return false;
    cur_ev_num++;
    MIDITimedBigMessage* msg = track->GetEventAddress(cur_ev_num);
    if (msg->GetTime() > cur_time) {
        cur_time = msg->GetTime();
        ScanEventsAtThisTime();                             // process all messages at this time
    }
    return true;
}


bool MIDITrackIterator::GoToTime(MIDIClockTime time) {
    MIDITimedBigMessage* msg;

    if (time > track->GetEndTime()) return false;
    if (time < cur_time)
        GoToZero();
    while (GetCurEventTime() < time)
        GetCurEvent(&msg);
    cur_time = time;
    ScanEventsAtThisTime();
    return true;
}


void MIDITrackIterator::FindChannel() {
    channel = 0xff;
    for (int i = 0; i < track->GetNumEvents(); i ++) {
        if (track->GetEvent(i).IsChannelMsg()) {
            channel = track->GetEvent(i).GetChannel();
            return;
        }
    }
}


bool MIDITrackIterator::Process(const MIDITimedBigMessage *msg) {

    // is it a normal MIDI channel message?
    if(msg->IsChannelMsg()) {
        if (msg->GetChannel() != channel) return false;
        switch (msg->GetType()) {
            case NOTE_OFF :
                if(notes_on[msg->GetNote()]) {
                    notes_on[msg->GetNote()] = 0;
                    num_notes_on--;
                }
                break;
            case NOTE_ON :
                if (msg->GetVelocity()) {
                    if (!notes_on[msg->GetNote()]) {
                        notes_on[msg->GetNote()] = msg->GetVelocity();
                        num_notes_on++;
                    }
                }
                else {
                    if (notes_on[msg->GetNote()]) {
                        notes_on[msg->GetNote()] = 0;
                        num_notes_on--;
                    }
                }
                break;
            case PITCH_BEND :
                bender_value = msg->GetBenderValue();
                break;
            case CONTROL_CHANGE :
                controls[msg->GetController()] = msg->GetControllerValue();
                break;
            case PROGRAM_CHANGE :
                program = msg->GetPGValue();
                break;
        }
        return true;
    }
    return false;
}


void MIDITrackIterator::ScanEventsAtThisTime() {
// process all messages up to and including this time only
// warning: this can be used only when we reach the first event at a new time!

    MIDIClockTime oldtime = cur_time;
    int oldnum = cur_ev_num;
    //cur_ev_num = track->FindEventNumber(cur_time);      // is it necessary? I think no
    MIDITimedBigMessage *msg;

    while(GetCurEvent(&msg, oldtime))
        Process(msg);
    cur_time = oldtime;
    cur_ev_num = oldnum;
}

*********/




}
