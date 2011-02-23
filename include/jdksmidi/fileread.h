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

#ifndef JDKSMIDI_FILEREAD_H
#define JDKSMIDI_FILEREAD_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/file.h"

namespace jdksmidi
{

class MIDIFileReadStream;
class MIDIFileReadStreamFile;
class MIDIFileEvents;
class MIDIFileRead;


class MIDIFileReadStream
{
public:
    MIDIFileReadStream()
    {
    }

    virtual ~MIDIFileReadStream()
    {
    }

    virtual void Rewind() = 0;

    virtual int ReadChar() = 0;
};

class MIDIFileReadStreamFile : public MIDIFileReadStream
{
public:
    explicit MIDIFileReadStreamFile ( const char *fname )
    {
        f = fopen ( fname, "rb" );
    }

#ifdef WIN32
    explicit MIDIFileReadStreamFile ( const wchar_t *fname )
    {
        f = _wfopen ( fname, L"rb" );
    }
#endif

    explicit MIDIFileReadStreamFile ( FILE *f_ ) : f ( f_ )
    {
    }

    virtual ~MIDIFileReadStreamFile()
    {
        if ( f ) fclose ( f );
    }

    virtual void Rewind()
    {
        if ( f ) rewind ( f );
    }

    bool IsValid()
    {
        return f != 0;
    }

    virtual int ReadChar()
    {
        int r = -1;

        if ( f && !feof ( f ) && !ferror ( f ) )
        {
            r = fgetc ( f );
        }

        return r;
    }


private:
    FILE *f;
};

class MIDIFileEvents : protected MIDIFile
{
public:
    MIDIFileEvents()
    {
    }

    virtual ~MIDIFileEvents()
    {
    }


//
// The possible events in a MIDI Files
//

    virtual void mf_system_mode ( const MIDITimedMessage &msg );
    virtual void mf_note_on ( const MIDITimedMessage &msg );
    virtual void mf_note_off ( const  MIDITimedMessage &msg );
    virtual void mf_poly_after ( const MIDITimedMessage &msg );
    virtual void mf_bender ( const MIDITimedMessage &msg );
    virtual void mf_program ( const MIDITimedMessage &msg );
    virtual void mf_chan_after ( const MIDITimedMessage &msg );
    virtual void mf_control ( const MIDITimedMessage &msg );

    virtual bool mf_metamisc ( MIDIClockTime time, int type, int len, unsigned char *data );
    virtual bool mf_timesig ( MIDIClockTime time, int, int, int, int );
    virtual bool mf_tempo ( MIDIClockTime time, unsigned char a, unsigned char b, unsigned char c );

    virtual bool mf_keysig ( MIDIClockTime time, int, int );
    virtual bool mf_sqspecific ( MIDIClockTime time, int, unsigned char * );
    virtual bool mf_text ( MIDIClockTime time, int, int, unsigned char * );
    virtual bool mf_eot ( MIDIClockTime time );
    virtual bool mf_sysex ( MIDIClockTime time, int type, int len, unsigned char *s );

//
// the following methods are to be overridden for your specific purpose
//

    virtual void mf_error ( const char * );

    virtual void mf_starttrack ( int trk );
    virtual void mf_endtrack ( int trk );
    virtual void mf_header ( int, int, int );

//
// Higher level dispatch functions
//
    virtual void UpdateTime ( MIDIClockTime delta_time );
    virtual bool MetaEvent ( MIDIClockTime time, int type, int len, unsigned char *buf );
    virtual bool ChanMessage ( const MIDITimedMessage &msg);
    virtual void SortEventsOrder() {}

};

class MIDIFileRead : protected MIDIFile
{
public:
    MIDIFileRead (
        MIDIFileReadStream *input_stream_,
        MIDIFileEvents *event_handler_,
        unsigned long max_msg_len = 8192
    );
    virtual ~MIDIFileRead();

    // return false if not enough number of tracks or events in any track
    virtual bool Parse();

    // read midifile header, return number of tracks
    int ReadNumTracks();

    int GetFormat() const
    {
        return header_format;
    }
    int GetNumTracks() const
    {
        return header_ntrks;
    }
    // call it after Parse(): return true if file contain event(s) with running status
    bool UsedRunningStatus() const
    {
        return used_running_status;
    }
    // return header_division = clock per beat value for range 1...32767
    int GetDivision() const
    {
        return header_division;
    }

protected:
    virtual int ReadHeader();
    virtual void mf_error ( const char * );

    MIDIClockTime cur_time;
    int skip_init;
    unsigned long to_be_read;
    int cur_track;
    int abort_parse;

    unsigned char *the_msg;
    int max_msg_len;
    int act_msg_len; // actual msg length

private:
    unsigned long ReadVariableNum();
    unsigned long Read32Bit();
    int Read16Bit();

    void ReadTrack();

    void MsgAdd ( int );
    void MsgInit();

    int EGetC();

    int ReadMT ( unsigned long, int );

    bool FormChanMessage ( unsigned char st, unsigned char b1, unsigned char b2 );
    // reset data for multiple parse
    void Reset();

    bool used_running_status;

    int header_format;
    int header_ntrks;
    int header_division;

    MIDIFileReadStream *input_stream;
    MIDIFileEvents *event_handler;
};
}

#endif

