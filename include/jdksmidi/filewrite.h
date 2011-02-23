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

#ifndef JDKSMIDI_FILEWRITE_H
#define JDKSMIDI_FILEWRITE_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/file.h"

namespace jdksmidi
{

class MIDIFileWriteStream;
class MIDIFileWriteStreamFile;
class MIDIFileWrite;

class MIDIFileWriteStream
{
public:
    MIDIFileWriteStream();
    virtual ~MIDIFileWriteStream();

    virtual long Seek ( long pos, int whence = SEEK_SET ) = 0;
    virtual int WriteChar ( int c ) = 0;
};

class MIDIFileWriteStreamFile : public MIDIFileWriteStream
{
public:
    MIDIFileWriteStreamFile ( FILE *f_ );
    virtual ~MIDIFileWriteStreamFile();

    long Seek ( long pos, int whence = SEEK_SET );
    int WriteChar ( int c );
protected:
    FILE *f;
};

class MIDIFileWriteStreamFileName : public MIDIFileWriteStreamFile
{
public:
    MIDIFileWriteStreamFileName ( const char *fname ) : MIDIFileWriteStreamFile ( fopen ( fname, "wb" ) )
    {
    }

#ifdef WIN32
    MIDIFileWriteStreamFileName ( const wchar_t *fname ) : MIDIFileWriteStreamFile ( _wfopen ( fname, L"wb" ) )
    {
    }
#endif

    bool IsValid()
    {
        return f != 0;
    }

    virtual ~MIDIFileWriteStreamFileName()
    {
        if ( f )
        {
            fclose ( f );
        }
    }

};

class MIDIFileWrite : protected MIDIFile
{
public:
    MIDIFileWrite ( MIDIFileWriteStream *out_stream_ );
    virtual ~MIDIFileWrite();

    bool ErrorOccurred()
    {
        return error;
    }
    unsigned long GetFileLength()
    {
        return file_length;
    }
    unsigned long GetTrackLength()
    {
        return track_length;
    }
    void ResetTrackLength()
    {
        track_length = 0;
    }
    void ResetTrackTime()
    {
        track_time = 0;
    }

    void WriteFileHeader ( int format, int ntrks, int division );
    void WriteTrackHeader ( unsigned long length );

    void WriteEvent ( const MIDITimedBigMessage &m );

    void WriteSystemExclusiveEvent ( const MIDITimedBigMessage &m );
    void WriteTextEvent ( unsigned long time, unsigned char type, const char *text );
    void WriteMetaEvent ( unsigned long time, unsigned char type, const unsigned char *data, long length );
    void WriteMetaMisc ( const MIDITimedBigMessage &m );
    void WriteTempo ( const MIDITimedBigMessage &m );

    void WriteKeySignature ( unsigned long time, char sharp_flat, char minor );
    void WriteTimeSignature (
        unsigned long time,
        unsigned char numerator = 4,
        unsigned char denominator_power = 2,
        unsigned char midi_clocks_per_metronome = 24,
        unsigned char num_32nd_per_midi_quarter_note = 8 );

    void WriteEndOfTrack ( unsigned long time );
    virtual void RewriteTrackLength();
    // false argument disable use running status in midi file (true on default)
    void UseRunningStatus( bool use )
    {
        use_running_status = use;
    }

protected:
    virtual void Error ( const char *s );

    void WriteCharacter ( uchar c )
    {
        if ( out_stream->WriteChar ( c ) < 0 )
            error = true;
    }

    void Seek ( long pos )
    {
        if ( out_stream->Seek ( pos ) < 0 )
            error = true;
    }

    void IncrementCounters ( int c )
    {
        track_length += c;
        file_length += c;
    }

    void WriteShort ( unsigned short c );
    void Write3Char ( long c );
    void WriteLong ( unsigned long c );
    int WriteVariableNum ( unsigned long n );
    void WriteDeltaTime ( unsigned long time );

private:
    bool use_running_status; // true on default
    bool error;
    bool within_track;
    unsigned long file_length;
    unsigned long track_length;
    unsigned long track_time;
    unsigned long track_position;
    uchar running_status;

    MIDIFileWriteStream *out_stream;
};
}

#endif


