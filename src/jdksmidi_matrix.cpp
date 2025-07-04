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

#include "jdksmidi/matrix.h"
#include "jdksmidi/world.h"

#ifndef DEBUG_MDMATRIX
#    define DEBUG_MDMATRIX 0
#endif

#if DEBUG_MDMATRIX
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIMatrix::MIDIMatrix()
{
    ENTER("MIDIMatrix::MIDIMatrix()");

    for (int channel = 0; channel < 16; channel++) {
        _channel_count[channel] = 0;
        _hold_pedal[channel] = false;

        for (std::uint8_t note = 0; note < 128; note++)
            _note_on_count[channel][note] = 0;
    }

    _total_count = 0;
}

MIDIMatrix::~MIDIMatrix()
{
    ENTER("MIDIMatrix::~MIDIMatrix()");
}

void MIDIMatrix::dec_note_count(MIDIMessage const&, int channel, int note)
{
    ENTER("MIDIMatrix::dec_note_count()");

    if (_note_on_count[channel][note] > 0) {
        --_note_on_count[channel][note];
        --_channel_count[channel];
        --_total_count;
    }
}

void MIDIMatrix::inc_note_count(MIDIMessage const&, int channel, int note)
{
    ENTER("MIDIMatrix::inc_note_count()");
    ++_note_on_count[channel][note];
    ++_channel_count[channel];
    ++_total_count;
}

void MIDIMatrix::other_message(MIDIMessage const&)
{
    ENTER("MIDIMatrix::other_message()");
}

bool MIDIMatrix::process(MIDIMessage const& m)
{
    ENTER("MIDIMatrix::process()");
    bool status = false;

    if (m.is_channel_msg()) {
        int channel = m.get_channel();
        int note = m.get_note();

        if (m.is_all_notes_off()) {
            clear_channel(channel);
            status = true;
        }

        else if (m.is_note_on()) {
            if (m.get_velocity() != 0)
                inc_note_count(m, channel, note);

            else
                dec_note_count(m, channel, note);

            status = true;
        }

        else if (m.is_note_off()) {
            dec_note_count(m, channel, note);
            status = true;
        }

        else if (m.is_control_change() && m.get_controller() == C_DAMPER) {
            if (m.get_controller_value() & 0x40) {
                _hold_pedal[channel] = true;
            }

            else {
                _hold_pedal[channel] = false;
            }
        }

        else
            other_message(m);
    }

    return status;
}

void MIDIMatrix::clear()
{
    ENTER("MIDIMatrix::clear()");

    for (int channel = 0; channel < 16; ++channel) {
        clear_channel(channel);
    }

    _total_count = 0;
}

void MIDIMatrix::clear_channel(int channel)
{
    ENTER("MIDIMatrix::clear_channel()");

    for (int note = 0; note < 128; ++note) {
        _total_count -= _note_on_count[channel][note];
        _note_on_count[channel][note] = 0;
    }

    _channel_count[channel] = 0;
    _hold_pedal[channel] = 0;
}

}  // namespace jdksmidi
