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
#include "jdksmidi/filereadmultitrack.h"

namespace jdksmidi
{

MIDIFileReadMultiTrack::MIDIFileReadMultiTrack ( MIDIMultiTrack *mlttrk )
    : multitrack ( mlttrk ), cur_track ( -1 )
{
}

MIDIFileReadMultiTrack::~MIDIFileReadMultiTrack()
{
}

void MIDIFileReadMultiTrack::mf_error ( const char * )
{
}

void MIDIFileReadMultiTrack::mf_starttrack ( int trk )
{
    cur_track = trk;
}

void MIDIFileReadMultiTrack::mf_endtrack ( int trk )
{
    cur_track = -1;
}

bool MIDIFileReadMultiTrack::AddEventToMultiTrack ( const MIDITimedMessage &msg, MIDISystemExclusive *sysex, int dest_track )
{
    bool result = false;

    if ( dest_track != -1 && dest_track < multitrack->GetNumTracks() )
    {
        MIDITrack *t = multitrack->GetTrack ( dest_track );

        if ( t )
        {
            result = t->PutEvent ( msg, sysex );
        }
    }

    return result;
}

void MIDIFileReadMultiTrack::mf_header (int the_format_, int ntrks_, int division_ )
{
    the_format = the_format_;
    num_tracks = ntrks_;
    division = division_;
    multitrack->SetClksPerBeat ( division );
}

bool MIDIFileReadMultiTrack::ChanMessage ( const MIDITimedMessage &msg )
{
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

void MIDIFileReadMultiTrack::SortEventsOrder()
{
    multitrack->SortEventsOrder();
}

bool MIDIFileReadMultiTrack::mf_metamisc ( MIDIClockTime time, int type, int len, unsigned char *data )
{
    // code for all miscellaneous meta events

    MIDITimedMessage msg;
    msg.SetTime ( time );

    msg.SetStatus( META_EVENT );
    msg.SetByte1( type );

    if ( len <= 5 )
    {
        if ( len > 0 )
            msg.SetByte2( data[0] );

        if ( len > 1 )
            msg.SetByte3( data[1] );

        if ( len > 2 )
            msg.SetByte4( data[2] );

        if ( len > 3 )
            msg.SetByte5( data[3] );

        if ( len > 4 )
            msg.SetByte6( data[4] );
    }
    // else msg add to track, but do'nt write to output midifile!

    msg.SetDataLength( len );
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

bool MIDIFileReadMultiTrack::mf_timesig ( MIDIClockTime time, int num, int den_pow, int clks_per_metro, int notated_32nd_per_quarter )
{
    MIDITimedMessage msg;
    msg.SetTime ( time );
    msg.SetTimeSig ( num, den_pow, clks_per_metro, notated_32nd_per_quarter );
    msg.SetDataLength( 5 ); // source 4 bytes + 1 byte for denominator
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

bool MIDIFileReadMultiTrack::mf_tempo ( MIDIClockTime time, unsigned char a, unsigned char b, unsigned char c )
{
    MIDITimedMessage msg;
    msg.SetTime ( time );
    msg.SetMetaEvent ( MF_META_TEMPO, a, b );
    msg.SetByte4( c );
    msg.SetDataLength( 3 );
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

bool MIDIFileReadMultiTrack::mf_keysig ( MIDIClockTime time, int c, int v )
{
    MIDITimedMessage msg;
    msg.SetTime ( time );
    msg.SetKeySig ( ( unsigned char ) c, ( unsigned char ) v );
    msg.SetDataLength( 2 );
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

bool MIDIFileReadMultiTrack::mf_sqspecific ( MIDIClockTime time, int len, unsigned char *s )
{
    // read sequencer specific message as pseudo-text message
    return mf_text ( time, MF_META_SEQUENCER_SPECIFIC, len, s );
}

bool MIDIFileReadMultiTrack::mf_text ( MIDIClockTime time, int type, int len, unsigned char *s )
{
    MIDITimedMessage msg;
    msg.SetStatus ( META_EVENT );
    msg.SetMetaType ( ( uchar ) type ); // remember - MF_META_* id codes match META_* codes
    msg.SetTime ( time );

    MIDISystemExclusive sysex( len );

    for ( int i = 0; i < len; ++i )
    {
        sysex.PutSysByte ( s[i] );
    }

    msg.SetDataLength( 0 ); // variable data length don't saved to data_length
    return AddEventToMultiTrack ( msg, &sysex, cur_track );
}

bool MIDIFileReadMultiTrack::mf_sysex ( MIDIClockTime time, int type, int len, unsigned char *s )
{
    MIDITimedMessage msg;
    msg.SetSysEx( type ); // set msg status byte (0xF0 or 0xF7)

    int num = len; // number of possible SysExURT header data bytes in msg, 0...5
    if ( num > 5 )
        num = 5;

    // add up to 5 starting bytes for SysExURT functions
    if ( num > 0 )
        msg.SetByte2( s[0] );
    if ( num > 1 )
        msg.SetByte3( s[1] );
    if ( num > 2 )
        msg.SetByte4( s[2] );
    if ( num > 3 )
        msg.SetByte5( s[3] );
    if ( num > 4 )
        msg.SetByte6( s[4] );

    msg.SetTime ( time );
    MIDISystemExclusive sysex( len );

    for ( int i = 0; i < len; ++i )
    {
        sysex.PutSysByte ( s[i] );
    }

   msg.SetDataLength( num );
   return AddEventToMultiTrack ( msg, &sysex, cur_track );
}

bool MIDIFileReadMultiTrack::mf_eot ( MIDIClockTime time )
{
    MIDITimedMessage msg;
    msg.SetStatus ( META_EVENT );
    msg.SetMetaType ( META_END_OF_TRACK );
    msg.SetTime ( time );
    msg.SetDataLength( 0 );
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

}
