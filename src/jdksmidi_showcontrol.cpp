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

#include "jdksmidi/showcontrol.h"
#include "jdksmidi/sysex.h"

#include <cinttypes>
#include <cstdint>
#include <cstdio>

namespace jdksmidi {

MIDIShowControlPacket::MIDIShowControlPacket()
    : q_number(0)
    , q_list(0)
    , q_path(0)
{
    device_id = 0;
    command_fmt = 0;
    command = MIDI_SC_GO;
    has_time = false;
    has_q_number = false;
    has_q_list = false;
    has_q_path = false;
    hours = 0;
    minutes = 0;
    seconds = 0;
    frames = 0;
    fract_frames = 0;
    val1 = 0;
    val2 = 0;
}

void MIDIShowControlPacket::clear_variable_stuff()
{
    q_number.clear();
    q_list.clear();
    q_path.clear();
    command = MIDI_SC_GO;
    has_time = false;
    has_q_number = false;
    has_q_list = false;
    has_q_path = false;
    hours = 0;
    minutes = 0;
    seconds = 0;
    frames = 0;
    fract_frames = 0;
    val1 = 0;
    val2 = 0;
}

bool MIDIShowControlPacket::parse_entire_sys_ex(MIDISystemExclusive const* e)
{
    bool f = true;
    int pos = 0;

    if (e->get_data(pos++) != 0xf0)
        return false;

    if (e->get_data(pos++) != 0x7f)
        return false;

    clear_variable_stuff();
    device_id = e->get_data(pos++);

    if (e->get_data(pos++) != 0x02)
        return false;

    command_fmt = e->get_data(pos++);
    command = (MIDIShowCommand)e->get_data(pos++);

    switch (command) {
        case MIDI_SC_GO:
        case MIDI_SC_STOP:
        case MIDI_SC_RESUME:
        case MIDI_SC_LOAD:
        case MIDI_SC_GO_OFF:
        case MIDI_SC_GO_JAM: {
            f &= parse_3_param(e, &pos);
        } break;
        case MIDI_SC_TIMED_GO: {
            f &= parse_time(e, &pos);
            f &= parse_3_param(e, &pos);
        } break;
        case MIDI_SC_SET: {
            f &= parse_set(e, &pos);
        } break;
        case MIDI_SC_FIRE: {
            f &= parse_fire(e, &pos);
        } break;
        case MIDI_SC_ALL_OFF:
        case MIDI_SC_RESTORE:
        case MIDI_SC_RESET:
            // no additional params.
            break;
        case MIDI_SC_STANDBY_PLUS:
        case MIDI_SC_STANDBY_MINUS:
        case MIDI_SC_SEQUENCE_PLUS:
        case MIDI_SC_SEQUENCE_MINUS:
        case MIDI_SC_START_CLOCK:
        case MIDI_SC_STOP_CLOCK:
        case MIDI_SC_ZERO_CLOCK:
        case MIDI_SC_MTC_CHASE_ON:
        case MIDI_SC_MTC_CHASE_OFF: {
            if (e->get_data(pos) != 0xf7 && e->get_data(pos) != 0)
                f &= parse_q_list(e, &pos);
        } break;
        case MIDI_SC_SET_CLOCK: {
            f &= parse_time(e, &pos);

            if (e->get_data(pos) != 0xf7 && e->get_data(pos) != 0)
                f &= parse_q_list(e, &pos);
        } break;
        case MIDI_SC_OPEN_Q_LIST:
        case MIDI_SC_CLOSE_Q_LIST: {
            f &= parse_q_list(e, &pos);
        } break;
        case MIDI_SC_OPEN_Q_PATH:
        case MIDI_SC_CLOSE_Q_PATH: {
            f &= parse_q_path(e, &pos);
        } break;
        default:
            f = false;  // unrecognized command
            break;
    }

    if (e->get_length() < pos) {
        return false;
    }

    return f;
}

bool MIDIShowControlPacket::store_to_sys_ex(MIDISystemExclusive* e) const
{
    bool f = true;
    e->clear();
    e->put_exc();
    e->put_byte(0x7f);
    e->put_byte(device_id);
    e->put_byte(0x02);
    e->put_byte(command_fmt);
    e->put_byte(command);

    switch (command) {
        case MIDI_SC_GO:
        case MIDI_SC_STOP:
        case MIDI_SC_RESUME:
        case MIDI_SC_LOAD:
        case MIDI_SC_GO_OFF:
        case MIDI_SC_GO_JAM: {
            f &= store_3_param(e);
        } break;
        case MIDI_SC_TIMED_GO: {
            f &= store_time(e);
            f &= store_3_param(e);
        } break;
        case MIDI_SC_SET: {
            f &= store_set(e);
        } break;
        case MIDI_SC_FIRE: {
            f &= store_fire(e);
        } break;
        case MIDI_SC_ALL_OFF:
        case MIDI_SC_RESTORE:
        case MIDI_SC_RESET:
            // no additional params.
            break;
        case MIDI_SC_STANDBY_PLUS:
        case MIDI_SC_STANDBY_MINUS:
        case MIDI_SC_SEQUENCE_PLUS:
        case MIDI_SC_SEQUENCE_MINUS:
        case MIDI_SC_START_CLOCK:
        case MIDI_SC_STOP_CLOCK:
        case MIDI_SC_ZERO_CLOCK:
        case MIDI_SC_MTC_CHASE_ON:
        case MIDI_SC_MTC_CHASE_OFF: {
            if (has_q_list)
                f &= store_q_list(e);
        } break;
        case MIDI_SC_SET_CLOCK: {
            f &= store_time(e);

            if (has_q_list)
                f &= store_q_list(e);
        } break;
        case MIDI_SC_OPEN_Q_LIST:
        case MIDI_SC_CLOSE_Q_LIST: {
            f &= store_q_list(e);
        } break;
        case MIDI_SC_OPEN_Q_PATH:
        case MIDI_SC_CLOSE_Q_PATH: {
            f &= store_q_path(e);
        } break;
        default:
            f = false;  // unrecognized command
            break;
    }

    e->put_eox();

    if (e->is_full())
        return false;

    return f;
}

bool MIDIShowControlPacket::store_time(MIDISystemExclusive* e) const
{
    if (has_time) {
        e->put_byte(hours);
        e->put_byte(minutes);
        e->put_byte(seconds);
        e->put_byte(frames);
        e->put_byte(fract_frames);
        return true;
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::parse_time(MIDISystemExclusive const* e, int* pos)
{
    hours = e->get_data((*pos)++);
    minutes = e->get_data((*pos)++);
    seconds = e->get_data((*pos)++);
    frames = e->get_data((*pos)++);
    fract_frames = e->get_data((*pos)++);
    return true;
}

bool MIDIShowControlPacket::store_3_param(MIDISystemExclusive* e) const
{
    bool f = true;

    if (has_q_number) {
        f &= store_ascii_num(e, get_q_number());

        if (has_q_list) {
            e->put_byte(0);
            f &= store_ascii_num(e, get_q_list());

            if (has_q_path) {
                e->put_byte(0);
                f &= store_ascii_num(e, get_q_path());
            }
        }
    }

    return f;
}

bool MIDIShowControlPacket::parse_3_param(MIDISystemExclusive const* e, int* pos)
{
    bool f = true;
    MIDICue v;

    if (e->get_data(*pos) != 0xf7) {
        //
        // Read the Q number
        //
        f = parse_ascii_num(e, pos, &v);

        if (f) {
            has_q_number = true;
            set_q_number(v);

            if (*pos < e->get_length()) {
                //
                // read the q list
                //
                f = parse_ascii_num(e, pos, &v);

                if (f) {
                    has_q_list = true;
                    set_q_list(v);

                    if (*pos < e->get_length()) {
                        //
                        // read the q path
                        //
                        f = parse_ascii_num(e, pos, &v);

                        if (f) {
                            has_q_path = true;
                            set_q_path(v);
                        }
                    }
                }
            }
        }
    }

    return f;
}

bool MIDIShowControlPacket::store_set(MIDISystemExclusive* e) const
{
    bool f = true;
    e->put_byte(static_cast<std::uint8_t>(get_control_num() & 0x7f));
    e->put_byte(static_cast<std::uint8_t>((get_control_num() >> 7) & 0x7f));
    e->put_byte(static_cast<std::uint8_t>(get_control_val() & 0x7f));
    e->put_byte(static_cast<std::uint8_t>((get_control_val() >> 7) & 0x7f));

    if (has_time) {
        f = store_time(e);
    }

    return f;
}

bool MIDIShowControlPacket::parse_set(MIDISystemExclusive const* e, int* pos)
{
    std::uint32_t v;
    v = e->get_data((*pos)++);
    v += (e->get_data((*pos)++) << 7);
    set_control_num(v);
    v = e->get_data((*pos)++);
    v += (e->get_data((*pos)++) << 7);
    set_control_val(v);

    if (e->get_data(*pos) != 0xf7) {
        return parse_time(e, pos);
    }

    else {
        return true;
    }
}

bool MIDIShowControlPacket::store_fire(MIDISystemExclusive* e) const
{
    e->put_byte(static_cast<std::uint8_t>(get_macro_num()));
    return true;
}

bool MIDIShowControlPacket::parse_fire(MIDISystemExclusive const* e, int* pos)
{
    int v;
    v = e->get_data((*pos)++);
    set_macro_num(v);
    return true;
}

bool MIDIShowControlPacket::store_q_path(MIDISystemExclusive* e) const
{
    if (has_q_path) {
        return store_ascii_num(e, get_q_path());
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::parse_q_path(MIDISystemExclusive const* e, int* pos)
{
    bool f = true;
    MIDICue v;

    if (e->get_data(*pos) != 0xf7) {
        //
        // Read the Q Path
        //
        f = parse_ascii_num(e, pos, &v);
        set_q_path(v);
        has_q_path = true;
        return f;
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::store_q_list(MIDISystemExclusive* e) const
{
    if (has_q_list) {
        return store_ascii_num(e, get_q_list());
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::parse_q_list(MIDISystemExclusive const* e, int* pos)
{
    bool f = true;
    MIDICue v;

    if (e->get_data(*pos) != 0xf7) {
        //
        // Read the Q list
        //
        f = parse_ascii_num(e, pos, &v);
        set_q_list(v);
        has_q_list = true;
        return f;
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::store_ascii(MIDISystemExclusive* e, char const* str) const
{
    while (*str) {
        e->put_byte(*str++);
    }

    return true;
}

bool MIDIShowControlPacket::store_ascii_num(MIDISystemExclusive* e, MIDICue const& num) const
{
    char buf[32];
    bool f = false;
    *buf = '\0';

    switch (num.get_num_values()) {
        default:
        case 0:
            break;
        case 1:
            snprintf(buf, sizeof(buf), "%" PRIu32, num.get_v1());
            break;
        case 2:
            snprintf(buf, sizeof(buf), "%" PRIu32 ".%" PRIu32, num.get_v1(), num.get_v2());
            break;
        case 3:
            snprintf(
                buf,
                sizeof(buf),
                "%" PRIu32 ".%" PRIu32 ".%" PRIu32,
                num.get_v1(),
                num.get_v2(),
                num.get_v3());
            break;
    }

    f = store_ascii(e, buf);
    return f;
}

bool MIDIShowControlPacket::parse_ascii_num(MIDISystemExclusive const* e, int* pos, MIDICue* num)
{
    std::uint32_t v;
    std::uint8_t c;
    bool f;
    // check if there is a field
    c = e->get_data(*pos);

    if (c == 0xf7) {
        // no field
        return false;
    }

    f = parse_ascii_num(e, pos, &v);

    if (f) {
        num->set_num_values(1);
        num->set_v1(v);
    }

    //
    // Is there another field?
    //
    c = e->get_data(*pos);

    if (c == '.') {
        //
        // yes read it
        //
        (*pos)++;
        f = parse_ascii_num(e, pos, &v);

        if (f) {
            num->set_num_values(2);
            num->set_v2(v);
        }

        //
        // Is there another field?
        //
        c = e->get_data(*pos);

        if (c == '.') {
            //
            // yes read it
            //
            (*pos)++;
            f = parse_ascii_num(e, pos, &v);

            if (f) {
                num->set_num_values(2);
                num->set_v3(v);
            }

            //
            // Is there more fields?
            //
            c = e->get_data(*pos);

            if (c == '.') {
                //
                // Yes, skip them
                //
                while (*pos < e->get_length()) {
                    c = e->get_data((*pos));

                    if (c == 0xf7 || c == 0x00) {
                        break;
                    }

                    (*pos)++;
                }
            }
        }
    }

    while (++(*pos) < e->get_length()) {
        if (e->get_data(*pos) != 0)
            break;
    }

    return f;
}

bool MIDIShowControlPacket::parse_ascii_num(
    MIDISystemExclusive const* e, int* pos, std::uint32_t* num)
{
    bool f = true;
    std::uint32_t v = 0;
    std::uint8_t c = 0;
    //
    // Read ascii decimal digits until '.' or 0x00 or 0xf7
    //

    while (*pos < e->get_length()) {
        c = e->get_data((*pos));

        if (c >= '0' && c <= '9') {
            //
            // If the character is ascii number then shift our current value and
            // add the digit
            //
            v *= 10;
            v += c - '0';
        }

        else if (c == 0x00 || c == 0xf7 || c == '.') {
            //
            // If the character is 0 or EOX or . then this field is over
            //
            *num = v;
            return f;
        }

        (*pos)++;
    }

    *num = v;
    return false;
}

}  // namespace jdksmidi
