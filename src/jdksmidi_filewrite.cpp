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
#include "jdksmidi/filewrite.h"

#ifndef DEBUG_MDFWR
# define DEBUG_MDFWR 0
#endif

#if DEBUG_MDFWR
# undef DBG
# define DBG(a) a
#endif

namespace jdksmidi
{

MIDIFileWriteStream::MIDIFileWriteStream()
{
}

MIDIFileWriteStream::~MIDIFileWriteStream()
{
}

MIDIFileWriteStreamFile::MIDIFileWriteStreamFile ( FILE *f_ )
    : f ( f_ )
{
}

MIDIFileWriteStreamFile::~MIDIFileWriteStreamFile()
{
}

long MIDIFileWriteStreamFile::Seek ( long pos, int whence )
{
    return fseek ( f, pos, whence );
}

int MIDIFileWriteStreamFile::WriteChar ( int c )
{
    if ( fputc ( c, f ) == EOF )
    {
        return -1;
    }

    else
    {
        return 0;
    }
}


MIDIFileWrite::MIDIFileWrite ( MIDIFileWriteStream *out_stream_ )
    : out_stream ( out_stream_ )
{
    ENTER ( "MIDIFileWrite::MIDIFileWrite()" );
    file_length = 0;
    error = 0;
    track_length = 0;
    track_time = 0;
    running_status = 0;
    track_position = 0;
    use_running_status = true;
}

MIDIFileWrite::~MIDIFileWrite()
{
    ENTER ( "MIDIFileWrite::~MIDIFileWrite()" );
}

void MIDIFileWrite::Error ( const char *s )
{
    ENTER ( "void MIDIFileWrite::Error()" );
    // NULL method; can override.
    error = true;
}

void MIDIFileWrite::WriteShort ( unsigned short c )
{
    ENTER ( "void MIDIFileWrite::WriteShort()" );
    WriteCharacter ( ( unsigned char ) ( ( c >> 8 ) & 0xff ) );
    WriteCharacter ( ( unsigned char ) ( ( c & 0xff ) ) );
}

void MIDIFileWrite::Write3Char ( long c )
{
    ENTER ( "void MIDIFileWrite::Write3Char()" );
    WriteCharacter ( ( unsigned char ) ( ( c >> 16 ) & 0xff ) );
    WriteCharacter ( ( unsigned char ) ( ( c >> 8 ) & 0xff ) );
    WriteCharacter ( ( unsigned char ) ( ( c & 0xff ) ) );
}

void MIDIFileWrite::WriteLong ( unsigned long c )
{
    ENTER ( "void MIDIFileWrite::WriteLong()" );
    WriteCharacter ( ( unsigned char ) ( ( c >> 24 ) & 0xff ) );
    WriteCharacter ( ( unsigned char ) ( ( c >> 16 ) & 0xff ) );
    WriteCharacter ( ( unsigned char ) ( ( c >> 8 ) & 0xff ) );
    WriteCharacter ( ( unsigned char ) ( ( c & 0xff ) ) );
}

void MIDIFileWrite::WriteFileHeader (
    int format,
    int ntrks,
    int division
)
{
    ENTER ( "void MIDIFileWrite::WriteFileHeader()" );
    WriteCharacter ( ( unsigned char ) 'M' );
    WriteCharacter ( ( unsigned char ) 'T' );
    WriteCharacter ( ( unsigned char ) 'h' );
    WriteCharacter ( ( unsigned char ) 'd' );
    WriteLong ( 6 );
    WriteShort ( ( short ) format );
    WriteShort ( ( short ) ntrks );
    WriteShort ( ( short ) division );
    file_length = 4 + 4 + 6;
}

void MIDIFileWrite::WriteTrackHeader ( unsigned long length )
{
    ENTER ( "void MIDIFileWrite::WriteTrackHeader()" );
    track_position = file_length;
    track_length = 0;
    track_time = 0;
    running_status = 0;
    WriteCharacter ( ( unsigned char ) 'M' );
    WriteCharacter ( ( unsigned char ) 'T' );
    WriteCharacter ( ( unsigned char ) 'r' );
    WriteCharacter ( ( unsigned char ) 'k' );
    WriteLong ( length );
    file_length += 8;
    within_track = true;
}

int MIDIFileWrite::WriteVariableNum ( unsigned long n )
{
    ENTER ( "short MIDIFileWrite::WriteVariableNum()" );
    register unsigned long buffer;
    short cnt = 0;
    buffer = n & 0x7f;

    while ( ( n >>= 7 ) > 0 )
    {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += ( n & 0x7f );
    }

    while ( true )
    {
        WriteCharacter ( ( unsigned char ) ( buffer & 0xff ) );
        cnt++;

        if ( buffer & 0x80 )
            buffer >>= 8;

        else
            break;
    }

    return cnt;
}

void MIDIFileWrite::WriteDeltaTime ( unsigned long abs_time )
{
    ENTER ( "void MIDIFileWrite::WriteDeltaTime()" );
    long dtime = abs_time - track_time;

    if ( dtime < 0 )
    {
        Error( "Events out of order" );
        dtime = 0;
    }

    IncrementCounters ( WriteVariableNum ( dtime ) );
    track_time = abs_time;
}

void MIDIFileWrite::WriteEvent ( const MIDITimedBigMessage &m )
{
    ENTER ( "void MIDIFileWrite::WriteEvent()" );

    if ( m.IsMetaEvent() ) // all Meta Events
    {
        // if this Meta Event has a sysex buffer attached, this
        // buffer contains the raw midi file meta data
        if ( m.GetSysEx() )
        {
            WriteMetaEvent ( m.GetTime(), m.GetMetaType(), m.GetSysEx()->GetBuf(), m.GetSysEx()->GetLengthSE() );
            return;
        }

        // otherwise, it is a type of meta event that doesnt have sysex data...

        if ( m.IsTempo() )
        {
            WriteTempo ( m );
        }

        else if ( m.IsKeySig() )
        {
            WriteKeySignature ( m.GetTime(), m.GetKeySigSharpFlats(), m.GetKeySigMajorMinor() );
        }

        else if ( m.IsTimeSig() )
        {
            // numerator in byte2 (denomenator in byte3), but denominator power in byte4
            WriteTimeSignature ( m.GetTime(), m.GetByte2(), m.GetByte4(), m.GetByte5(), m.GetByte6() );
        }

        else // all other meta events
        {
            WriteMetaMisc( m );
        }

        return;
    }

    // else not Meta Events: all System Exclusive Events and all Channel Events

    // all System Exclusive Events

    if ( m.IsSystemExclusive() && m.GetSysEx() ) // IsSysEx() or IsSysExA()
    {
        WriteSystemExclusiveEvent ( m );
        return;
    }

    // else all Channel Events

    int len = m.GetLengthMSG();

    if ( len > 0 )
    {
        WriteDeltaTime ( m.GetTime() );

        unsigned char status = m.GetStatus();

        if ( running_status != status )
        {
            WriteCharacter ( status );
            IncrementCounters ( 1 );
            running_status = status;
            if ( !use_running_status )
                running_status = 0;
        }

        if ( len > 1 )
        {
            WriteCharacter ( m.GetByte1() );
            IncrementCounters ( 1 );
        }

        if ( len > 2 )
        {
            WriteCharacter ( m.GetByte2() );
            IncrementCounters ( 1 );
        }
    }
}

void MIDIFileWrite::WriteSystemExclusiveEvent ( const MIDITimedBigMessage &m )
{
    ENTER ( "void MIDIFileWrite::WriteSystemExclusiveEvent()" );
    WriteDeltaTime ( m.GetTime() );

    WriteCharacter ( m.GetStatus() ); // write System Exclusive Event type (0xF0 or 0xF7)
    IncrementCounters ( 1 );

    int len = m.GetSysEx()->GetLengthSE();
    IncrementCounters ( WriteVariableNum ( len ) );

    for ( int i = 0; i < len; i++ )
    {
        WriteCharacter ( m.GetSysEx()->GetData ( i ) );
    }
    IncrementCounters ( len );

    running_status = 0;
}

void MIDIFileWrite::WriteTextEvent ( unsigned long time, unsigned char type, const char *text )
{
    ENTER ( "void MIDIFileWrite::WriteTextEvent()" );
    WriteDeltaTime ( time );

    WriteCharacter ( META_EVENT );
    WriteCharacter ( type ); // Text event type
    IncrementCounters ( 2 );

    int len = strlen ( text );
    IncrementCounters ( WriteVariableNum ( len ) );

    while ( *text )
    {
        WriteCharacter ( ( unsigned char ) *text++ );
    }
    IncrementCounters ( len );

    running_status = 0;
}

void MIDIFileWrite::WriteMetaEvent ( unsigned long time, unsigned char type, const unsigned char *data, long length )
{
    ENTER ( "void MIDIFileWrite::WriteMetaEvent()" );
    WriteDeltaTime ( time );

    WriteCharacter ( META_EVENT );
    WriteCharacter ( type ); // Meta-event type
    IncrementCounters ( 2 );

    IncrementCounters ( WriteVariableNum ( length ) );

    for ( int i = 0; i < length; i++ )
    {
        WriteCharacter ( data[i] );
    }

    IncrementCounters ( length );
    running_status = 0;
}

void MIDIFileWrite::WriteMetaMisc ( const MIDITimedBigMessage &m )
{
    ENTER ( "void MIDIFileWrite::WriteMetaMisc()" );

    int len = m.GetDataLength();
    if ( len > 5 )
        return; // not valid meta event, do'nt write to output midifile!

    WriteDeltaTime ( m.GetTime() );

    WriteCharacter ( m.GetStatus() );
    WriteCharacter ( m.GetByte1() );
    WriteCharacter ( len ); // length of event

    if ( len > 0 )
        WriteCharacter ( m.GetByte2() );

    if ( len > 1 )
        WriteCharacter ( m.GetByte3() );

    if ( len > 2 )
        WriteCharacter ( m.GetByte4() );

    if ( len > 3 )
        WriteCharacter ( m.GetByte5() );

    if ( len > 4 )
        WriteCharacter ( m.GetByte6() );

    IncrementCounters ( len + 3 );
    running_status = 0;
}

void MIDIFileWrite::WriteTempo ( const MIDITimedBigMessage &m )
{
    ENTER ( "void MIDIFileWrite::WriteTempo()" );
    WriteDeltaTime ( m.GetTime() );

    WriteCharacter ( m.GetStatus() ); // META_EVENT
    WriteCharacter ( m.GetByte1() );  // META_TEMPO
    WriteCharacter ( 3 ); // length of event
    WriteCharacter ( m.GetByte2() ); // a
    WriteCharacter ( m.GetByte3() ); // b
    WriteCharacter ( m.GetByte4() ); // c
    IncrementCounters ( 6 );

    running_status = 0;
}

void MIDIFileWrite::WriteKeySignature ( unsigned long time, char sharp_flat, char minor )
{
    ENTER ( "void MIDIFileWrite::WriteKeySignature()" );
    WriteDeltaTime ( time );

    WriteCharacter ( META_EVENT );
    WriteCharacter ( META_KEYSIG );
    WriteCharacter ( 2 );  // length of event
    WriteCharacter ( ( unsigned char ) sharp_flat ); // - for flats, + for sharps
    WriteCharacter ( ( unsigned char ) minor ); // 1 if minor key
    IncrementCounters ( 5 );

    running_status = 0;
}

void MIDIFileWrite::WriteTimeSignature (
    unsigned long time,
    unsigned char numerator,
    unsigned char denominator_power,
    unsigned char midi_clocks_per_metronome,
    unsigned char num_32nd_per_midi_quarter_note )
{
    ENTER ( "void MIDIFileWrite::WriteTimeSignature()" );
    WriteDeltaTime ( time );

    WriteCharacter ( META_EVENT );
    WriteCharacter ( META_TIMESIG );
    WriteCharacter ( 4 );  // length of event
    WriteCharacter ( numerator );
    WriteCharacter ( denominator_power );
    WriteCharacter ( midi_clocks_per_metronome );
    WriteCharacter ( num_32nd_per_midi_quarter_note );
    IncrementCounters ( 7 );

    running_status = 0;
}

void MIDIFileWrite::WriteEndOfTrack ( unsigned long time )
{
    ENTER ( "void MIDIFileWrite::WriteEndOfTrack()" );

    if ( within_track == true )
    {
        if ( time == 0 )
            time = track_time;

        WriteDeltaTime ( time );

        WriteCharacter ( META_EVENT );
        WriteCharacter ( META_END_OF_TRACK );
        WriteCharacter ( 0 );  // length of event
        IncrementCounters ( 3 );

        within_track = false;
        running_status = 0;
    }
}

void MIDIFileWrite::RewriteTrackLength()
{
    ENTER ( "void MIDIFileWrite::RewriteTrackLength()" );
    // go back and patch in the tracks length into the track chunk
    // header, now that we know the proper value.
    // then make sure we go back to the end of the file
    Seek ( track_position + 4 );
    WriteLong ( track_length );
    Seek ( track_position + 8 + track_length );
}

}
