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

#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

char const* MIDIMessage::chan_msg_name[16] = {
    "ERROR 00    ",  // 0x00
    "ERROR 10    ",  // 0x10
    "ERROR 20    ",  // 0x20
    "ERROR 30    ",  // 0x30
    "ERROR 40    ",  // 0x40
    "ERROR 50    ",  // 0x50
    "ERROR 60    ",  // 0x60
    "ERROR 70    ",  // 0x70
    "NOTE OFF    ",  // 0x80
    "NOTE ON     ",  // 0x90
    "POLY PRES.  ",  // 0xa0
    "CTRL CHANGE ",  // 0xb0
    "PG CHANGE   ",  // 0xc0
    "CHAN PRES.  ",  // 0xd0
    "BENDER      ",  // 0xe0
    "SYSTEM      "   // 0xf0
};

char const* MIDIMessage::sys_msg_name[16] = {
    "SYSEX       ",  // 0xf0
    "MTC         ",  // 0xf1
    "SONG POS    ",  // 0xf2
    "SONG SELECT ",  // 0xf3
    "ERR - F4    ",  // 0xf4
    "ERR - F5    ",  // 0xf5
    "TUNE REQ.   ",  // 0xf6
    "SYSEX END   ",  // 0xf7
    "CLOCK       ",  // 0xf8
    "MEASURE END ",  // 0xf9
    "START       ",  // 0xfa
    "CONTINUE    ",  // 0xfb
    "STOP        ",  // 0xfc
    "ERR - FD    ",  // 0xfd
    "SENSE       ",  // 0xfe
    "META-EVENT  "   // 0xff
};

char const* MIDIMessage::msg_to_text(char* txt) const
{
    char buf[64];
    int len = get_length();
    *txt = 0;

    if (is_all_notes_off()) {
        snprintf(
            buf,
            sizeof(buf),
            "Ch %2d  All Notes Off  (ctrl=%3d)",
            static_cast<int>(get_channel()) + 1,
            static_cast<int>(byte1));
        strcat(txt, buf);
    }

    else {
        int type = (status & 0xf0) >> 4;
        //
        // if it is a note on with vel=0, call it a NOTE OFF
        //

        if (type == 9 && byte2 == 0)
            type = 8;

        if (type != 0xf) {
            snprintf(buf, sizeof(buf), "Ch %2d  ", static_cast<int>(get_channel()) + 1);
            strcat(txt, buf);
        }

        strcat(txt, chan_msg_name[type]);

        if (status >= 0xf0) {
            strcat(txt, sys_msg_name[status - 0xf0]);

            if (len > 1) {
                snprintf(buf, sizeof(buf), "%02x", static_cast<int>(byte1));
                strcat(txt, buf);
            }

            if (len > 2) {
                snprintf(buf, sizeof(buf), ",%02x", static_cast<int>(byte2));
                strcat(txt, buf);
            }

            if (len > 3) {
                snprintf(buf, sizeof(buf), ",%02x", static_cast<int>(byte3));
                strcat(txt, buf);
            }
        }

        else {
            char* endtxt = txt + strlen(txt);

            switch (status & 0xf0) {
                case NOTE_ON:

                    if (byte2 == 0)
                        snprintf(endtxt, 64, "Note %3d", static_cast<int>(byte1));

                    else
                        snprintf(
                            endtxt,
                            64,
                            "Note %3d  Vel  %3d  ",
                            static_cast<int>(byte1),
                            static_cast<int>(byte2));

                    break;
                case NOTE_OFF:
                    snprintf(
                        endtxt,
                        64,
                        "Note %3d  Vel  %3d  ",
                        static_cast<int>(byte1),
                        static_cast<int>(byte2));
                    break;
                case POLY_PRESSURE:
                    snprintf(
                        endtxt,
                        64,
                        "Note %3d  Pres %3d  ",
                        static_cast<int>(byte1),
                        static_cast<int>(byte2));
                    break;
                case CONTROL_CHANGE:
                    snprintf(
                        endtxt,
                        64,
                        "Ctrl %3d  Val  %3d  ",
                        static_cast<int>(byte1),
                        static_cast<int>(byte2));
                    break;
                case PROGRAM_CHANGE:
                    snprintf(endtxt, 64, "PG   %3d  ", static_cast<int>(byte1));
                    break;
                case CHANNEL_PRESSURE:
                    snprintf(endtxt, 64, "Pres %3d  ", static_cast<int>(byte1));
                    break;
                case PITCH_BEND:
                    snprintf(endtxt, 64, "Val %5d", static_cast<int>(get_bender_value()));
                    break;
            }
        }
    }

    //
    // pad the rest with spaces
    //
    {
        int len = strlen(txt);
        char* p = txt + len;

        while (len < 45) {
            *p++ = ' ';
            ++len;
        }

        *p++ = '\0';
    }
    return txt;
}

MIDIMessage::MIDIMessage()
{
    status = 0;
    byte1 = 0;
    byte2 = 0;
    byte3 = 0;
}

MIDIMessage::MIDIMessage(MIDIMessage const& m)
{
    status = m.status;
    byte1 = m.byte1;
    byte2 = m.byte2;
    byte3 = m.byte3;
}

void MIDIMessage::clear()
{
    status = 0;
    byte1 = 0;
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::copy(MIDIMessage const& m)
{
    status = m.status;
    byte1 = m.byte1;
    byte2 = m.byte2;
    byte3 = m.byte3;
}

//
// The equal operator
//

MIDIMessage const& MIDIMessage::operator=(MIDIMessage const& m)
{
    status = m.status;
    byte1 = m.byte1;
    byte2 = m.byte2;
    byte3 = m.byte3;
    return *this;
}

char MIDIMessage::get_length() const
{
    if ((status & 0xf0) == 0xf0) {
        return get_system_message_length(status);
    }

    else {
        return get_message_length(status);
    }
}

short MIDIMessage::get_bender_value() const
{
    return (short)(((byte2 << 7) | byte1) - 8192);
}

unsigned short MIDIMessage::get_meta_value() const
{
    return (unsigned short)((byte3 << 8) | byte2);
}

std::uint8_t MIDIMessage::get_time_sig_numerator() const
{
    return byte2;
}

std::uint8_t MIDIMessage::get_time_sig_denominator() const
{
    return byte3;
}

signed char MIDIMessage::get_key_sig_sharp_flats() const
{
    return (signed char)byte2;
}

std::uint8_t MIDIMessage::get_key_sig_major_minor() const
{
    return byte3;
}

bool MIDIMessage::is_channel_msg() const
{
    return (status >= 0x80) && (status < 0xf0);
}

bool MIDIMessage::is_note_on() const
{
    return ((status & 0xf0) == NOTE_ON) && byte2;
}

bool MIDIMessage::is_note_off() const
{
    return ((status & 0xf0) == NOTE_OFF) || (((status & 0xf0) == NOTE_ON) && byte2 == 0);
}

bool MIDIMessage::is_poly_pressure() const
{
    return ((status & 0xf0) == POLY_PRESSURE);
}

bool MIDIMessage::is_control_change() const
{
    return ((status & 0xf0) == CONTROL_CHANGE);
}

bool MIDIMessage::is_program_change() const
{
    return ((status & 0xf0) == PROGRAM_CHANGE);
}

bool MIDIMessage::is_channel_pressure() const
{
    return ((status & 0xf0) == CHANNEL_PRESSURE);
}

bool MIDIMessage::is_pitch_bend() const
{
    return ((status & 0xf0) == PITCH_BEND);
}

bool MIDIMessage::is_system_message() const
{
    return (status & 0xf0) == 0xf0;
}

bool MIDIMessage::is_sys_ex() const
{
    return (status == SYSEX_START);
}

short MIDIMessage::get_sys_ex_num() const
{
    return (short)((byte3 << 8) | byte2);
}

bool MIDIMessage::is_mtc() const
{
    return (status == MTC);
}

bool MIDIMessage::is_song_position() const
{
    return (status == SONG_POSITION);
}

bool MIDIMessage::is_song_select() const
{
    return (status == SONG_SELECT);
}

bool MIDIMessage::is_tune_request() const
{
    return (status == TUNE_REQUEST);
}

bool MIDIMessage::is_meta_event() const
{
    return (status == META_EVENT);
}

bool MIDIMessage::is_text_event() const
{
    return (status == META_EVENT) && (byte1 >= 0x1 && byte1 <= 0xf);
}

bool MIDIMessage::is_all_notes_off() const
{
    return ((status & 0xf0) == CONTROL_CHANGE) && (byte1 >= C_ALL_NOTES_OFF);
}

bool MIDIMessage::is_no_op() const
{
    return (status == META_EVENT) && (byte1 == META_NO_OPERATION);
}

bool MIDIMessage::is_tempo() const
{
    return (status == META_EVENT) && (byte1 == META_TEMPO);
}

bool MIDIMessage::is_data_end() const
{
    return (status == META_EVENT) && (byte1 == META_DATA_END);
}

bool MIDIMessage::is_time_sig() const
{
    return (status == META_EVENT) && (byte1 == META_TIMESIG);
}

bool MIDIMessage::is_key_sig() const
{
    return (status == META_EVENT) && (byte1 == META_KEYSIG);
}

bool MIDIMessage::is_beat_marker() const
{
    return (status == META_EVENT) && (byte1 == META_BEAT_MARKER);
}

unsigned short MIDIMessage::get_tempo32() const
{
    return get_meta_value();
}

unsigned short MIDIMessage::get_loop_number() const
{
    return get_meta_value();
}

void MIDIMessage::set_bender_value(short v)
{
    short x = static_cast<short>(v + 8192);
    byte1 = static_cast<std::uint8_t>(x & 0x7f);
    byte2 = static_cast<std::uint8_t>((x >> 7) & 0x7f);
}

void MIDIMessage::set_meta_type(std::uint8_t t)
{
    byte1 = t;
}

void MIDIMessage::set_meta_value(unsigned short v)
{
    byte2 = static_cast<std::uint8_t>(v & 0xff);
    byte3 = static_cast<std::uint8_t>((v >> 8) & 0xff);
}

void MIDIMessage::set_note_on(std::uint8_t chan, std::uint8_t note, std::uint8_t vel)
{
    status = static_cast<std::uint8_t>(chan | NOTE_ON);
    byte1 = note;
    byte2 = vel;
    byte3 = 0;
}

void MIDIMessage::set_note_off(std::uint8_t chan, std::uint8_t note, std::uint8_t vel)
{
    status = static_cast<std::uint8_t>(chan | NOTE_OFF);
    byte1 = note;
    byte2 = vel;
    byte3 = 0;
}

void MIDIMessage::set_poly_pressure(std::uint8_t chan, std::uint8_t note, std::uint8_t pres)
{
    status = static_cast<std::uint8_t>(chan | POLY_PRESSURE);
    byte1 = note;
    byte2 = pres;
    byte3 = 0;
}

void MIDIMessage::set_control_change(std::uint8_t chan, std::uint8_t ctrl, std::uint8_t val)
{
    status = static_cast<std::uint8_t>(chan | CONTROL_CHANGE);
    byte1 = ctrl;
    byte2 = val;
    byte3 = 0;
}

void MIDIMessage::set_program_change(std::uint8_t chan, std::uint8_t val)
{
    status = static_cast<std::uint8_t>(chan | PROGRAM_CHANGE);
    byte1 = val;
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::set_channel_pressure(std::uint8_t chan, std::uint8_t val)
{
    status = static_cast<std::uint8_t>(chan | CHANNEL_PRESSURE);
    byte1 = val;
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::set_pitch_bend(std::uint8_t chan, short val)
{
    status = static_cast<std::uint8_t>(chan | PITCH_BEND);
    val += static_cast<short>(0x2000);              // center value
    byte1 = static_cast<std::uint8_t>(val & 0x7f);  // 7 bit bytes
    byte2 = static_cast<std::uint8_t>((val >> 7) & 0x7f);
    byte3 = 0;
}

void MIDIMessage::set_pitch_bend(std::uint8_t chan, std::uint8_t low, std::uint8_t high)
{
    status = static_cast<std::uint8_t>(chan | PITCH_BEND);
    byte1 = static_cast<std::uint8_t>(low);
    byte2 = static_cast<std::uint8_t>(high);
    byte3 = 0;
}

void MIDIMessage::set_sys_ex()
{
    status = SYSEX_START;
    byte1 = 0;
    int num = 0;
    byte2 = static_cast<std::uint8_t>(num & 0xff);
    byte3 = static_cast<std::uint8_t>((num >> 8) & 0xff);
}

void MIDIMessage::set_mtc(std::uint8_t field, std::uint8_t v)
{
    status = MTC;
    byte1 = static_cast<std::uint8_t>((field << 4) | v);
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::set_song_position(short pos)
{
    status = SONG_POSITION;
    byte1 = static_cast<std::uint8_t>(pos & 0x7f);
    byte2 = static_cast<std::uint8_t>((pos >> 7) & 0x7f);
    byte3 = 0;
}

void MIDIMessage::set_song_select(std::uint8_t sng)
{
    status = SONG_SELECT;
    byte1 = sng;
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::set_tune_request()
{
    status = TUNE_REQUEST;
    byte1 = 0;
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::set_meta_event(std::uint8_t type, std::uint8_t v1, std::uint8_t v2)
{
    status = META_EVENT;
    byte1 = type;
    byte2 = v1;
    byte3 = v2;
}

void MIDIMessage::set_meta_event(std::uint8_t type, unsigned short v)
{
    status = META_EVENT;
    byte1 = type;
    byte2 = static_cast<std::uint8_t>(v & 0xff);
    byte3 = static_cast<std::uint8_t>((v >> 8) & 0xff);
}

void MIDIMessage::set_all_notes_off(std::uint8_t chan, std::uint8_t type)
{
    status = static_cast<std::uint8_t>(chan | CONTROL_CHANGE);
    byte1 = type;
    byte2 = 0x7f;
    byte3 = 0;
}

void MIDIMessage::set_local(std::uint8_t chan, std::uint8_t v)
{
    status = static_cast<std::uint8_t>(chan | CONTROL_CHANGE);
    byte1 = C_LOCAL;
    byte2 = v;
    byte3 = 0;
}

void MIDIMessage::set_no_op()
{
    status = META_EVENT;
    byte1 = META_NO_OPERATION;
    byte2 = 0;
    byte3 = 0;
}

void MIDIMessage::set_tempo32(unsigned short tempo_times_32)
{
    set_meta_event(META_TEMPO, tempo_times_32);
}

void MIDIMessage::set_text(unsigned short text_num, std::uint8_t type)
{
    set_meta_event(type, text_num);
}

void MIDIMessage::set_data_end()
{
    set_meta_event(META_DATA_END, 0);
}

void MIDIMessage::set_time_sig(std::uint8_t num, std::uint8_t den)
{
    set_meta_event(META_TIMESIG, num, den);
}

void MIDIMessage::set_key_sig(signed char sharp_flats, std::uint8_t major_minor)
{
    set_meta_event(META_KEYSIG, sharp_flats, major_minor);
}

void MIDIMessage::set_beat_marker()
{
    set_meta_event(META_BEAT_MARKER, 0, 0);
}

MIDIBigMessage::MIDIBigMessage()
    : sysex(0)
{}

MIDIBigMessage::MIDIBigMessage(MIDIBigMessage const& m)
    : MIDIMessage(m)
    , sysex(0)
{
    if (m.sysex) {
        sysex = new MIDISystemExclusive(*m.sysex);
    }
}

MIDIBigMessage::MIDIBigMessage(MIDIMessage const& m)
    : MIDIMessage(m)
    , sysex(0)
{}

void MIDIBigMessage::clear()
{
    if (sysex) {
        delete sysex;
    }

    sysex = 0;
    MIDIMessage::clear();
}

void MIDIBigMessage::copy(MIDIBigMessage const& m)
{
    delete sysex;

    if (m.sysex) {
        sysex = new MIDISystemExclusive(*m.sysex);
    }

    else {
        sysex = 0;
    }

    MIDIMessage::copy(m);
}

void MIDIBigMessage::copy(MIDIMessage const& m)
{
    delete sysex;
    sysex = 0;
    MIDIMessage::copy(m);
}

//
// destructors
//

MIDIBigMessage::~MIDIBigMessage()
{
    if (sysex) {
        delete sysex;
        sysex = 0;
    }
}

//
// operator =
//

MIDIBigMessage const& MIDIBigMessage::operator=(MIDIBigMessage const& m)
{
    delete sysex;

    if (m.sysex) {
        sysex = new MIDISystemExclusive(*m.sysex);
    }

    else {
        sysex = 0;
    }

    MIDIMessage::operator=(m);
    return *this;
}

MIDIBigMessage const& MIDIBigMessage::operator=(MIDIMessage const& m)
{
    delete sysex;
    sysex = 0;
    MIDIMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDISystemExclusive* MIDIBigMessage::get_sys_ex()
{
    return sysex;
}

MIDISystemExclusive const* MIDIBigMessage::get_sys_ex() const
{
    return sysex;
}

//
// 'Set' methods
//

void MIDIBigMessage::copy_sys_ex(MIDISystemExclusive const* e)
{
    delete sysex;
    sysex = 0;

    if (e) {
        sysex = new MIDISystemExclusive(*e);
    }
}

#if 0
void MIDIBigMessage::set_sys_ex ( MIDISystemExclusive *e )
{
    delete sysex;
    sysex = e;
}
#endif

void MIDIBigMessage::clear_sys_ex()
{
    delete sysex;
    sysex = 0;
}

//
// Constructors
//

MIDITimedMessage::MIDITimedMessage()
    : time(0)
{}

MIDITimedMessage::MIDITimedMessage(MIDITimedMessage const& m)
    : MIDIMessage(m)
    , time(m.get_time())
{}

MIDITimedMessage::MIDITimedMessage(MIDIMessage const& m)
    : MIDIMessage(m)
    , time(0)
{}

void MIDITimedMessage::clear()
{
    time = 0;
    MIDIMessage::clear();
}

void MIDITimedMessage::copy(MIDITimedMessage const& m)
{
    time = m.get_time();
    MIDIMessage::copy(m);
}

//
// operator =
//

MIDITimedMessage const& MIDITimedMessage::operator=(MIDITimedMessage const& m)
{
    time = m.get_time();
    MIDIMessage::operator=(m);
    return *this;
}

MIDITimedMessage const& MIDITimedMessage::operator=(MIDIMessage const& m)
{
    time = 0;
    MIDIMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDITimedMessage::get_time() const
{
    return time;
}

//
// 'Set' methods
//

void MIDITimedMessage::set_time(MIDIClockTime t)
{
    time = t;
}

int MIDITimedMessage::compare_events(MIDITimedMessage const& m1, MIDITimedMessage const& m2)
{
    bool n1 = m1.is_no_op();
    bool n2 = m2.is_no_op();
    // NOP's always are larger.

    if (n1 && n2)
        return 0;  // same, do not care.

    if (n2)
        return 2;  // m2 is larger

    if (n1)
        return 1;  // m1 is larger

    if (m1.get_time() > m2.get_time())
        return 1;  // m1 is larger

    if (m2.get_time() > m1.get_time())
        return 2;  // m2 is larger

    // if times are the same, a note off is always larger

    if (m1.byte1 == m2.byte1 && m1.get_status() == NOTE_ON &&
        ((m2.get_status() == NOTE_ON && m2.byte2 == 0) || (m2.get_status() == NOTE_OFF)))
        return 2;  // m2 is larger

    if (m1.byte1 == m2.byte1 && m2.get_status() == NOTE_ON &&
        ((m1.get_status() == NOTE_ON && m1.byte2 == 0) || (m1.get_status() == NOTE_OFF)))
        return 1;  // m1 is larger

    return 0;  // both are equal.
}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage()
    : dtime(0)
{}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage(MIDIDeltaTimedMessage const& m)
    : MIDIMessage(m)
    , dtime(m.get_delta_time())
{}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage(MIDIMessage const& m)
    : MIDIMessage(m)
    , dtime(0)
{}

void MIDIDeltaTimedMessage::clear()
{
    dtime = 0;
    MIDIMessage::clear();
}

void MIDIDeltaTimedMessage::copy(MIDIDeltaTimedMessage const& m)
{
    dtime = m.get_delta_time();
    MIDIMessage::copy(m);
}

//
// operator =
//

MIDIDeltaTimedMessage const& MIDIDeltaTimedMessage::operator=(MIDIDeltaTimedMessage const& m)
{
    dtime = m.get_delta_time();
    MIDIMessage::operator=(m);
    return *this;
}

MIDIDeltaTimedMessage const& MIDIDeltaTimedMessage::operator=(MIDIMessage const& m)
{
    dtime = 0;
    MIDIMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDIDeltaTimedMessage::get_delta_time() const
{
    return dtime;
}

//
// 'Set' methods
//

void MIDIDeltaTimedMessage::set_delta_time(MIDIClockTime t)
{
    dtime = t;
}

//
// Constructors
//

MIDITimedBigMessage::MIDITimedBigMessage()
    : time(0)
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDITimedBigMessage const& m)
    : MIDIBigMessage(m)
    , time(m.get_time())
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDIBigMessage const& m)
    : MIDIBigMessage(m)
    , time(0)
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDITimedMessage const& m)
    : MIDIBigMessage(m)
    , time(m.get_time())
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDIMessage const& m)
    : MIDIBigMessage(m)
    , time(0)
{}

void MIDITimedBigMessage::clear()
{
    time = 0;
    MIDIBigMessage::clear();
}

void MIDITimedBigMessage::copy(MIDITimedBigMessage const& m)
{
    time = m.get_time();
    MIDIBigMessage::copy(m);
}

void MIDITimedBigMessage::copy(MIDITimedMessage const& m)
{
    time = m.get_time();
    MIDIBigMessage::copy(m);
}

//
// operator =
//

MIDITimedBigMessage const& MIDITimedBigMessage::operator=(MIDITimedBigMessage const& m)
{
    time = m.get_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDITimedBigMessage const& MIDITimedBigMessage::operator=(MIDITimedMessage const& m)
{
    time = m.get_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDITimedBigMessage const& MIDITimedBigMessage::operator=(MIDIMessage const& m)
{
    time = 0;
    MIDIBigMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDITimedBigMessage::get_time() const
{
    return time;
}

//
// 'Set' methods
//

void MIDITimedBigMessage::set_time(MIDIClockTime t)
{
    time = t;
}

int MIDITimedBigMessage::compare_events(MIDITimedBigMessage const& m1, MIDITimedBigMessage const& m2)
{
    bool n1 = m1.is_no_op();
    bool n2 = m2.is_no_op();
    // NOP's always are larger.

    if (n1 && n2)
        return 0;  // same, do not care.

    if (n2)
        return 2;  // m2 is larger

    if (n1)
        return 1;  // m1 is larger

    if (m1.get_time() > m2.get_time())
        return 1;  // m1 is larger

    if (m2.get_time() > m1.get_time())
        return 2;  // m2 is larger

    // if times are the same, a note off is always larger

    if (m1.byte1 == m2.byte1 && m1.get_status() == NOTE_ON &&
        ((m2.get_status() == NOTE_ON && m2.byte2 == 0) || (m2.get_status() == NOTE_OFF)))
        return 2;  // m2 is larger

    if (m1.byte1 == m2.byte1 && m2.get_status() == NOTE_ON &&
        ((m1.get_status() == NOTE_ON && m1.byte2 == 0) || (m1.get_status() == NOTE_OFF)))
        return 1;  // m1 is larger

    return 0;  // both are equal.
}

//
// Constructors
//

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage()
    : dtime(0)
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIDeltaTimedBigMessage const& m)
    : MIDIBigMessage(m)
    , dtime(m.get_delta_time())
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIBigMessage const& m)
    : MIDIBigMessage(m)
    , dtime(0)
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIMessage const& m)
    : MIDIBigMessage(m)
    , dtime(0)
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIDeltaTimedMessage const& m)
    : MIDIBigMessage(m)
    , dtime(m.get_delta_time())
{}

void MIDIDeltaTimedBigMessage::clear()
{
    dtime = 0;
    MIDIBigMessage::clear();
}

void MIDIDeltaTimedBigMessage::copy(MIDIDeltaTimedBigMessage const& m)
{
    dtime = m.get_delta_time();
    MIDIBigMessage::copy(m);
}

void MIDIDeltaTimedBigMessage::copy(MIDIDeltaTimedMessage const& m)
{
    dtime = m.get_delta_time();
    MIDIBigMessage::copy(m);
}

//
// operator =
//

MIDIDeltaTimedBigMessage const& MIDIDeltaTimedBigMessage::operator=(
    MIDIDeltaTimedBigMessage const& m)
{
    dtime = m.get_delta_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDIDeltaTimedBigMessage const& MIDIDeltaTimedBigMessage::operator=(MIDIDeltaTimedMessage const& m)
{
    dtime = m.get_delta_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDIDeltaTimedBigMessage const& MIDIDeltaTimedBigMessage::operator=(MIDIMessage const& m)
{
    dtime = 0;
    MIDIBigMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDIDeltaTimedBigMessage::get_delta_time() const
{
    return dtime;
}

//
// 'Set' methods
//

void MIDIDeltaTimedBigMessage::set_delta_time(MIDIClockTime t)
{
    dtime = t;
}

}  // namespace jdksmidi
