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
#include "jdksmidi/driver.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

MIDIDriver::MIDIDriver(int queue_size)
    : in_queue(queue_size)
    , out_queue(queue_size)
    , in_proc(0)
    , out_proc(0)
    , thru_proc(0)
    , thru_enable(false)
    , tick_proc(0)
{}

MIDIDriver::~MIDIDriver()
{}

void MIDIDriver::Reset()
{
    in_queue.Clear();
    out_queue.Clear();
    out_matrix.Clear();
}

void MIDIDriver::AllNotesOff(int chan)
{
    MIDITimedBigMessage msg;
    // send a note off for every note on in the out_matrix

    if (out_matrix.GetChannelCount(chan) > 0) {
        for (int note = 0; note < 128; ++note) {
            while (out_matrix.GetNoteCount(chan, note) > 0) {
                // make a note off with note on msg, velocity 0
                msg.SetNoteOn(static_cast<std::uint8_t>(chan), static_cast<std::uint8_t>(note), 0);
                OutputMessage(msg);
            }
        }
    }

    msg.SetControlChange(chan, C_DAMPER, 0);
    OutputMessage(msg);
    msg.SetAllNotesOff(static_cast<std::uint8_t>(chan));
    OutputMessage(msg);
}

void MIDIDriver::AllNotesOff()
{
    for (int i = 0; i < 16; ++i) {
        AllNotesOff(i);
    }
}

bool MIDIDriver::HardwareMsgIn(MIDITimedBigMessage& msg)
{
    // put input midi messages thru the in processor
    if (in_proc) {
        if (in_proc->Process(&msg) == false) {
            // message was deleted, so ignore it.
            return true;
        }
    }

    // stick input into in queue

    if (in_queue.CanPut()) {
        in_queue.Put(msg);
    }

    else {
        return false;
    }

    // now stick it through the THRU processor

    if (thru_proc) {
        if (thru_proc->Process(&msg) == false) {
            // message was deleted, so ignore it.
            return true;
        }
    }

    if (thru_enable) {
        // stick this message into the out queue so the tick procedure
        // will play it out asap
        if (out_queue.CanPut()) {
            out_queue.Put(msg);
        }

        else {
            return false;
        }
    }

    return true;
}

void MIDIDriver::TimeTick(unsigned long sys_time)
{
    // run the additional tick procedure if we need to
    if (tick_proc) {
        tick_proc->TimeTick(sys_time);
    }

    // feed as many midi messages from out_queu to the hardware out port
    // as we can

    while (out_queue.CanGet()) {
        // use the Peek() function to avoid allocating memory for
        // a duplicate sysex
        if (HardwareMsgOut(*(out_queue.Peek())) == true) {
            // ok, got and sent a message - update our out_queue now
            out_queue.Next();
        }

        else {
            // cant send any more, stop now.
            break;
        }
    }
}

}  // namespace jdksmidi
