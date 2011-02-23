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

#include "jdksmidi/fileshow.h"

#ifndef DEBUG_MDFSH
# define DEBUG_MDFSH 0
#endif

#if DEBUG_MDFSH
# undef DBG
# define DBG(a) a
#endif

namespace jdksmidi
{

MIDIFileShow::MIDIFileShow ( FILE *out_, bool sqspecific_as_text_ )
    :
    out ( out_ ),
    sqspecific_as_text ( sqspecific_as_text_ )
{
    ENTER ( "MIDIFileShow::MIDIFileShow()" );
}

MIDIFileShow::~MIDIFileShow()
{
    ENTER ( "MIDIFileShow::~MIDIFileShow()" );
}

void MIDIFileShow::mf_error ( const char *e )
{
    fprintf ( out, "\nParse Error: %s\n", e );
    MIDIFileEvents::mf_error ( e );
}

void MIDIFileShow::mf_starttrack ( int trk )
{
    fprintf ( out, "Start Track #%d\n", trk );
}

void MIDIFileShow::mf_endtrack ( int trk )
{
    fprintf ( out, "End Track   #%d\n", trk );
}

void MIDIFileShow::mf_header ( int format, int ntrks, int d )
{
    fprintf ( out, "Header: Type=%d Tracks=%d", format, ntrks );
    division = d;

    if ( division > 0x8000 )
    {
        unsigned char smpte_rate = ( ( unsigned char ) ( ( -division ) >> 8 ) );
        unsigned char smpte_division = ( unsigned char ) ( division & 0xff );
        fprintf ( out, " SMPTE=%d Division=%d\n", smpte_rate, smpte_division );
    }

    else
    {
        fprintf ( out, " Division=%d\n", division );
    }
}

void MIDIFileShow::show_time ( MIDIClockTime time )
{
    if ( division > 0 )
    {
        unsigned long beat = time / division;
        unsigned long clk = time % division;
        fprintf ( out, "Time: %6ld:%3ld    ", beat, clk );
    }

    else
    {
        fprintf ( out, "Time: %9ld     ", time );
    }
}

//
// The possible events in a MIDI Files
//

void MIDIFileShow::mf_system_mode ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_note_on ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_note_off ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_poly_after ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_bender ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_program ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_chan_after ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

void MIDIFileShow::mf_control ( const MIDITimedMessage &msg )
{
    show_time ( msg.GetTime() );
    char buf[256];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
}

bool MIDIFileShow::mf_sysex ( MIDIClockTime time, int type, int len, unsigned char *s )
{
    show_time ( time );
    fprintf ( out, "SysEx  Type=%02X   Length=%d\n\n", type, len );

    for ( int i = 0; i < len; ++i )
    {
        if ( i > 0 && (i %16) == 0 )
            fprintf ( out, "\n" );

        fprintf ( out, "%02x ", ( int ) s[i] );
    }

    fprintf ( out, "\n\n" );
    return true;
}

bool MIDIFileShow::mf_metamisc ( MIDIClockTime time, int type, int len, unsigned char *d )
{
    // code for all miscellaneous meta events

    show_time ( time );

    switch ( type )
    {
    case MF_META_SEQUENCE_NUMBER:
        fprintf ( out, "Sequence Number  %d\n", To16Bit ( d[0], d[1] ) );
        return true;
    case MF_META_SMPTE:
        fprintf ( out, "SMPTE Event      %02x, %02x, %02x, %02x, %02x\n", d[0], d[1], d[2], d[3], d[4] );
        return true;
    case MF_META_TRACK_LOOP:
        fprintf ( out, "META-EVENT  TRACK_LOOP      Length=%d\n", len );
        break;
    case MF_META_OUTPUT_CABLE:
        fprintf ( out, "META-EVENT  OUTPUT_CABLE    Length=%d\n", len );
        break;
    case MF_META_CHANNEL_PREFIX:
        fprintf ( out, "META-EVENT  CHANNEL_PREFIX  Length=%d\n", len );
        break;
    default:
        fprintf ( out, "META-EVENT  TYPE=%d         Length=%d\n", type, len );
        break;
    }

    for ( int i = 0; i < len; ++i )
    {
        if ( i > 0 && (i %16) == 0 )
            fprintf ( out, "\n" );

        fprintf ( out, "%02x ", ( int ) d[i] );
    }

    fprintf ( out, "\n" );
    return true;
}

bool MIDIFileShow::mf_timesig ( MIDIClockTime time, int num, int den_pow, int clocks_per_metro, int notated_32nds_per_quarter_note )
{
    show_time ( time );
    fprintf ( out, "Time Signature   %d/%d  Clks/Metro.=%d 32nd/Quarter=%d\n",
              num,
              (1 << den_pow), // print denominator, not denominator power!
              clocks_per_metro,
              notated_32nds_per_quarter_note );
    return true;
}


bool MIDIFileShow::mf_tempo ( MIDIClockTime time, unsigned char a, unsigned char b, unsigned char c )
{
    unsigned long tempo = MIDIFile::To32Bit ( 0, a, b, c );
    show_time ( time );
    fprintf ( out, "Tempo              %4.2f BPM (%9ld usec/beat)\n", 60000000.0 / tempo, tempo );
    return true;
}

bool MIDIFileShow::mf_keysig ( MIDIClockTime time, int sf, int mi )
{
    show_time ( time );
    fprintf ( out, "Key Signature      " );

    if ( mi )
        fprintf ( out, "MINOR KEY  " );

    else
        fprintf ( out, "MAJOR KEY  " );

    if ( sf < 0 )
        fprintf ( out, "%d Flats\n", -sf );

    else
        fprintf ( out, "%d Sharps\n", sf );
    return true;
}

bool MIDIFileShow::mf_sqspecific ( MIDIClockTime time, int len, unsigned char *data )
{
    show_time ( time );
    fprintf ( out, "Sequencer Specific     Length=%d\n", len );

    if ( sqspecific_as_text )
    {
        std::string str( (const char *) data, len );
        fprintf ( out, "%s\n", str.c_str() );
    }
    else
    {
        fprintf ( out, "\n" );
        for ( int i = 0; i < len; ++i )
        {
            if ( i > 0 && (i %16) == 0 )
                fprintf ( out, "\n" );

            fprintf ( out, "%02x ", ( int ) data[i] );
        }

        fprintf ( out, "\n\n" );
    }

    return true;
}

bool MIDIFileShow::mf_text ( MIDIClockTime time, int type, int len, unsigned char *txt )
{
    static const char * text_event_names[16] =
    {
        "SEQ. #    ",
        "GENERIC   ",
        "COPYRIGHT ",
        "TRACK NAME",
        "INST. NAME",
        "LYRIC     ",
        "MARKER    ",
        "CUE       ",
        "PROGRAM   ",
        "DEVICE    ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   "
    };

    if ( type > 15 )
        type = 15;

    show_time ( time );
    fprintf ( out, "TEXT   %s  '%s'\n", text_event_names[type], ( char * ) txt );
    return true;
}

bool MIDIFileShow::mf_eot ( MIDIClockTime time )
{
    show_time ( time );
    fprintf ( out, "End Of Track\n" );
    return true;
}


}
