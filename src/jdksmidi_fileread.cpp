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
#include "jdksmidi/fileread.h"

// Standard MIDI-File Format Spec. 1.1, page 9 of 18:
// "Sysex events and meta events cancel any running status which was in effect.
// Running status does not apply to and may not be used for these messages."

// to do: decide which way is right for this flag and fix it - The standard midi file format specs are (were?) unclear
#define MIDIFRD_ALLOW_STATUS_ACROSS_META_EVENT 0 // correct value is 0 !

namespace jdksmidi
{

void MIDIFileEvents::UpdateTime ( MIDIClockTime delta_time )
{
}

bool MIDIFileEvents::ChanMessage ( const MIDITimedMessage &msg )
{
    switch ( msg.GetStatus() & 0xf0 )
    {
    case NOTE_OFF:
        mf_note_off ( msg );
        break;

    case NOTE_ON:
        mf_note_on ( msg );
        break;

    case POLY_PRESSURE:
        mf_poly_after ( msg );
        break;

    case CONTROL_CHANGE:
        if ( msg.GetByte2() > C_ALL_NOTES_OFF ) // TODO@VRM may be (msg.GetByte1() >= C_ALL_SOUNDS_OFF) ??
        {
            mf_system_mode ( msg );
        }
        else
        {
            mf_control ( msg );
        }
        break;

    case PROGRAM_CHANGE:
        mf_program ( msg );
        break;

    case CHANNEL_PRESSURE:
        mf_chan_after ( msg );
        break;

    case PITCH_BEND:
        mf_bender ( msg );
        break;
    }

    return true;
}

bool MIDIFileEvents::MetaEvent ( MIDIClockTime time, int type, int leng, unsigned char *m )
{
    switch ( type )
    {
    case MF_META_GENERIC_TEXT:
    case MF_META_COPYRIGHT:
    case MF_META_TRACK_NAME:
    case MF_META_INSTRUMENT_NAME:
    case MF_META_LYRIC_TEXT:
    case MF_META_MARKER_TEXT:
    case MF_META_CUE_POINT:
    case MF_META_GENERIC_TEXT_8:
    case MF_META_GENERIC_TEXT_9:
    case MF_META_GENERIC_TEXT_A:
    case MF_META_GENERIC_TEXT_B:
    case MF_META_GENERIC_TEXT_C:
    case MF_META_GENERIC_TEXT_D:
    case MF_META_GENERIC_TEXT_E:
    case MF_META_GENERIC_TEXT_F:
        // These are all text events
        m[leng] = '\0'; // make sure string ends in NULL
        return mf_text ( time, type, leng, m );

    case MF_META_END_OF_TRACK: // End of Track
        return mf_eot ( time );

    case MF_META_TEMPO:
        return mf_tempo ( time, m[0], m[1], m[2] );

    case MF_META_TIMESIG:
        return mf_timesig ( time, m[0], m[1], m[2], m[3] );

    case MF_META_KEYSIG:
        return mf_keysig ( time, (char) m[0], m[1] );

    case MF_META_SEQUENCER_SPECIFIC:
        return mf_sqspecific ( time, leng, m );

    case MF_META_SEQUENCE_NUMBER:
    case MF_META_SMPTE:
    case MF_META_TRACK_LOOP:
    case MF_META_OUTPUT_CABLE:
    case MF_META_CHANNEL_PREFIX:
    default:
        return mf_metamisc ( time, type, leng, m );
        break;
    }
    return true;
}

void MIDIFileEvents::mf_starttrack ( int trk )
{
}

void MIDIFileEvents::mf_endtrack ( int trk )
{
}

bool MIDIFileEvents::mf_eot ( MIDIClockTime time )
{
    return true;
}

void MIDIFileEvents::mf_error ( const char *s )
{
}

void MIDIFileEvents::mf_header ( int a, int b, int c )
{
}

bool MIDIFileEvents::mf_metamisc ( MIDIClockTime time, int a, int b, unsigned char *s )
{
    return true;
}

bool MIDIFileEvents::mf_timesig ( MIDIClockTime time, int a, int b, int c, int d )
{
    return true;
}

bool MIDIFileEvents::mf_tempo ( MIDIClockTime time, unsigned char a, unsigned char b, unsigned char c )
{
    return true;
}

bool MIDIFileEvents::mf_keysig ( MIDIClockTime time, int a, int b )
{
    return true;
}

bool MIDIFileEvents::mf_sqspecific ( MIDIClockTime time, int a, unsigned char *s )
{
    return true;
}

bool MIDIFileEvents::mf_text ( MIDIClockTime time, int a, int b, unsigned char *s )
{
    return true;
}

void MIDIFileEvents::mf_system_mode ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_note_on ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_note_off ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_poly_after ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_bender ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_program ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_chan_after ( const MIDITimedMessage &msg )
{
}

void MIDIFileEvents::mf_control ( const MIDITimedMessage &msg )
{
}

bool MIDIFileEvents::mf_sysex ( MIDIClockTime time, int type, int len, unsigned char *s )
{
    return true;
}





MIDIFileRead::MIDIFileRead (
    MIDIFileReadStream *input_stream_,
    MIDIFileEvents *event_handler_,
    unsigned long max_msg_len_
)
    :
    input_stream ( input_stream_ ),
    event_handler ( event_handler_ )
{
    // setup data
    cur_time = 0;
    skip_init = 1;
    to_be_read = 0;
    act_msg_len = 0;
    cur_track = 0;
    abort_parse = 0;
    used_running_status = false;

    max_msg_len = max_msg_len_;
    the_msg = new unsigned char[max_msg_len];
}

MIDIFileRead::~MIDIFileRead()
{
    jdks_safe_delete_array( the_msg );
}

void MIDIFileRead::mf_error ( const char *e )
{
    event_handler->mf_error ( e );
    abort_parse = true;
}

void MIDIFileRead::Reset()
{
    // setup data
    cur_time = 0;
    skip_init = 1;
    to_be_read = 0;
    act_msg_len = 0;
    cur_track = 0;
    abort_parse = 0;
    used_running_status = false;

    // rewind input stream
    input_stream->Rewind();
}

int MIDIFileRead::ReadNumTracks()
{
    Reset();
    return ReadHeader();
}

bool MIDIFileRead::Parse()
{
    Reset();

    int n = ReadHeader();

    if ( n <= 0 )
    {
        mf_error ( "No Tracks" );
        return false;
    }

    for ( cur_track = 0; cur_track < n; cur_track++ )
    {
        ReadTrack();

        if ( abort_parse )
        {
            return false;
        }
    }

    return true;
}

int MIDIFileRead::ReadMT ( unsigned long type, int skip )
{
    unsigned long read = 0;
    int c;
    read = OSTYPE ( EGetC(), EGetC(), EGetC(), EGetC() );

    if ( type != read )
    {
        if ( skip )
        {
            do
            {
                read <<= 8;
                c = EGetC();
                read |= c;

                if ( read == type )
                    return true;

                if ( abort_parse )
                    return false;
            }
            while ( c != -1 );
        }

        mf_error ( "Error looking for chunk type" );
        return false;
    }

    else
    {
        return true;
    }
}

int MIDIFileRead::ReadHeader()
{
    int the_format;
    int ntrks;
    int division;

    if ( ReadMT ( _MThd, skip_init ) == 0xffff )
        return 0;

    if ( abort_parse )
        return 0;

    to_be_read = Read32Bit();
    the_format = Read16Bit();
    ntrks = Read16Bit();
    division = Read16Bit();

    if ( abort_parse )
        return 0;

    // silently fix error if midi file have format = 0 and ntrks > 1
    if ( the_format == 0 && ntrks > 1 )
        the_format = 1;

    header_format = the_format;
    header_ntrks = ntrks;

    header_division = division;
    event_handler->mf_header ( the_format, ntrks, division );
    // printf( "\nto be read = %d\n", to_be_read );

    while ( to_be_read > 0 )
        EGetC();

    return ntrks;
}

//
// read a track chunk
//

void MIDIFileRead::ReadTrack()
{
    //
    // This array is indexed by the high half of a status byte.
    // Its/ value is either the number of bytes needed (1 or 2) for a channel message,
    // or 0 (meaning it's not a channel message).
    //
    static char chantype[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,  // 0x00 through 0x70
        2, 2, 2, 2, 1, 1, 2, 0   // 0x80 through 0xF0
    };
    unsigned long lookfor, lng;
    int c, c1, type;
    int running = 0; // 1 when running status used
    int status = 0;  // (possible running) status byte
    int needed;      // number of bytes needed (1 or 2) for a channel message, or 0 if not a channel message

    if ( ReadMT ( _MTrk, 0 ) == 0xFFFF )
        return;

    to_be_read = Read32Bit();
    cur_time = 0;
    event_handler->mf_starttrack ( cur_track );

    while ( to_be_read > 0 && !abort_parse )
    {
        unsigned long deltat = ReadVariableNum();
        event_handler->UpdateTime ( deltat );
        cur_time += deltat;
        c = EGetC();

        if ( c == -1 )
            break;

        if ( ( c & 0x80 ) == 0 )
        {
            if ( status == 0 )
                mf_error ( "Unexpected Running Status" );

            running = 1;
            used_running_status = true;
            needed = chantype[ ( status>>4 ) & 0x0F ];
        }
        else
        {
            status = c;
            running = 0;
            needed = chantype[ ( status>>4 ) & 0x0F ];
        }

        if ( needed ) // ie. is it a channel message?
        {
            if ( running ) c1 = c;
            else           c1 = EGetC();
            if ( !FormChanMessage ( status, c1, (needed > 1)? EGetC():0 ) )
                abort_parse = true;
            continue;
        }

        // else System Exclusive Event or Meta Event:

        switch ( status )
        {
        case 0xFF: // META_EVENT
            type = EGetC();
            lng = ReadVariableNum();
            if ( lng > to_be_read )
            {
                // false variable length in midifile, thanks to Stephan.Huebler@tu-dresden.de
                mf_error ( "Variable length incorrect" );
                abort_parse = true;
                break;
            }
            lookfor = to_be_read - lng;
            MsgInit();
            while ( to_be_read > lookfor )
                MsgAdd ( EGetC() );

            if ( !event_handler->MetaEvent ( cur_time, type, act_msg_len, the_msg ) )
                abort_parse = true;
            break;

        case 0xF0: // SYSEX_START
        case 0xF7: // SYSEX_START_A
            type = status;
            lng = ReadVariableNum();
            if ( lng > to_be_read )
            {
                mf_error ( "Variable length incorrect" );
                abort_parse = true;
                break;
            }
            lookfor = to_be_read - lng;
            MsgInit();
            while ( to_be_read > lookfor )
                MsgAdd ( EGetC() );

            if ( !event_handler->mf_sysex ( cur_time, type, act_msg_len, the_msg ) )
                abort_parse = true;
            break;

        default:
            mf_error ( "Unexpected status byte" );
            abort_parse = true;
            break;
        }
    }

    event_handler->mf_endtrack ( cur_track );
    return;
}

unsigned long MIDIFileRead::ReadVariableNum()
{
    unsigned long value;
    int c;
    c = EGetC();

    if ( c == -1 )
    {
        return 0;
    }

    value = c;

    if ( c & 0x80 )
    {
        value &= 0x7f;

        do
        {
            c = EGetC();
            value = ( value << 7 ) + ( c & 0x7f );
        }
        while ( c & 0x80 );
    }

    return value;
}

unsigned long MIDIFileRead::Read32Bit()
{
    int c1, c2, c3, c4;
    c1 = EGetC();
    c2 = EGetC();
    c3 = EGetC();
    c4 = EGetC();
    return To32Bit ( ( unsigned char ) c1, ( unsigned char ) c2, ( unsigned char ) c3, ( unsigned char ) c4 );
}

int MIDIFileRead::Read16Bit()
{
    int c1, c2;
    c1 = EGetC();
    c2 = EGetC();
    return To16Bit ( ( unsigned char ) c1, ( unsigned char ) c2 );
}

int MIDIFileRead::EGetC()
{
    int c;
    c = input_stream->ReadChar();

    if ( c < 0 )
    {
        mf_error ( "Unexpected Stream Error" );
        abort_parse = true;
        return -1;
    }

    --to_be_read;
    return ( int ) c;
}

void MIDIFileRead::MsgAdd ( int a )
{
    if ( act_msg_len < (max_msg_len-1) ) // spare 1 byte for last NULL char
        the_msg[ act_msg_len++ ] = ( unsigned char ) a;
}

void MIDIFileRead::MsgInit()
{
    act_msg_len = 0;
}

bool MIDIFileRead::FormChanMessage ( unsigned char st, unsigned char b1, unsigned char b2 )
{
    MIDITimedMessage m;
    m.SetStatus ( st );
    m.SetByte1 ( b1 );
    m.SetByte2 ( b2 );
    m.SetTime ( cur_time );

    if ( st >= 0x80 && st < 0xf0 )
    {
        return event_handler->ChanMessage ( m );
    }

    return false;
}


}
