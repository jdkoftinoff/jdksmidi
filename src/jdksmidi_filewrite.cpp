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

long MIDIFileWriteStreamFile::seek(long pos, int whence)
{
    return fseek(f, pos, whence);
}

int MIDIFileWriteStreamFile::write_char(int c)
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

void MIDIFileWrite::error_handler(char* s)
{
    ENTER("void MIDIFileWrite::error_handler()");
    // NULL method; can override.
    error = true;
}

void MIDIFileWrite::write_short(unsigned short c)
{
    ENTER("void    MIDIFileWrite::write_short()");
    write_character((std::uint8_t)((c >> 8) & 0xff));
    write_character((std::uint8_t)((c & 0xff)));
}

void MIDIFileWrite::write_3_char(long c)
{
    ENTER("void MIDIFileWrite::write_3_char()");
    write_character((std::uint8_t)((c >> 16) & 0xff));
    write_character((std::uint8_t)((c >> 8) & 0xff));
    write_character((std::uint8_t)((c & 0xff)));
}

void MIDIFileWrite::write_long(unsigned long c)
{
    ENTER("void MIDIFileWrite::write_long()");
    write_character((std::uint8_t)((c >> 24) & 0xff));
    write_character((std::uint8_t)((c >> 16) & 0xff));
    write_character((std::uint8_t)((c >> 8) & 0xff));
    write_character((std::uint8_t)((c & 0xff)));
}

void MIDIFileWrite::write_file_header(int format, int ntrks, int division)
{
    ENTER("void MIDIFileWrite::write_file_header()");
    write_character((std::uint8_t)'M');
    write_character((std::uint8_t)'T');
    write_character((std::uint8_t)'h');
    write_character((std::uint8_t)'d');
    write_long(6);
    write_short((short)format);
    write_short((short)ntrks);
    write_short((short)division);
    file_length = 4 + 4 + 6;
}

void MIDIFileWrite::write_track_header(unsigned long length)
{
    ENTER("void MIDIFileWrite::write_track_header()");
    track_position = file_length;
    track_length = 0;
    track_time = 0;
    running_status = 0;
    write_character((std::uint8_t)'M');
    write_character((std::uint8_t)'T');
    write_character((std::uint8_t)'r');
    write_character((std::uint8_t)'k');
    write_long(length);
    file_length += 8;
    within_track = true;
}

int MIDIFileWrite::write_variable_num(unsigned long n)
{
    ENTER("short MIDIFileWrite::write_variable_num()");
    unsigned long buffer;
    short cnt = 0;
    buffer = n & 0x7f;

    while ((n >>= 7) > 0) {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (n & 0x7f);
    }

    while (true) {
        write_character((std::uint8_t)(buffer & 0xff));
        cnt++;

        if (buffer & 0x80)
            buffer >>= 8;

        else
            break;
    }

    return cnt;
}

void MIDIFileWrite::write_delta_time(unsigned long abs_time)
{
    ENTER("void MIDIFileWrite::write_delta_time()");
    long dtime = abs_time - track_time;

    if (dtime < 0) {
        //  error( "Events out of order" );
        dtime = 0;
    }

    increment_counters(write_variable_num(dtime));
    track_time = abs_time;
}

void MIDIFileWrite::write_event(MIDITimedMessage const& m)
{
    ENTER("void    MIDIFileWrite::write_event()");

    if (m.is_no_op()) {
        return;
    }

    if (m.is_meta_event()) {
        // TO DO: add more meta events.
        if (m.is_tempo()) {
            unsigned long tempo = (60000000 / m.get_tempo32()) * 32;
            write_tempo(m.get_time(), tempo);
            return;
        }

        if (m.is_data_end()) {
            write_end_of_track(m.get_time());
            return;
        }

        if (m.is_key_sig()) {
            write_key_signature(m.get_time(), m.get_key_sig_sharp_flats(), m.get_key_sig_major_minor());
            return;
        }

        return;  // all other marks are ignored.
    }

    else {
        short len = m.get_length();
        write_delta_time(m.get_time());

        if (m.get_status() != running_status) {
            running_status = m.get_status();
            write_character((std::uint8_t)running_status);
            increment_counters(1);
        }

        if (len > 1) {
            write_character((std::uint8_t)m.get_byte1());
            increment_counters(1);
        }

        if (len > 2) {
            write_character((std::uint8_t)m.get_byte2());
            increment_counters(1);
        }
    }
}

void MIDIFileWrite::write_event(MIDITimedBigMessage const& m)
{
    if (m.is_no_op()) {
        return;
    }

    if (m.is_meta_event()) {
        // if this meta-event has a sysex buffer attached, this
        // buffer contains the raw midi file meta data
        if (m.get_sys_ex()) {
            write_meta_event(
                m.get_time(), m.get_meta_type(), m.get_sys_ex()->get_buf(), m.get_sys_ex()->get_length());
        }

        else {
            // otherwise, it is a type of sysex that doesnt have
            // data...
            if (m.is_tempo()) {
                unsigned long tempo = (60000000 / m.get_tempo32()) * 32;
                write_tempo(m.get_time(), tempo);
            }

            else if (m.is_data_end()) {
                write_end_of_track(m.get_time());
            }

            else if (m.is_key_sig()) {
                write_key_signature(m.get_time(), m.get_key_sig_sharp_flats(), m.get_key_sig_major_minor());
            }

            else if (m.is_time_sig()) {
                write_time_signature(m.get_time(), m.get_time_sig_numerator(), m.get_time_sig_denominator());
            }
        }
    }

    else {
        short len = m.get_length();

        if (m.is_sys_ex() && m.get_sys_ex()) {
            write_event(m.get_time(), m.get_sys_ex());
        }

        else if (len > 0) {
            write_delta_time(m.get_time());

            if (m.get_status() != running_status) {
                running_status = m.get_status();
                write_character((std::uint8_t)running_status);
                increment_counters(1);
            }

            if (len > 1) {
                write_character((std::uint8_t)m.get_byte1());
                increment_counters(1);
            }

            if (len > 2) {
                write_character((std::uint8_t)m.get_byte2());
                increment_counters(1);
            }
        }
    }
}

void MIDIFileWrite::write_event(unsigned long time, MIDISystemExclusive const* e)
{
    ENTER("void MIDIFileWrite::write_event()");
    int len = e->get_length();
    write_delta_time(time);
    write_character((std::uint8_t)SYSEX_START);
    increment_counters(write_variable_num(len - 1));

    for (int i = 1; i < len; i++)  // skip the initial 0xF0
    {
        write_character((std::uint8_t)(e->get_data(i)));
    }

    increment_counters(len);
    running_status = 0;
}

void MIDIFileWrite::write_event(unsigned long time, unsigned short text_type, char const* text)
{
    ENTER("void MIDIFileWrite::write_event()");
    write_delta_time(time);
    write_character((std::uint8_t)0xff);       // META-Event
    write_character((std::uint8_t)text_type);  // Text event type
    increment_counters(2);
    long len = strlen(text);
    increment_counters(write_variable_num(len));

    while (*text) {
        write_character((std::uint8_t)*text++);
    }

    increment_counters(len);
    running_status = 0;
}

void MIDIFileWrite::write_meta_event(
    unsigned long time, std::uint8_t type, std::uint8_t const* data, long length)
{
    ENTER("void MIDIFileWrite::write_meta_event()");
    write_delta_time(time);
    write_character((std::uint8_t)0xff);  // META-Event
    write_character((std::uint8_t)type);  // Meta-event type
    increment_counters(2);
    increment_counters(write_variable_num(length));

    for (int i = 0; i < length; i++) {
        write_character((std::uint8_t)data[i]);
    }

    increment_counters(length);
    running_status = 0;
}

void MIDIFileWrite::write_tempo(unsigned long time, long tempo)
{
    ENTER("void MIDIFileWrite::write_tempo()");
    write_delta_time(time);
    write_character((std::uint8_t)0xff);  // Meta-Event
    write_character((std::uint8_t)0x51);  // Tempo event
    write_character((std::uint8_t)0x03);  // length of event
    write_3_char(tempo);
    increment_counters(6);
    running_status = 0;
}

void MIDIFileWrite::write_key_signature(unsigned long time, char sharp_flat, char minor)
{
    ENTER("void MIDIFileWrite::write_key_signature()");
    write_delta_time(time);
    write_character((std::uint8_t)0xff);        // Meta-Event
    write_character((std::uint8_t)0x59);        // Key Sig
    write_character((std::uint8_t)0x02);        // length of event
    write_character((std::uint8_t)sharp_flat);  // - for flats, + for sharps
    write_character((std::uint8_t)minor);       // 1 if minor key
    increment_counters(5);
    running_status = 0;
}

void MIDIFileWrite::write_time_signature(
    unsigned long time,
    char numerator,
    char denominator_power,
    char midi_clocks_per_metronome,
    char num_32nd_per_midi_quarter_note)
{
    ENTER("void MIDIFileWrite::write_time_signature()");
    write_delta_time(time);
    write_character((std::uint8_t)0xff);  // Meta-Event
    write_character((std::uint8_t)0x58);  // time signature
    write_character((std::uint8_t)0x04);  // length of event
    write_character((std::uint8_t)numerator);
    write_character((std::uint8_t)denominator_power);
    write_character((std::uint8_t)midi_clocks_per_metronome);
    write_character((std::uint8_t)num_32nd_per_midi_quarter_note);
    increment_counters(7);
    running_status = 0;
}

void MIDIFileWrite::write_end_of_track(unsigned long time)
{
    ENTER("void MIDIFileWrite::write_end_of_track()");

    if (within_track == true) {
        if (time == 0)
            time = track_time;

        write_delta_time(time);
        write_character((std::uint8_t)0xff);  // Meta-Event
        write_character((std::uint8_t)0x2f);  // End of track
        write_character((std::uint8_t)0x00);  // length of event
        increment_counters(3);
        within_track = false;
        running_status = 0;
    }
}

void MIDIFileWrite::rewrite_track_length()
{
    ENTER("void MIDIFileWrite::rewrite_track_length()");
    // go back and patch in the tracks length into the track chunk
    // header, now that we know the proper value.
    // then make sure we go back to the end of the file
    seek(track_position + 4);
    write_long(track_length);
    seek(track_position + 8 + track_length);
}

}  // namespace jdksmidi
