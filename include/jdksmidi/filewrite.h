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

#ifndef JDKSMIDI_FILEWRITE_H
#define JDKSMIDI_FILEWRITE_H

#include "jdksmidi/file.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <cstdint>
#include <cstdio>

namespace jdksmidi {

class MIDIFileWriteStream;
class MIDIFileWriteStreamFile;
class MIDIFileWrite;

class MIDIFileWriteStream
{
  public:
    MIDIFileWriteStream();
    virtual ~MIDIFileWriteStream();

    virtual std::int32_t seek(std::int32_t pos, int whence = SEEK_SET) = 0;
    virtual int write_char(int c) = 0;
};

class MIDIFileWriteStreamFile : public MIDIFileWriteStream
{
  public:
    MIDIFileWriteStreamFile(FILE* f_);
    virtual ~MIDIFileWriteStreamFile();

    std::int32_t seek(std::int32_t pos, int whence = SEEK_SET);
    int write_char(int c);

  protected:
    FILE* f;
};

class MIDIFileWriteStreamFileName : public MIDIFileWriteStreamFile
{
  public:
    MIDIFileWriteStreamFileName(char const* fname)
        : MIDIFileWriteStreamFile(fopen(fname, "wb"))
    {}

    bool is_valid() { return f != 0; }

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

    bool error_occurred() { return error; }
    std::uint32_t get_file_length() { return file_length; }
    std::uint32_t get_track_length() { return track_length; }
    void reset_track_length() { track_length = 0; }
    void reset_track_time() { track_time = 0; }

    void write_file_header(int format, int ntrks, int division);

    void write_track_header(std::uint32_t length);

    void write_event(MIDITimedMessage const& m);
    void write_event(std::uint32_t time, MIDISystemExclusive const* e);
    void write_event(std::uint32_t time, std::uint16_t text_type, char const* text);
    void write_event(MIDITimedBigMessage const& m);

    void write_meta_event(
        std::uint32_t time, std::uint8_t type, std::uint8_t const* data, std::int32_t length);
    void write_tempo(std::uint32_t time, std::int32_t tempo);
    void write_key_signature(std::uint32_t time, char sharp_flat, char minor);
    void write_time_signature(
        std::uint32_t time,
        char numerator = 4,
        char denominator_power = 2,
        char midi_clocks_per_metronome = 24,
        char num_32nd_per_midi_quarter_note = 8);

    void write_end_of_track(std::uint32_t time);

    virtual void rewrite_track_length();

  protected:
    virtual void error_handler(char* s);

    void write_character(std::uint8_t c)
    {
        if (out_stream->write_char(c) < 0)
            error = true;
    }

    void seek(std::int32_t pos)
    {
        if (out_stream->seek(pos) < 0)
            error = true;
    }

    void increment_counters(int c)
    {
        track_length += c;
        file_length += c;
    }

    void write_short(std::uint16_t c);
    void write_3_char(std::int32_t c);
    void write_long(std::uint32_t c);

    int write_variable_num(std::uint32_t n);

    void write_delta_time(std::uint32_t time);

  private:
    bool error;
    bool within_track;
    std::uint32_t file_length;
    std::uint32_t track_length;
    std::uint32_t track_time;
    std::uint32_t track_position;
    std::uint8_t running_status;

    MIDIFileWriteStream* out_stream;
};
}  // namespace jdksmidi

#endif
