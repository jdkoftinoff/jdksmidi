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
// doxygen comments by N. Cassetta ncassetta@tiscali.it
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

/// This class is used internally for writing MIDI files. It is pure virtual and implements a stream of *char*
/// to be be written to a MIDI file
class MIDIFileWriteStream
{
  public:
    MIDIFileWriteStream();
    virtual ~MIDIFileWriteStream();

    /// To be overriden: sets the position of the next character to be written
    virtual long Seek( long pos, int whence = SEEK_SET ) = 0;

    /// To be overriden: writes a *char*
    virtual int WriteChar( int c ) = 0;
};

/// This class is used internally for writing MIDI files. It inherits from pure virtual MIDIWriteStream and
/// writes a stream of *char* to a FILE C object
// note by NC: I think this class should be declared deprecated: it has no correspective in MIDIFileRead,
// and its name generates confusion with MIDIFileReadStreamFile (which corresponds to it and MIDIFileWriteStreamFileName
// TODO: fix this
class MIDIFileWriteStreamFile : public MIDIFileWriteStream
{
  public:
    /// In the constructor you must specify an already opened FILE f
    MIDIFileWriteStreamFile( FILE *f_ );

    /// The destructor doesn't close the file
    virtual ~MIDIFileWriteStreamFile();

    long Seek( long pos, int whence = SEEK_SET );
    int WriteChar( int c );

  protected:
    FILE *f;
};

/// This class is used internally for writing MIDI files. It inherits from MIDIFileWriteStreamFile and writes
/// a stream of *char* to a FILE C object specified by its filename
class MIDIFileWriteStreamFileName : public MIDIFileWriteStreamFile
{
  public:

    /// In the constructor you must specify the filename.\ The constructor tries to open the FILE, you
    /// should call IsValid() for checking if it was successful
    MIDIFileWriteStreamFileName( const char *fname ) : MIDIFileWriteStreamFile( fopen( fname, "wb" ) )
    {
    }

#if 0 // MinGW says it's not defined, even including <wchar.h>
    MIDIFileWriteStreamFileName ( const wchar_t *fname ) : MIDIFileWriteStreamFile ( _wfopen ( fname, L"wb" ) )
    {
    }
#endif

    /// Returns *true* if the FILE was opened
    bool IsValid()
    {
        return f != 0;
    }

    /// The destructor closes the FILE
    virtual ~MIDIFileWriteStreamFileName()
    {
        if ( f )
        {
            fclose( f );
        }
    }
};

/// This class inherits from MIDIFile and converts MIDI data into a stream of *char*,
/// writing them to a MIDIFileWriteStream object
class MIDIFileWrite : protected MIDIFile
{
  public:
    /// In the constructor you must specify the MIDIFileWriteStream.\ The stream must be alreafy opem
    MIDIFileWrite( MIDIFileWriteStream *out_stream_ );

    /// The destructor doesn't destroy or close the MIDIFileWriteStream
    virtual ~MIDIFileWrite();

    /// Returns *true* if an write error occurred
    bool ErrorOccurred()
    {
        return error;
    }

    /// Returns the number of *char* currently written
    unsigned long GetFileLength()
    {
        return file_length;
    }

    /// Returns the number of *char* written in the current track
    unsigned long GetTrackLength()
    {
        return track_length;
    }

    /// Resets track lenght to 0
    void ResetTrackLength()
    {
        track_length = 0;
    }

    /// Resets track time to 0
    void ResetTrackTime()
    {
        track_time = 0;
    }

    /// \name Functions to write specific messages or data chunks
    //@{
    void WriteFileHeader( int format, int ntrks, int division );
    void WriteTrackHeader( unsigned long length );

    void WriteEvent( const MIDITimedBigMessage &m );

    void WriteSystemExclusiveEvent( const MIDITimedBigMessage &m );
    void WriteTextEvent( unsigned long time, unsigned char type, const char *text );
    void WriteMetaEvent( unsigned long time, unsigned char type, const unsigned char *data, long length );
    void WriteMetaMisc( const MIDITimedBigMessage &m );
    void WriteTempo( const MIDITimedBigMessage &m );

    void WriteKeySignature( unsigned long time, char sharp_flat, char minor );
    void WriteTimeSignature( unsigned long time,
                             unsigned char numerator = 4,
                             unsigned char denominator_power = 2,
                             unsigned char midi_clocks_per_metronome = 24,
                             unsigned char num_32nd_per_midi_quarter_note = 8 );

    void WriteEndOfTrack( unsigned long time );
    virtual void RewriteTrackLength();
    //@}

    /// False argument disable use running status in midi file (true on default)
    void UseRunningStatus( bool use )
    {
        use_running_status = use;
    }

  protected:
    virtual void Error( const char *s );

    void WriteCharacter( uchar c )
    {
        if ( out_stream->WriteChar( c ) < 0 )
            error = true;
    }

    void Seek( long pos )
    {
        if ( out_stream->Seek( pos ) < 0 )
            error = true;
    }

    void IncrementCounters( int c )
    {
        track_length += c;
        file_length += c;
    }

    void WriteShort( unsigned short c );
    void Write3Char( long c );
    void WriteLong( unsigned long c );
    int WriteVariableNum( unsigned long n );
    void WriteDeltaTime( unsigned long time );

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
