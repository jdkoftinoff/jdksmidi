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

#include "jdksmidi/filewrite.h"
#include "jdksmidi/world.h"

#ifndef DEBUG_MDFWR
#    define DEBUG_MDFWR 0
#endif

#if DEBUG_MDFWR
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIFileWriteStream::MIDIFileWriteStream()
{}

MIDIFileWriteStream::~MIDIFileWriteStream()
{}

MIDIFileWriteStreamFile::MIDIFileWriteStreamFile(FILE* f_)
    : f(f_)
{}

MIDIFileWriteStreamFile::~MIDIFileWriteStreamFile()
{}

long MIDIFileWriteStreamFile::Seek(long pos, int whence)
{
    return fseek(f, pos, whence);
}

int MIDIFileWriteStreamFile::WriteChar(int c)
{
    if (fputc(c, f) == EOF) {
        return -1;
    }

    else {
        return 0;
    }
}

MIDIFileWrite::MIDIFileWrite(MIDIFileWriteStream* out_stream_)
    : out_stream(out_stream_)
{
    ENTER("MIDIFileWrite::MIDIFileWrite()");
    file_length = 0;
    error = 0;
    track_length = 0;
    track_time = 0;
    running_status = 0;
    track_position = 0;
}

MIDIFileWrite::~MIDIFileWrite()
{
    ENTER("MIDIFileWrite::~MIDIFileWrite()");
}

void MIDIFileWrite::Error(char* s)
{
    ENTER("void MIDIFileWrite::Error()");
    // NULL method; can override.
    error = true;
}

void MIDIFileWrite::WriteShort(unsigned short c)
{
    ENTER("void    MIDIFileWrite::WriteShort()");
    WriteCharacter((std::uint8_t)((c >> 8) & 0xff));
    WriteCharacter((std::uint8_t)((c & 0xff)));
}

void MIDIFileWrite::Write3Char(long c)
{
    ENTER("void MIDIFileWrite::Write3Char()");
    WriteCharacter((std::uint8_t)((c >> 16) & 0xff));
    WriteCharacter((std::uint8_t)((c >> 8) & 0xff));
    WriteCharacter((std::uint8_t)((c & 0xff)));
}

void MIDIFileWrite::WriteLong(unsigned long c)
{
    ENTER("void MIDIFileWrite::WriteLong()");
    WriteCharacter((std::uint8_t)((c >> 24) & 0xff));
    WriteCharacter((std::uint8_t)((c >> 16) & 0xff));
    WriteCharacter((std::uint8_t)((c >> 8) & 0xff));
    WriteCharacter((std::uint8_t)((c & 0xff)));
}

void MIDIFileWrite::WriteFileHeader(int format, int ntrks, int division)
{
    ENTER("void MIDIFileWrite::WriteFileHeader()");
    WriteCharacter((std::uint8_t)'M');
    WriteCharacter((std::uint8_t)'T');
    WriteCharacter((std::uint8_t)'h');
    WriteCharacter((std::uint8_t)'d');
    WriteLong(6);
    WriteShort((short)format);
    WriteShort((short)ntrks);
    WriteShort((short)division);
    file_length = 4 + 4 + 6;
}

void MIDIFileWrite::WriteTrackHeader(unsigned long length)
{
    ENTER("void MIDIFileWrite::WriteTrackHeader()");
    track_position = file_length;
    track_length = 0;
    track_time = 0;
    running_status = 0;
    WriteCharacter((std::uint8_t)'M');
    WriteCharacter((std::uint8_t)'T');
    WriteCharacter((std::uint8_t)'r');
    WriteCharacter((std::uint8_t)'k');
    WriteLong(length);
    file_length += 8;
    within_track = true;
}

int MIDIFileWrite::WriteVariableNum(unsigned long n)
{
    ENTER("short MIDIFileWrite::WriteVariableNum()");
    unsigned long buffer;
    short cnt = 0;
    buffer = n & 0x7f;

    while ((n >>= 7) > 0) {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (n & 0x7f);
    }

    while (true) {
        WriteCharacter((std::uint8_t)(buffer & 0xff));
        cnt++;

        if (buffer & 0x80)
            buffer >>= 8;

        else
            break;
    }

    return cnt;
}

void MIDIFileWrite::WriteDeltaTime(unsigned long abs_time)
{
    ENTER("void MIDIFileWrite::WriteDeltaTime()");
    long dtime = abs_time - track_time;

    if (dtime < 0) {
        //  Error( "Events out of order" );
        dtime = 0;
    }

    IncrementCounters(WriteVariableNum(dtime));
    track_time = abs_time;
}

void MIDIFileWrite::WriteEvent(MIDITimedMessage const& m)
{
    ENTER("void    MIDIFileWrite::WriteEvent()");

    if (m.IsNoOp()) {
        return;
    }

    if (m.IsMetaEvent()) {
        // TO DO: add more meta events.
        if (m.IsTempo()) {
            unsigned long tempo = (60000000 / m.GetTempo32()) * 32;
            WriteTempo(m.GetTime(), tempo);
            return;
        }

        if (m.IsDataEnd()) {
            WriteEndOfTrack(m.GetTime());
            return;
        }

        if (m.IsKeySig()) {
            WriteKeySignature(m.GetTime(), m.GetKeySigSharpFlats(), m.GetKeySigMajorMinor());
            return;
        }

        return;  // all other marks are ignored.
    }

    else {
        short len = m.GetLength();
        WriteDeltaTime(m.GetTime());

        if (m.GetStatus() != running_status) {
            running_status = m.GetStatus();
            WriteCharacter((std::uint8_t)running_status);
            IncrementCounters(1);
        }

        if (len > 1) {
            WriteCharacter((std::uint8_t)m.GetByte1());
            IncrementCounters(1);
        }

        if (len > 2) {
            WriteCharacter((std::uint8_t)m.GetByte2());
            IncrementCounters(1);
        }
    }
}

void MIDIFileWrite::WriteEvent(MIDITimedBigMessage const& m)
{
    if (m.IsNoOp()) {
        return;
    }

    if (m.IsMetaEvent()) {
        // if this meta-event has a sysex buffer attached, this
        // buffer contains the raw midi file meta data
        if (m.GetSysEx()) {
            WriteMetaEvent(
                m.GetTime(), m.GetMetaType(), m.GetSysEx()->GetBuf(), m.GetSysEx()->GetLength());
        }

        else {
            // otherwise, it is a type of sysex that doesnt have
            // data...
            if (m.IsTempo()) {
                unsigned long tempo = (60000000 / m.GetTempo32()) * 32;
                WriteTempo(m.GetTime(), tempo);
            }

            else if (m.IsDataEnd()) {
                WriteEndOfTrack(m.GetTime());
            }

            else if (m.IsKeySig()) {
                WriteKeySignature(m.GetTime(), m.GetKeySigSharpFlats(), m.GetKeySigMajorMinor());
            }

            else if (m.IsTimeSig()) {
                WriteTimeSignature(m.GetTime(), m.GetTimeSigNumerator(), m.GetTimeSigDenominator());
            }
        }
    }

    else {
        short len = m.GetLength();

        if (m.IsSysEx() && m.GetSysEx()) {
            WriteEvent(m.GetTime(), m.GetSysEx());
        }

        else if (len > 0) {
            WriteDeltaTime(m.GetTime());

            if (m.GetStatus() != running_status) {
                running_status = m.GetStatus();
                WriteCharacter((std::uint8_t)running_status);
                IncrementCounters(1);
            }

            if (len > 1) {
                WriteCharacter((std::uint8_t)m.GetByte1());
                IncrementCounters(1);
            }

            if (len > 2) {
                WriteCharacter((std::uint8_t)m.GetByte2());
                IncrementCounters(1);
            }
        }
    }
}

void MIDIFileWrite::WriteEvent(unsigned long time, MIDISystemExclusive const* e)
{
    ENTER("void MIDIFileWrite::WriteEvent()");
    int len = e->GetLength();
    WriteDeltaTime(time);
    WriteCharacter((std::uint8_t)SYSEX_START);
    IncrementCounters(WriteVariableNum(len - 1));

    for (int i = 1; i < len; i++)  // skip the initial 0xF0
    {
        WriteCharacter((std::uint8_t)(e->GetData(i)));
    }

    IncrementCounters(len);
    running_status = 0;
}

void MIDIFileWrite::WriteEvent(unsigned long time, unsigned short text_type, char const* text)
{
    ENTER("void MIDIFileWrite::WriteEvent()");
    WriteDeltaTime(time);
    WriteCharacter((std::uint8_t)0xff);       // META-Event
    WriteCharacter((std::uint8_t)text_type);  // Text event type
    IncrementCounters(2);
    long len = strlen(text);
    IncrementCounters(WriteVariableNum(len));

    while (*text) {
        WriteCharacter((std::uint8_t)*text++);
    }

    IncrementCounters(len);
    running_status = 0;
}

void MIDIFileWrite::WriteMetaEvent(
    unsigned long time, std::uint8_t type, std::uint8_t const* data, long length)
{
    ENTER("void MIDIFileWrite::WriteMetaEvent()");
    WriteDeltaTime(time);
    WriteCharacter((std::uint8_t)0xff);  // META-Event
    WriteCharacter((std::uint8_t)type);  // Meta-event type
    IncrementCounters(2);
    IncrementCounters(WriteVariableNum(length));

    for (int i = 0; i < length; i++) {
        WriteCharacter((std::uint8_t)data[i]);
    }

    IncrementCounters(length);
    running_status = 0;
}

void MIDIFileWrite::WriteTempo(unsigned long time, long tempo)
{
    ENTER("void MIDIFileWrite::WriteTempo()");
    WriteDeltaTime(time);
    WriteCharacter((std::uint8_t)0xff);  // Meta-Event
    WriteCharacter((std::uint8_t)0x51);  // Tempo event
    WriteCharacter((std::uint8_t)0x03);  // length of event
    Write3Char(tempo);
    IncrementCounters(6);
    running_status = 0;
}

void MIDIFileWrite::WriteKeySignature(unsigned long time, char sharp_flat, char minor)
{
    ENTER("void MIDIFileWrite::WriteKeySignature()");
    WriteDeltaTime(time);
    WriteCharacter((std::uint8_t)0xff);        // Meta-Event
    WriteCharacter((std::uint8_t)0x59);        // Key Sig
    WriteCharacter((std::uint8_t)0x02);        // length of event
    WriteCharacter((std::uint8_t)sharp_flat);  // - for flats, + for sharps
    WriteCharacter((std::uint8_t)minor);       // 1 if minor key
    IncrementCounters(5);
    running_status = 0;
}

void MIDIFileWrite::WriteTimeSignature(
    unsigned long time,
    char numerator,
    char denominator_power,
    char midi_clocks_per_metronome,
    char num_32nd_per_midi_quarter_note)
{
    ENTER("void MIDIFileWrite::WriteTimeSignature()");
    WriteDeltaTime(time);
    WriteCharacter((std::uint8_t)0xff);  // Meta-Event
    WriteCharacter((std::uint8_t)0x58);  // time signature
    WriteCharacter((std::uint8_t)0x04);  // length of event
    WriteCharacter((std::uint8_t)numerator);
    WriteCharacter((std::uint8_t)denominator_power);
    WriteCharacter((std::uint8_t)midi_clocks_per_metronome);
    WriteCharacter((std::uint8_t)num_32nd_per_midi_quarter_note);
    IncrementCounters(7);
    running_status = 0;
}

void MIDIFileWrite::WriteEndOfTrack(unsigned long time)
{
    ENTER("void MIDIFileWrite::WriteEndOfTrack()");

    if (within_track == true) {
        if (time == 0)
            time = track_time;

        WriteDeltaTime(time);
        WriteCharacter((std::uint8_t)0xff);  // Meta-Event
        WriteCharacter((std::uint8_t)0x2f);  // End of track
        WriteCharacter((std::uint8_t)0x00);  // length of event
        IncrementCounters(3);
        within_track = false;
        running_status = 0;
    }
}

void MIDIFileWrite::RewriteTrackLength()
{
    ENTER("void MIDIFileWrite::RewriteTrackLength()");
    // go back and patch in the tracks length into the track chunk
    // header, now that we know the proper value.
    // then make sure we go back to the end of the file
    Seek(track_position + 4);
    WriteLong(track_length);
    Seek(track_position + 8 + track_length);
}

}  // namespace jdksmidi
