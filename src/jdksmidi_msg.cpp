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

#include <stdio.h>
#include <string.h>

#include <cstdint>

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
            static_cast<int>(_byte1));
        strcat(txt, buf);
    }

    else {
        int type = (_status & 0xf0) >> 4;
        //
        // if it is a note on with vel=0, call it a NOTE OFF
        //

        if (type == 9 && _byte2 == 0)
            type = 8;

        if (type != 0xf) {
            snprintf(buf, sizeof(buf), "Ch %2d  ", static_cast<int>(get_channel()) + 1);
            strcat(txt, buf);
        }

        strcat(txt, chan_msg_name[type]);

        if (_status >= 0xf0) {
            strcat(txt, sys_msg_name[_status - 0xf0]);

            if (len > 1) {
                snprintf(buf, sizeof(buf), "%02x", static_cast<int>(_byte1));
                strcat(txt, buf);
            }

            if (len > 2) {
                snprintf(buf, sizeof(buf), ",%02x", static_cast<int>(_byte2));
                strcat(txt, buf);
            }

            if (len > 3) {
                snprintf(buf, sizeof(buf), ",%02x", static_cast<int>(_byte3));
                strcat(txt, buf);
            }
        }

        else {
            char* endtxt = txt + strlen(txt);

            switch (_status & 0xf0) {
                case NOTE_ON:

                    if (_byte2 == 0)
                        snprintf(endtxt, 64, "Note %3d", static_cast<int>(_byte1));

                    else
                        snprintf(
                            endtxt,
                            64,
                            "Note %3d  Vel  %3d  ",
                            static_cast<int>(_byte1),
                            static_cast<int>(_byte2));

                    break;
                case NOTE_OFF:
                    snprintf(
                        endtxt,
                        64,
                        "Note %3d  Vel  %3d  ",
                        static_cast<int>(_byte1),
                        static_cast<int>(_byte2));
                    break;
                case POLY_PRESSURE:
                    snprintf(
                        endtxt,
                        64,
                        "Note %3d  Pres %3d  ",
                        static_cast<int>(_byte1),
                        static_cast<int>(_byte2));
                    break;
                case CONTROL_CHANGE:
                    snprintf(
                        endtxt,
                        64,
                        "Ctrl %3d  Val  %3d  ",
                        static_cast<int>(_byte1),
                        static_cast<int>(_byte2));
                    break;
                case PROGRAM_CHANGE:
                    snprintf(endtxt, 64, "PG   %3d  ", static_cast<int>(_byte1));
                    break;
                case CHANNEL_PRESSURE:
                    snprintf(endtxt, 64, "Pres %3d  ", static_cast<int>(_byte1));
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
    _status = 0;
    _byte1 = 0;
    _byte2 = 0;
    _byte3 = 0;
}

MIDIMessage::MIDIMessage(MIDIMessage const& m)
{
    _status = m._status;
    _byte1 = m._byte1;
    _byte2 = m._byte2;
    _byte3 = m._byte3;
}

void MIDIMessage::clear()
{
    _status = 0;
    _byte1 = 0;
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::copy(MIDIMessage const& m)
{
    _status = m._status;
    _byte1 = m._byte1;
    _byte2 = m._byte2;
    _byte3 = m._byte3;
}

//
// The equal operator
//

MIDIMessage const& MIDIMessage::operator=(MIDIMessage const& m)
{
    _status = m._status;
    _byte1 = m._byte1;
    _byte2 = m._byte2;
    _byte3 = m._byte3;
    return *this;
}

int MIDIMessage::get_length() const
{
    if ((_status & 0xf0) == 0xf0) {
        return get_system_message_length(_status);
    }

    else {
        return get_message_length(_status);
    }
}

std::int16_t MIDIMessage::get_bender_value() const
{
    return (std::int16_t)(((_byte2 << 7) | _byte1) - 8192);
}

std::uint16_t MIDIMessage::get_meta_value() const
{
    return (std::uint16_t)((_byte3 << 8) | _byte2);
}

std::uint8_t MIDIMessage::get_time_sig_numerator() const
{
    return _byte2;
}

std::uint8_t MIDIMessage::get_time_sig_denominator() const
{
    return _byte3;
}

int MIDIMessage::get_key_sig_sharp_flats() const
{
    return (int)(std::int8_t)_byte2;
}

std::uint8_t MIDIMessage::get_key_sig_major_minor() const
{
    return _byte3;
}

bool MIDIMessage::is_channel_msg() const
{
    return (_status >= 0x80) && (_status < 0xf0);
}

bool MIDIMessage::is_note_on() const
{
    return ((_status & 0xf0) == NOTE_ON) && _byte2;
}

bool MIDIMessage::is_note_off() const
{
    return ((_status & 0xf0) == NOTE_OFF) || (((_status & 0xf0) == NOTE_ON) && _byte2 == 0);
}

bool MIDIMessage::is_poly_pressure() const
{
    return ((_status & 0xf0) == POLY_PRESSURE);
}

bool MIDIMessage::is_control_change() const
{
    return ((_status & 0xf0) == CONTROL_CHANGE);
}

bool MIDIMessage::is_program_change() const
{
    return ((_status & 0xf0) == PROGRAM_CHANGE);
}

bool MIDIMessage::is_channel_pressure() const
{
    return ((_status & 0xf0) == CHANNEL_PRESSURE);
}

bool MIDIMessage::is_pitch_bend() const
{
    return ((_status & 0xf0) == PITCH_BEND);
}

bool MIDIMessage::is_system_message() const
{
    return (_status & 0xf0) == 0xf0;
}

bool MIDIMessage::is_sys_ex() const
{
    return (_status == SYSEX_START);
}

std::int16_t MIDIMessage::get_sys_ex_num() const
{
    return (std::int16_t)((_byte3 << 8) | _byte2);
}

bool MIDIMessage::is_mtc() const
{
    return (_status == MTC);
}

bool MIDIMessage::is_song_position() const
{
    return (_status == SONG_POSITION);
}

bool MIDIMessage::is_song_select() const
{
    return (_status == SONG_SELECT);
}

bool MIDIMessage::is_tune_request() const
{
    return (_status == TUNE_REQUEST);
}

bool MIDIMessage::is_meta_event() const
{
    return (_status == META_EVENT);
}

bool MIDIMessage::is_text_event() const
{
    return (_status == META_EVENT) && (_byte1 >= 0x1 && _byte1 <= 0xf);
}

bool MIDIMessage::is_all_notes_off() const
{
    return ((_status & 0xf0) == CONTROL_CHANGE) && (_byte1 >= C_ALL_NOTES_OFF);
}

bool MIDIMessage::is_no_op() const
{
    return (_status == META_EVENT) && (_byte1 == META_NO_OPERATION);
}

bool MIDIMessage::is_tempo() const
{
    return (_status == META_EVENT) && (_byte1 == META_TEMPO);
}

bool MIDIMessage::is_data_end() const
{
    return (_status == META_EVENT) && (_byte1 == META_DATA_END);
}

bool MIDIMessage::is_time_sig() const
{
    return (_status == META_EVENT) && (_byte1 == META_TIMESIG);
}

bool MIDIMessage::is_key_sig() const
{
    return (_status == META_EVENT) && (_byte1 == META_KEYSIG);
}

bool MIDIMessage::is_beat_marker() const
{
    return (_status == META_EVENT) && (_byte1 == META_BEAT_MARKER);
}

std::uint16_t MIDIMessage::get_tempo32() const
{
    return get_meta_value();
}

std::uint16_t MIDIMessage::get_loop_number() const
{
    return get_meta_value();
}

void MIDIMessage::set_bender_value(std::int16_t v)
{
    std::int16_t x = static_cast<std::int16_t>(v + 8192);
    _byte1 = static_cast<std::uint8_t>(x & 0x7f);
    _byte2 = static_cast<std::uint8_t>((x >> 7) & 0x7f);
}

void MIDIMessage::set_meta_type(std::uint8_t t)
{
    _byte1 = t;
}

void MIDIMessage::set_meta_value(std::uint16_t v)
{
    _byte2 = static_cast<std::uint8_t>(v & 0xff);
    _byte3 = static_cast<std::uint8_t>((v >> 8) & 0xff);
}

void MIDIMessage::set_note_on(std::uint8_t chan, std::uint8_t note, std::uint8_t vel)
{
    _status = static_cast<std::uint8_t>(chan | NOTE_ON);
    _byte1 = note;
    _byte2 = vel;
    _byte3 = 0;
}

void MIDIMessage::set_note_off(std::uint8_t chan, std::uint8_t note, std::uint8_t vel)
{
    _status = static_cast<std::uint8_t>(chan | NOTE_OFF);
    _byte1 = note;
    _byte2 = vel;
    _byte3 = 0;
}

void MIDIMessage::set_poly_pressure(std::uint8_t chan, std::uint8_t note, std::uint8_t pres)
{
    _status = static_cast<std::uint8_t>(chan | POLY_PRESSURE);
    _byte1 = note;
    _byte2 = pres;
    _byte3 = 0;
}

void MIDIMessage::set_control_change(std::uint8_t chan, std::uint8_t ctrl, std::uint8_t val)
{
    _status = static_cast<std::uint8_t>(chan | CONTROL_CHANGE);
    _byte1 = ctrl;
    _byte2 = val;
    _byte3 = 0;
}

void MIDIMessage::set_program_change(std::uint8_t chan, std::uint8_t val)
{
    _status = static_cast<std::uint8_t>(chan | PROGRAM_CHANGE);
    _byte1 = val;
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::set_channel_pressure(std::uint8_t chan, std::uint8_t val)
{
    _status = static_cast<std::uint8_t>(chan | CHANNEL_PRESSURE);
    _byte1 = val;
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::set_pitch_bend(std::uint8_t chan, std::int16_t val)
{
    _status = static_cast<std::uint8_t>(chan | PITCH_BEND);
    val += static_cast<std::int16_t>(0x2000);        // center value
    _byte1 = static_cast<std::uint8_t>(val & 0x7f);  // 7 bit bytes
    _byte2 = static_cast<std::uint8_t>((val >> 7) & 0x7f);
    _byte3 = 0;
}

void MIDIMessage::set_pitch_bend(std::uint8_t chan, std::uint8_t low, std::uint8_t high)
{
    _status = static_cast<std::uint8_t>(chan | PITCH_BEND);
    _byte1 = static_cast<std::uint8_t>(low);
    _byte2 = static_cast<std::uint8_t>(high);
    _byte3 = 0;
}

void MIDIMessage::set_sys_ex()
{
    _status = SYSEX_START;
    _byte1 = 0;
    int num = 0;
    _byte2 = static_cast<std::uint8_t>(num & 0xff);
    _byte3 = static_cast<std::uint8_t>((num >> 8) & 0xff);
}

void MIDIMessage::set_mtc(std::uint8_t field, std::uint8_t v)
{
    _status = MTC;
    _byte1 = static_cast<std::uint8_t>((field << 4) | v);
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::set_song_position(std::int16_t pos)
{
    _status = SONG_POSITION;
    _byte1 = static_cast<std::uint8_t>(pos & 0x7f);
    _byte2 = static_cast<std::uint8_t>((pos >> 7) & 0x7f);
    _byte3 = 0;
}

void MIDIMessage::set_song_select(std::uint8_t sng)
{
    _status = SONG_SELECT;
    _byte1 = sng;
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::set_tune_request()
{
    _status = TUNE_REQUEST;
    _byte1 = 0;
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::set_meta_event(std::uint8_t type, std::uint8_t v1, std::uint8_t v2)
{
    _status = META_EVENT;
    _byte1 = type;
    _byte2 = v1;
    _byte3 = v2;
}

void MIDIMessage::set_meta_event(std::uint8_t type, std::uint16_t v)
{
    _status = META_EVENT;
    _byte1 = type;
    _byte2 = static_cast<std::uint8_t>(v & 0xff);
    _byte3 = static_cast<std::uint8_t>((v >> 8) & 0xff);
}

void MIDIMessage::set_all_notes_off(std::uint8_t chan, std::uint8_t type)
{
    _status = static_cast<std::uint8_t>(chan | CONTROL_CHANGE);
    _byte1 = type;
    _byte2 = 0x7f;
    _byte3 = 0;
}

void MIDIMessage::set_local(std::uint8_t chan, std::uint8_t v)
{
    _status = static_cast<std::uint8_t>(chan | CONTROL_CHANGE);
    _byte1 = C_LOCAL;
    _byte2 = v;
    _byte3 = 0;
}

void MIDIMessage::set_no_op()
{
    _status = META_EVENT;
    _byte1 = META_NO_OPERATION;
    _byte2 = 0;
    _byte3 = 0;
}

void MIDIMessage::set_tempo32(std::uint16_t tempo_times_32)
{
    set_meta_event(META_TEMPO, tempo_times_32);
}

void MIDIMessage::set_text(std::uint16_t text_num, std::uint8_t type)
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

void MIDIMessage::set_key_sig(int sharp_flats, std::uint8_t major_minor)
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
    : _time(0)
{}

MIDITimedMessage::MIDITimedMessage(MIDITimedMessage const& m)
    : MIDIMessage(m)
    , _time(m.get_time())
{}

MIDITimedMessage::MIDITimedMessage(MIDIMessage const& m)
    : MIDIMessage(m)
    , _time(0)
{}

void MIDITimedMessage::clear()
{
    _time = 0;
    MIDIMessage::clear();
}

void MIDITimedMessage::copy(MIDITimedMessage const& m)
{
    _time = m.get_time();
    MIDIMessage::copy(m);
}

//
// operator =
//

MIDITimedMessage const& MIDITimedMessage::operator=(MIDITimedMessage const& m)
{
    _time = m.get_time();
    MIDIMessage::operator=(m);
    return *this;
}

MIDITimedMessage const& MIDITimedMessage::operator=(MIDIMessage const& m)
{
    _time = 0;
    MIDIMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDITimedMessage::get_time() const
{
    return _time;
}

//
// 'Set' methods
//

void MIDITimedMessage::set_time(MIDIClockTime t)
{
    _time = t;
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

    // if _times are the same, a note off is always larger

    if (m1._byte1 == m2._byte1 && m1.get_status() == NOTE_ON &&
        ((m2.get_status() == NOTE_ON && m2._byte2 == 0) || (m2.get_status() == NOTE_OFF)))
        return 2;  // m2 is larger

    if (m1._byte1 == m2._byte1 && m2.get_status() == NOTE_ON &&
        ((m1.get_status() == NOTE_ON && m1._byte2 == 0) || (m1.get_status() == NOTE_OFF)))
        return 1;  // m1 is larger

    return 0;  // both are equal.
}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage()
    : _d_time(0)
{}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage(MIDIDeltaTimedMessage const& m)
    : MIDIMessage(m)
    , _d_time(m.get_delta_time())
{}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage(MIDIMessage const& m)
    : MIDIMessage(m)
    , _d_time(0)
{}

void MIDIDeltaTimedMessage::clear()
{
    _d_time = 0;
    MIDIMessage::clear();
}

void MIDIDeltaTimedMessage::copy(MIDIDeltaTimedMessage const& m)
{
    _d_time = m.get_delta_time();
    MIDIMessage::copy(m);
}

//
// operator =
//

MIDIDeltaTimedMessage const& MIDIDeltaTimedMessage::operator=(MIDIDeltaTimedMessage const& m)
{
    _d_time = m.get_delta_time();
    MIDIMessage::operator=(m);
    return *this;
}

MIDIDeltaTimedMessage const& MIDIDeltaTimedMessage::operator=(MIDIMessage const& m)
{
    _d_time = 0;
    MIDIMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDIDeltaTimedMessage::get_delta_time() const
{
    return _d_time;
}

//
// 'Set' methods
//

void MIDIDeltaTimedMessage::set_delta_time(MIDIClockTime t)
{
    _d_time = t;
}

//
// Constructors
//

MIDITimedBigMessage::MIDITimedBigMessage()
    : _time(0)
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDITimedBigMessage const& m)
    : MIDIBigMessage(m)
    , _time(m.get_time())
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDIBigMessage const& m)
    : MIDIBigMessage(m)
    , _time(0)
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDITimedMessage const& m)
    : MIDIBigMessage(m)
    , _time(m.get_time())
{}

MIDITimedBigMessage::MIDITimedBigMessage(MIDIMessage const& m)
    : MIDIBigMessage(m)
    , _time(0)
{}

void MIDITimedBigMessage::clear()
{
    _time = 0;
    MIDIBigMessage::clear();
}

void MIDITimedBigMessage::copy(MIDITimedBigMessage const& m)
{
    _time = m.get_time();
    MIDIBigMessage::copy(m);
}

void MIDITimedBigMessage::copy(MIDITimedMessage const& m)
{
    _time = m.get_time();
    MIDIBigMessage::copy(m);
}

//
// operator =
//

MIDITimedBigMessage const& MIDITimedBigMessage::operator=(MIDITimedBigMessage const& m)
{
    _time = m.get_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDITimedBigMessage const& MIDITimedBigMessage::operator=(MIDITimedMessage const& m)
{
    _time = m.get_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDITimedBigMessage const& MIDITimedBigMessage::operator=(MIDIMessage const& m)
{
    _time = 0;
    MIDIBigMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDITimedBigMessage::get_time() const
{
    return _time;
}

//
// 'Set' methods
//

void MIDITimedBigMessage::set_time(MIDIClockTime t)
{
    _time = t;
}

int MIDITimedBigMessage::compare_events(
    MIDITimedBigMessage const& m1, MIDITimedBigMessage const& m2)
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

    // if _times are the same, a note off is always larger

    if (m1._byte1 == m2._byte1 && m1.get_status() == NOTE_ON &&
        ((m2.get_status() == NOTE_ON && m2._byte2 == 0) || (m2.get_status() == NOTE_OFF)))
        return 2;  // m2 is larger

    if (m1._byte1 == m2._byte1 && m2.get_status() == NOTE_ON &&
        ((m1.get_status() == NOTE_ON && m1._byte2 == 0) || (m1.get_status() == NOTE_OFF)))
        return 1;  // m1 is larger

    return 0;  // both are equal.
}

//
// Constructors
//

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage()
    : _d_time(0)
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIDeltaTimedBigMessage const& m)
    : MIDIBigMessage(m)
    , _d_time(m.get_delta_time())
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIBigMessage const& m)
    : MIDIBigMessage(m)
    , _d_time(0)
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIMessage const& m)
    : MIDIBigMessage(m)
    , _d_time(0)
{}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage(MIDIDeltaTimedMessage const& m)
    : MIDIBigMessage(m)
    , _d_time(m.get_delta_time())
{}

void MIDIDeltaTimedBigMessage::clear()
{
    _d_time = 0;
    MIDIBigMessage::clear();
}

void MIDIDeltaTimedBigMessage::copy(MIDIDeltaTimedBigMessage const& m)
{
    _d_time = m.get_delta_time();
    MIDIBigMessage::copy(m);
}

void MIDIDeltaTimedBigMessage::copy(MIDIDeltaTimedMessage const& m)
{
    _d_time = m.get_delta_time();
    MIDIBigMessage::copy(m);
}

//
// operator =
//

MIDIDeltaTimedBigMessage const& MIDIDeltaTimedBigMessage::operator=(
    MIDIDeltaTimedBigMessage const& m)
{
    _d_time = m.get_delta_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDIDeltaTimedBigMessage const& MIDIDeltaTimedBigMessage::operator=(MIDIDeltaTimedMessage const& m)
{
    _d_time = m.get_delta_time();
    MIDIBigMessage::operator=(m);
    return *this;
}

MIDIDeltaTimedBigMessage const& MIDIDeltaTimedBigMessage::operator=(MIDIMessage const& m)
{
    _d_time = 0;
    MIDIBigMessage::operator=(m);
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDIDeltaTimedBigMessage::get_delta_time() const
{
    return _d_time;
}

//
// 'Set' methods
//

void MIDIDeltaTimedBigMessage::set_delta_time(MIDIClockTime t)
{
    _d_time = t;
}

}  // namespace jdksmidi
