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

#ifndef JDKSMIDI_FILEWRITE_H
#define JDKSMIDI_FILEWRITE_H

#include "jdksmidi/file.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <cstdint>

namespace jdksmidi {

class MIDIFileWriteStream;
class MIDIFileWriteStreamFile;
class MIDIFileWrite;

class MIDIFileWriteStream
{
  public:
    MIDIFileWriteStream();
    virtual ~MIDIFileWriteStream();

    virtual long Seek(long pos, int whence = SEEK_SET) = 0;
    virtual int WriteChar(int c) = 0;
};

class MIDIFileWriteStreamFile : public MIDIFileWriteStream
{
  public:
    MIDIFileWriteStreamFile(FILE* f_);
    virtual ~MIDIFileWriteStreamFile();

    long Seek(long pos, int whence = SEEK_SET);
    int WriteChar(int c);

  protected:
    FILE* f;
};

class MIDIFileWriteStreamFileName : public MIDIFileWriteStreamFile
{
  public:
    MIDIFileWriteStreamFileName(char const* fname)
        : MIDIFileWriteStreamFile(fopen(fname, "wb"))
    {}

    bool IsValid() { return f != 0; }

    virtual ~MIDIFileWriteStreamFileName()
    {
        if (f) {
            fclose(f);
        }
    }
};

class MIDIFileWrite : protected MIDIFile
{
  public:
    MIDIFileWrite(MIDIFileWriteStream* out_stream_);
    virtual ~MIDIFileWrite();

    bool ErrorOccurred() { return error; }
    unsigned long GetFileLength() { return file_length; }
    unsigned long GetTrackLength() { return track_length; }
    void ResetTrackLength() { track_length = 0; }
    void ResetTrackTime() { track_time = 0; }

    void WriteFileHeader(int format, int ntrks, int division);

    void WriteTrackHeader(unsigned long length);

    void WriteEvent(MIDITimedMessage const& m);
    void WriteEvent(unsigned long time, MIDISystemExclusive const* e);
    void WriteEvent(unsigned long time, unsigned short text_type, char const* text);
    void WriteEvent(MIDITimedBigMessage const& m);

    void WriteMetaEvent(
        unsigned long time, std::uint8_t type, std::uint8_t const* data, long length);
    void WriteTempo(unsigned long time, long tempo);
    void WriteKeySignature(unsigned long time, char sharp_flat, char minor);
    void WriteTimeSignature(
        unsigned long time,
        char numerator = 4,
        char denominator_power = 2,
        char midi_clocks_per_metronome = 24,
        char num_32nd_per_midi_quarter_note = 8);

    void WriteEndOfTrack(unsigned long time);

    virtual void RewriteTrackLength();

  protected:
    virtual void Error(char* s);

    void WriteCharacter(std::uint8_t c)
    {
        if (out_stream->WriteChar(c) < 0)
            error = true;
    }

    void Seek(long pos)
    {
        if (out_stream->Seek(pos) < 0)
            error = true;
    }

    void IncrementCounters(int c)
    {
        track_length += c;
        file_length += c;
    }

    void WriteShort(unsigned short c);
    void Write3Char(long c);
    void WriteLong(unsigned long c);

    int WriteVariableNum(unsigned long n);

    void WriteDeltaTime(unsigned long time);

  private:
    bool error;
    bool within_track;
    unsigned long file_length;
    unsigned long track_length;
    unsigned long track_time;
    unsigned long track_position;
    std::uint8_t running_status;

    MIDIFileWriteStream* out_stream;
};
}  // namespace jdksmidi

#endif
