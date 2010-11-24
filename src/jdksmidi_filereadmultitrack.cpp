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

bool MIDIFileReadMultiTrack::ChanMessage ( const MIDITimedMessage &msg ) // VRM
{
    /*
        // this original code deleted for separate functionality in MIDIMultiTrack::AssignEventsToTracks(0)
        if ( the_format == 0 )
        {
          // split format 0 files into separate tracks, one for each channel,
          // keep track 0 for tempo and meta-events
          AddEventToMultiTrack ( msg, 0, msg.GetChannel() + 1 );
        }
        else
        {
          AddEventToMultiTrack ( msg, 0, cur_track );
        }
    */
    return AddEventToMultiTrack ( msg, 0, cur_track ); // VRM
}

void MIDIFileReadMultiTrack::SortEventsOrder() // func by VRM
{
    multitrack->SortEventsOrder();
}

bool MIDIFileReadMultiTrack::mf_sysex ( MIDIClockTime time, const MIDISystemExclusive &ex ) // VRM
{
    MIDITimedMessage msg;
    msg.SetSysEx();
    msg.SetTime ( time );
    MIDISystemExclusive *sysex = new MIDISystemExclusive ( ex );
    return AddEventToMultiTrack ( msg, sysex, cur_track ); // VRM
}

bool MIDIFileReadMultiTrack::mf_arbitrary ( MIDIClockTime time, int len, unsigned char *data )
{
    // ignore arbitrary byte strings; VRM@TODO - add event to multitrack ??
    return true; // VRM
}

bool MIDIFileReadMultiTrack::mf_metamisc ( MIDIClockTime time, int type, int len, unsigned char *data )
{
    // VRM code for all miscellaneous meta events

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

bool MIDIFileReadMultiTrack::mf_timesig ( MIDIClockTime time, int num, int den_pow, int clks_per_metro, int notated_32nd_per_quarter ) // func by VRM
{
    MIDITimedMessage msg;
    msg.SetTime ( time );
    msg.SetTimeSig ( num, den_pow, clks_per_metro, notated_32nd_per_quarter );
    msg.SetDataLength( 5 ); // VRM source 4 bytes + 1 byte for denominator
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

bool MIDIFileReadMultiTrack::mf_tempo ( MIDIClockTime time, unsigned char a, unsigned char b, unsigned char c ) // func by VRM
{
    MIDITimedMessage msg;
    msg.SetTime ( time );
    msg.SetMetaEvent ( MF_META_TEMPO, a, b );
    msg.SetByte4( c );
    msg.SetDataLength( 3 );
    return AddEventToMultiTrack ( msg, 0, cur_track );
}

bool MIDIFileReadMultiTrack::mf_keysig ( MIDIClockTime time, int c, int v ) // VRM
{
    MIDITimedMessage msg;
    msg.SetTime ( time );
    msg.SetKeySig ( ( unsigned char ) c, ( unsigned char ) v );
    msg.SetDataLength( 2 ); // VRM
    return AddEventToMultiTrack ( msg, 0, cur_track ); // VRM
}

bool MIDIFileReadMultiTrack::mf_sqspecific ( MIDIClockTime time, int len, unsigned char *s ) // VRM
{
    // read sequencer specific message as pseudo-text message
    return mf_text ( time, MF_META_SEQUENCER_SPECIFIC, len, s ); // VRM
}

bool MIDIFileReadMultiTrack::mf_text ( MIDIClockTime time, int type, int len, unsigned char *s ) // VRM
{
    MIDITimedMessage msg;
    msg.SetStatus ( META_EVENT );
    msg.SetMetaType ( ( uchar ) type ); // remember - MF_META_* id codes match META_* codes
    msg.SetTime ( time );

    MIDISystemExclusive *sysex = new MIDISystemExclusive ( len );

    for ( int i = 0; i < len; ++i )
    {
        sysex->PutSysByte ( s[i] );
    }

    msg.SetDataLength( 0 ); // VRM variable data length don't saved to data_length
    return AddEventToMultiTrack ( msg, sysex, cur_track ); // VRM
}

bool MIDIFileReadMultiTrack::mf_eot ( MIDIClockTime time ) // VRM
{
    MIDITimedMessage msg;
    msg.SetStatus ( META_EVENT );
    msg.SetMetaType ( META_END_OF_TRACK ); // VRM
    msg.SetTime ( time );
    msg.SetDataLength( 0 ); // VRM
    return AddEventToMultiTrack ( msg, 0, cur_track ); // VRM
}

}
