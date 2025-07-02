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
        channel_count[channel] = 0;
        hold_pedal[channel] = false;

        for (std::uint8_t note = 0; note < 128; note++)
            note_on_count[channel][note] = 0;
    }

    total_count = 0;
}

MIDIMatrix::~MIDIMatrix()
{
    ENTER("MIDIMatrix::~MIDIMatrix()");
}

void MIDIMatrix::DecNoteCount(MIDIMessage const&, int channel, int note)
{
    ENTER("MIDIMatrix::DecNoteCount()");

    if (note_on_count[channel][note] > 0) {
        --note_on_count[channel][note];
        --channel_count[channel];
        --total_count;
    }
}

void MIDIMatrix::IncNoteCount(MIDIMessage const&, int channel, int note)
{
    ENTER("MIDIMatrix::IncNoteCount()");
    ++note_on_count[channel][note];
    ++channel_count[channel];
    ++total_count;
}

void MIDIMatrix::OtherMessage(MIDIMessage const&)
{
    ENTER("MIDIMatrix::OtherMessage()");
}

bool MIDIMatrix::Process(MIDIMessage const& m)
{
    ENTER("MIDIMatrix::Process()");
    bool status = false;

    if (m.IsChannelMsg()) {
        int channel = m.GetChannel();
        int note = m.GetNote();

        if (m.IsAllNotesOff()) {
            ClearChannel(channel);
            status = true;
        }

        else if (m.IsNoteOn()) {
            if (m.GetVelocity() != 0)
                IncNoteCount(m, channel, note);

            else
                DecNoteCount(m, channel, note);

            status = true;
        }

        else if (m.IsNoteOff()) {
            DecNoteCount(m, channel, note);
            status = true;
        }

        else if (m.IsControlChange() && m.GetController() == C_DAMPER) {
            if (m.GetControllerValue() & 0x40) {
                hold_pedal[channel] = true;
            }

            else {
                hold_pedal[channel] = false;
            }
        }

        else
            OtherMessage(m);
    }

    return status;
}

void MIDIMatrix::Clear()
{
    ENTER("MIDIMatrix::Clear()");

    for (int channel = 0; channel < 16; ++channel) {
        ClearChannel(channel);
    }

    total_count = 0;
}

void MIDIMatrix::ClearChannel(int channel)
{
    ENTER("MIDIMatrix::ClearChannel()");

    for (int note = 0; note < 128; ++note) {
        total_count -= note_on_count[channel][note];
        note_on_count[channel][note] = 0;
    }

    channel_count[channel] = 0;
    hold_pedal[channel] = 0;
}

}  // namespace jdksmidi
