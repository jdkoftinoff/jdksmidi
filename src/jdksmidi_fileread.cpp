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

#include "jdksmidi/file.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <cstdint>
#include <vector>

// TO DO: decide which way is right for this flag and fix it - The standard midi file format specs
// are (were?) unclear

#define MIDIFRD_ALLOW_STATUS_ACROSS_META_EVENT 0

namespace jdksmidi {

void MIDIFileEvents::update_time(MIDIClockTime delta_time)
{}

void MIDIFileEvents::chan_message(MIDITimedMessage const& msg)
{
    switch (msg.get_status() & 0xf0) {
        case NOTE_OFF:
            mf_note_off(msg);
            break;
        case NOTE_ON:

            if (msg.get_velocity() == 0) {
                mf_note_off(msg);
            }

            else {
                mf_note_on(msg);
            }

            break;
        case POLY_PRESSURE:
            mf_poly_after(msg);
            break;
        case CONTROL_CHANGE:

            if (msg.get_byte2() > C_ALL_NOTES_OFF) {
                mf_system_mode(msg);
            }

            else {
                mf_control(msg);
            }

            break;
        case PROGRAM_CHANGE:
            mf_program(msg);
            break;
        case CHANNEL_PRESSURE:
            mf_chan_after(msg);
            break;
        case PITCH_BEND:
            mf_bender(msg);
            break;
    }
}

void MIDIFileEvents::meta_event(MIDIClockTime time, int type, int leng, std::uint8_t* m)
{
    switch (type) {
        case MF_SEQUENCE_NUMBER:
            mf_seqnum(time, to_16_bit(m[0], m[1]));
            break;
        case MF_TEXT_EVENT:
        case MF_COPYRIGHT:
        case MF_TRACK_NAME:
        case MF_INSTRUMENT_NAME:
        case MF_LYRIC:
        case MF_MARKER:
        case MF_CUE_POINT:
        case MF_GENERIC_TEXT_8:
        case MF_GENERIC_TEXT_9:
        case MF_GENERIC_TEXT_A:
        case MF_GENERIC_TEXT_B:
        case MF_GENERIC_TEXT_C:
        case MF_GENERIC_TEXT_D:
        case MF_GENERIC_TEXT_E:
        case MF_GENERIC_TEXT_F:
            // These are all text events
            m[leng] = 0;  // make sure string ends in NULL
            mf_text(time, type, leng, m);
            break;
        case MF_OUTPUT_CABLE:
            // TO DO:
            break;
        case MF_TRACK_LOOP:
            // TO DO:
            break;
        case MF_END_OF_TRACK:  // End of Track
            mf_eot(time);
            break;
        case MF_TEMPO:  // Set Tempo
            mf_tempo(time, to_32_bit(0, m[0], m[1], m[2]));
            break;
        case MF_SMPTE:
            mf_smpte(time, m[0], m[1], m[2], m[3], m[4]);
            break;
        case MF_TIMESIG:
            mf_timesig(time, m[0], m[1], m[2], m[3]);
            break;
        case MF_KEYSIG: {
            char c = m[0];
            mf_keysig(time, c, m[1]);
        } break;
        case MF_SEQUENCER_SPECIFIC:
            mf_sqspecific(time, leng, m);
            break;
        default:
            mf_metamisc(time, type, leng, m);
            break;
    }
}

void MIDIFileEvents::mf_starttrack(int trk)
{}

void MIDIFileEvents::mf_endtrack(int trk)
{}

void MIDIFileEvents::mf_eot(MIDIClockTime time)
{}

void MIDIFileEvents::mf_error(char const* s)
{}

void MIDIFileEvents::mf_header(int a, int b, int c)
{}

void MIDIFileEvents::mf_arbitrary(MIDIClockTime time, int a, std::uint8_t* s)
{}

void MIDIFileEvents::mf_metamisc(MIDIClockTime time, int a, int b, std::uint8_t* s)
{}

void MIDIFileEvents::mf_seqnum(MIDIClockTime time, int a)
{}

void MIDIFileEvents::mf_smpte(MIDIClockTime time, int a, int b, int c, int d, int e)
{}

void MIDIFileEvents::mf_timesig(MIDIClockTime time, int a, int b, int c, int d)
{}

void MIDIFileEvents::mf_tempo(MIDIClockTime time, std::uint32_t a)
{}

void MIDIFileEvents::mf_keysig(MIDIClockTime time, int a, int b)
{}

void MIDIFileEvents::mf_sqspecific(MIDIClockTime time, int a, std::uint8_t* s)
{}

void MIDIFileEvents::mf_text(MIDIClockTime time, int a, int b, std::uint8_t* s)
{}

void MIDIFileEvents::mf_system_mode(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_note_on(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_note_off(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_poly_after(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_bender(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_program(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_chan_after(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_control(MIDITimedMessage const& msg)
{}

void MIDIFileEvents::mf_sysex(MIDIClockTime time, MIDISystemExclusive const& ex)
{}

MIDIFileRead::MIDIFileRead(
    MIDIFileReadStream* input_stream_, MIDIFileEvents* event_handler_, std::uint32_t max_msg_len_)
    : input_stream(input_stream_)
    , event_handler(event_handler_)
{
    no_merge = 0;
    cur_time = 0;
    skip_init = 1;
    to_be_read = 0;
    msg_index = 0;
    cur_track = 0;
    abort_parse = 0;
    message_buffer.resize(max_msg_len_);
}

MIDIFileRead::~MIDIFileRead() = default;

void MIDIFileRead::mf_error(char const* e)
{
    event_handler->mf_error(e);
    abort_parse = 1;
}

bool MIDIFileRead::parse()
{
    int n;
    n = read_header();

    if (n <= 0) {
        mf_error("No Tracks");
        return false;
    }

    for (cur_track = 0; cur_track < n; cur_track++) {
        read_track();

        if (abort_parse != 0) {
            return false;
        }
    }

    return true;
}

int MIDIFileRead::read_mt(std::uint32_t type, int skip)
{
    std::uint32_t read = 0;
    int c;
    read = JDKSMIDI_OSTYPE(e_get_c(), e_get_c(), e_get_c(), e_get_c());

    if (type != read) {
        if (skip != 0) {
            do {
                read <<= 8;
                c = e_get_c();
                read |= c;

                if (read == type)
                    return 1;

                if (abort_parse != 0)
                    return 0;
            } while (c != -1);
        }

        mf_error("Error looking for chunk type");
        return 0;
    }

    return 1;
}

int MIDIFileRead::read_header()
{
    int the_format;
    int ntrks;
    int division;

    if (read_mt(header_MThd, skip_init) == 0xffff)
        return 0;

    if (abort_parse != 0)
        return 0;

    to_be_read = read_32_bit();
    the_format = read_16_bit();
    ntrks = read_16_bit();
    division = read_16_bit();

    if (abort_parse != 0)
        return 0;

    header_format = the_format;
    header_ntrks = ntrks;
    header_division = division;
    event_handler->mf_header(the_format, ntrks, division);
    // printf( "\nto be read = %d\n", to_be_read );

    while (to_be_read > 0)
        e_get_c();

    return ntrks;
}

//
// read a track chunk
//

void MIDIFileRead::read_track()
{
    //
    // This array is indexed by the high half of a status byte.  Its
    // value is either the number of bytes needed (1 or 2) for a channel
    // message, or 0 (meaning it's not  a channel message).
    //
    static char chantype[] = {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,  // 0x00 through 0x70
        2,
        2,
        2,
        2,
        1,
        1,
        2,
        0  // 0x80 through 0xf0
    };
    std::uint32_t lookfor, lng;
    int c, c1, type;
    int sysexcontinue = 0;  // 1 if last message was unfinished sysex
    int running = 0;        // 1 when running status used
    int status = 0;         // (possible running) status byte
    int needed;

    if (read_mt(header_MTrk, 0) == 0xffff)
        return;

    to_be_read = read_32_bit();
    cur_time = 0;
    event_handler->mf_starttrack(cur_track);

    while (to_be_read > 0 && abort_parse == 0) {
        std::uint32_t deltat = read_variable_num();
        event_handler->update_time(deltat);
        cur_time += deltat;
        c = e_get_c();

        if (c == -1)
            break;

        if (sysexcontinue != 0 && c != 0xf7)
            mf_error("Error after expected continuation of SysEx");

        if ((c & 0x80) == 0) {
            if (status == 0)
                mf_error("Unexpected Running Status");

            running = 1;
            needed = chantype[(status >> 4) & 0xf];
        }

        else {
            // TO DO: is running status to be cleared or not when meta event happens?
#if MIDIFRD_ALLOW_STATUS_ACROSS_META_EVENT
            if (c != 0xff) {
                status = c;
                running = 0;
                needed = 0;
            }

#else
            status = c;
            running = 0;
            needed = chantype[(status >> 4) & 0xf];
#endif
        }

        if (needed != 0)  // ie. is it a channel message?
        {
            if (running != 0)
                c1 = c;

            else
                c1 = e_get_c();

            form_chan_message(
                static_cast<std::uint8_t>(status),
                static_cast<std::uint8_t>(c1),
                static_cast<std::uint8_t>((needed > 1) ? e_get_c() : 0));
            continue;
        }

        switch (c) {
            case 0xff:  // meta-event
                type = e_get_c();
                lng = read_variable_num();
                lookfor = to_be_read - lng;
                msg_init();

                while (to_be_read > lookfor) {
                    msg_add(e_get_c());
                }

                event_handler->meta_event(cur_time, type, msg_index, message_buffer.data());
                break;
            case 0xf0:  // start of sys-ex
                lng = read_variable_num();
                lookfor = to_be_read - lng;
                msg_init();
                msg_add(0xf0);

                while (to_be_read > lookfor)
                    msg_add(c = e_get_c());

                if (c == 0xf7 || no_merge == 0) {
                    // make a sysex object out of the raw sysex data
                    // the buffer is not to be deleted upon destruction of ex
                    MIDISystemExclusive ex(message_buffer.data(), msg_index, msg_index, false);
                    // give the sysex object to our event handler
                    event_handler->mf_sysex(cur_time, ex);
                }

                else
                    sysexcontinue = 1;  // merge into next msg

                break;
            case 0xf7:  // sysex continuation or
                // arbitary stuff
                lng = read_variable_num();
                lookfor = to_be_read - lng;

                if (sysexcontinue == 0)
                    msg_init();

                while (to_be_read > lookfor)
                    msg_add(c = e_get_c());

                if (sysexcontinue == 0) {
                    event_handler->mf_arbitrary(cur_time, msg_index, message_buffer.data());
                }

                else if (c == 0xf7) {
                    // make a sysex object out of the raw sysex data
                    // the buffer is not to be deleted upon destruction of ex
                    MIDISystemExclusive ex(message_buffer.data(), msg_index, msg_index, false);
                    event_handler->mf_sysex(cur_time, ex);
                    sysexcontinue = 0;
                }

                break;
            default:
                bad_byte(c);
                break;
        }
    }

    event_handler->mf_endtrack(cur_track);
    return;
}

std::uint32_t MIDIFileRead::read_variable_num()
{
    std::uint32_t value;
    int c;
    c = e_get_c();

    if (c == -1) {
        return 0;
    }

    value = c;

    if ((c & 0x80) != 0) {
        value &= 0x7f;

        do {
            c = e_get_c();
            value = (value << 7) + (c & 0x7f);
        } while ((c & 0x80) != 0);
    }

    return value;
}

std::uint32_t MIDIFileRead::read_32_bit()
{
    int c1, c2, c3, c4;
    c1 = e_get_c();
    c2 = e_get_c();
    c3 = e_get_c();
    c4 = e_get_c();
    return to_32_bit(
        static_cast<std::uint8_t>(c1),
        static_cast<std::uint8_t>(c2),
        static_cast<std::uint8_t>(c3),
        static_cast<std::uint8_t>(c4));
}

int MIDIFileRead::read_16_bit()
{
    int c1, c2;
    c1 = e_get_c();
    c2 = e_get_c();
    return to_16_bit(static_cast<std::uint8_t>(c1), static_cast<std::uint8_t>(c2));
}

int MIDIFileRead::e_get_c()
{
    int c;
    c = input_stream->read_char();

    if (c < 0) {
        mf_error("Unexpected Stream Error");
        abort_parse = 1;
        return -1;
    }

    --to_be_read;
    return c;
}

void MIDIFileRead::msg_add(int a)
{
    if (msg_index < static_cast<int>(message_buffer.size()))
        message_buffer[msg_index++] = static_cast<std::uint8_t>(a);
}

void MIDIFileRead::msg_init()
{
    msg_index = 0;
}

void MIDIFileRead::bad_byte(int c)
{
    mf_error("Unexpected Byte");
    abort_parse = 1;
}

void MIDIFileRead::form_chan_message(std::uint8_t st, std::uint8_t b1, std::uint8_t b2)
{
    MIDITimedMessage m;
    m.set_status(st);
    m.set_byte1(b1);
    m.set_byte2(b2);
    m.set_time(cur_time);

    if (st >= 0x80 && st < 0xf0) {
        event_handler->chan_message(m);
    }
}

}  // namespace jdksmidi
