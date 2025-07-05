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

#include "jdksmidi/fileshow.h"

#include <stdio.h>

#include <cstdint>

#ifndef DEBUG_MDFSH
#    define DEBUG_MDFSH 0
#endif

#if DEBUG_MDFSH
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIFileShow::MIDIFileShow(FILE* out_)
    : out(out_)
{}

MIDIFileShow::~MIDIFileShow()
{}

void MIDIFileShow::mf_error(char const* e)
{
    fprintf(out, "\nParse Error: %s\n", e);
    MIDIFileEvents::mf_error(e);
}

void MIDIFileShow::mf_starttrack(int trk)
{
    fprintf(out, "Start Track #%d\n", trk);
}

void MIDIFileShow::mf_endtrack(int trk)
{
    fprintf(out, "End Track   #%d\n", trk);
}

void MIDIFileShow::mf_header(int format, int ntrks, int d)
{
    fprintf(out, "Header: Type=%d Tracks=%d", format, ntrks);
    division = d;

    if (division > 0x8000) {
        std::uint8_t smpte_rate = ((std::uint8_t)((-division) >> 8));
        std::uint8_t smpte_division = (std::uint8_t)(division & 0xff);
        fprintf(out, " SMPTE=%d Division=%d\n", smpte_rate, smpte_division);
    }

    else {
        fprintf(out, " Division=%d\n", division);
    }
}

void MIDIFileShow::show_time(MIDIClockTime time)
{
    if (division > 0) {
        std::uint32_t beat = time / division;
        std::uint32_t clk = time % division;
        fprintf(out, "Time: %6u:%3u    ", beat, clk);
    }

    else {
        fprintf(out, "Time: %9u     ", time);
    }
}

//
// The possible events in a MIDI Files
//

void MIDIFileShow::mf_system_mode(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_note_on(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_note_off(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_poly_after(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_bender(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_program(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_chan_after(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_control(MIDITimedMessage const& msg)
{
    show_time(msg.get_time());
    char buf[64];
    fprintf(out, "%s\n", msg.msg_to_text(buf));
}

void MIDIFileShow::mf_sysex(MIDIClockTime time, MIDISystemExclusive const& ex)
{
    show_time(time);
    fprintf(out, "SysEx     Length=%d\n", ex.get_length());

    for (int i = 0; i < ex.get_length(); ++i) {
        if ((i & 0x1f) == 0)
            fprintf(out, "\n");

        fprintf(out, "%02x ", (int)ex.get_data(i));
    }

    fprintf(out, "\n");
}

void MIDIFileShow::mf_arbitrary(MIDIClockTime time, int len, std::uint8_t* data)
{
    show_time(time);
    fprintf(out, "RAW MIDI DATA    Length=%d\n", len);

    for (int i = 0; i < len; ++i) {
        if ((i & 0x1f) == 0)
            fprintf(out, "\n");

        fprintf(out, "%02x ", (int)data[i]);
    }

    fprintf(out, "\n");
}

void MIDIFileShow::mf_metamisc(MIDIClockTime time, int type, int len, std::uint8_t* data)
{
    show_time(time);
    fprintf(out, "META-EVENT       TYPE=%d Length=%d\n", type, len);

    for (int i = 0; i < len; ++i) {
        if ((i & 0x1f) == 0)
            fprintf(out, "\n");

        fprintf(out, "%02x ", (int)data[i]);
    }

    fprintf(out, "\n");
}

void MIDIFileShow::mf_seqnum(MIDIClockTime time, int num)
{
    show_time(time);
    fprintf(out, "Sequence Number  %d\n", num);
}

void MIDIFileShow::mf_smpte(MIDIClockTime time, int a, int b, int c, int d, int e)
{
    show_time(time);
    fprintf(out, "SMPTE Event      %02x,%02x,%02x,%02x,%02x\n", a, b, c, d, e);
}

void MIDIFileShow::mf_timesig(
    MIDIClockTime time,
    int num,
    int denom_power,
    int midi_clocks_per_metronome,
    int notated_32nds_per_midi_quarter_note)
{
    show_time(time);
    fprintf(
        out,
        "Time Signature   %d/%d  Clks/Metro.=%d 32nd/Quarter=%d\n",
        num,
        denom_power,
        midi_clocks_per_metronome,
        notated_32nds_per_midi_quarter_note);
}

void MIDIFileShow::mf_tempo(MIDIClockTime time, std::uint32_t tempo)
{
    show_time(time);
    fprintf(
        out, "Tempo              %4.2f BPM (%9u usec/beat)\n", (60000000.0 / (double)tempo), tempo);
}

void MIDIFileShow::mf_keysig(MIDIClockTime time, int sf, int mi)
{
    show_time(time);
    fprintf(out, "Key Signature      ");

    if (mi)
        fprintf(out, "MINOR KEY  ");

    else
        fprintf(out, "MAJOR KEY  ");

    if (sf < 0)
        fprintf(out, "%d Flats\n", -sf);

    else
        fprintf(out, "%d Sharps\n", sf);
}

void MIDIFileShow::mf_sqspecific(MIDIClockTime time, int len, std::uint8_t* data)
{
    show_time(time);
    fprintf(out, "Sequencer Specific     Length=%d\n", len);

    for (int i = 0; i < len; ++i) {
        if ((i & 0x1f) == 0)
            fprintf(out, "\n");

        fprintf(out, "%02x ", (int)data[i]);
    }

    fprintf(out, "\n");
}

void MIDIFileShow::mf_text(MIDIClockTime time, int type, int len, std::uint8_t* txt)
{
    static char const* text_event_names[16] = {
        "SEQ. #    ",
        "GENERIC   ",
        "COPYRIGHT ",
        "INST. NAME",
        "TRACK NAME",
        "LYRIC     ",
        "MARKER    ",
        "CUE       ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   ",
        "UNKNOWN   "};

    if (type > 15)
        type = 15;

    show_time(time);
    fprintf(out, "TEXT   %s  '%s'\n", text_event_names[type], (char*)txt);
}

void MIDIFileShow::mf_eot(MIDIClockTime time)
{
    show_time(time);
    fprintf(out, "End Of Track\n");
}

}  // namespace jdksmidi
