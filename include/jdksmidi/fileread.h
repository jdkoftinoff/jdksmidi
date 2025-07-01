/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004-2025  Jeffrey Koftinoff
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE MIT LICENSE ***
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
/*
** Copyright 1986 to 1998 By Jeffrey Koftinoff
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by Jeffrey Koftinoff
**
*/

#ifndef JDKSMIDI_FILEREAD_H
#define JDKSMIDI_FILEREAD_H

#include "jdksmidi/file.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <vector>

namespace jdksmidi {
class MIDIFileReadStream;
class MIDIFileReadStreamFile;
class MIDIFileEvents;
class MIDIFileRead;

class MIDIFileReadStream
{
  public:
    MIDIFileReadStream() {}

    virtual ~MIDIFileReadStream() {}

    virtual int ReadChar() = 0;
};

class MIDIFileReadStreamFile : public MIDIFileReadStream
{
  public:
    explicit MIDIFileReadStreamFile(char const* fname) { f = fopen(fname, "rb"); }

    explicit MIDIFileReadStreamFile(FILE* f_)
        : f(f_)
    {}

    virtual ~MIDIFileReadStreamFile()
    {
        if (f) {
            fclose(f);
        }
    }

    virtual int ReadChar()
    {
        int r = -1;

        if (f && !feof(f) && !ferror(f)) {
            r = fgetc(f);
        }

        return r;
    }

  private:
    FILE* f;
};

class MIDIFileEvents : protected MIDIFile
{
  public:
    MIDIFileEvents() {}

    virtual ~MIDIFileEvents() {}

    //
    // The possible events in a MIDI Files
    //

    virtual void mf_system_mode(MIDITimedMessage const& msg);
    virtual void mf_note_on(MIDITimedMessage const& msg);
    virtual void mf_note_off(MIDITimedMessage const& msg);
    virtual void mf_poly_after(MIDITimedMessage const& msg);
    virtual void mf_bender(MIDITimedMessage const& msg);
    virtual void mf_program(MIDITimedMessage const& msg);
    virtual void mf_chan_after(MIDITimedMessage const& msg);
    virtual void mf_control(MIDITimedMessage const& msg);
    virtual void mf_sysex(MIDIClockTime time, MIDISystemExclusive const& ex);

    virtual void mf_arbitrary(MIDIClockTime time, int len, unsigned char* data);
    virtual void mf_metamisc(MIDIClockTime time, int, int, unsigned char*);
    virtual void mf_seqnum(MIDIClockTime time, int);
    virtual void mf_smpte(MIDIClockTime time, int, int, int, int, int);
    virtual void mf_timesig(MIDIClockTime time, int, int, int, int);
    virtual void mf_tempo(MIDIClockTime time, unsigned long tempo);
    virtual void mf_keysig(MIDIClockTime time, int, int);
    virtual void mf_sqspecific(MIDIClockTime time, int, unsigned char*);
    virtual void mf_text(MIDIClockTime time, int, int, unsigned char*);
    virtual void mf_eot(MIDIClockTime time);

    //
    // the following methods are to be overridden for your specific purpose
    //

    virtual void mf_error(char const*);

    virtual void mf_starttrack(int trk);
    virtual void mf_endtrack(int trk);
    virtual void mf_header(int, int, int);

    //
    // Higher level dispatch functions
    //
    virtual void UpdateTime(MIDIClockTime delta_time);
    virtual void MetaEvent(MIDIClockTime time, int type, int len, unsigned char* buf);
    virtual void ChanMessage(MIDITimedMessage const& msg);
};

class MIDIFileRead : protected MIDIFile
{
  public:
    MIDIFileRead(
        MIDIFileReadStream* input_stream_,
        MIDIFileEvents* event_handler_,
        unsigned long max_msg_len = 8192);
    virtual ~MIDIFileRead();

    virtual bool Parse();

    int GetFormat() { return header_format; }
    int GetNumberTracks() { return header_ntrks; }
    int GetDivision() { return header_division; }

  protected:
    virtual int ReadHeader();

    virtual void mf_error(char const*);

  protected:
    int no_merge;
    MIDIClockTime cur_time;
    int skip_init;
    unsigned long to_be_read;
    int cur_track;
    int abort_parse;

    std::vector<unsigned char> message_buffer;
    int msg_index;

  private:
    unsigned long ReadVariableNum();
    unsigned long Read32Bit();
    int Read16Bit();

    void ReadTrack();

    void MsgAdd(int);
    void MsgInit();

    int EGetC();

    int ReadMT(unsigned long, int);
    void BadByte(int);

    void FormChanMessage(unsigned char st, unsigned char b1, unsigned char b2);

    int header_format;
    int header_ntrks;
    int header_division;

    MIDIFileReadStream* input_stream;
    MIDIFileEvents* event_handler;
};
}  // namespace jdksmidi

#endif
