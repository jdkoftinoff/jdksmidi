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
#include "jdksmidi/matrix.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/process.h"
#include "jdksmidi/queue.h"
#include "jdksmidi/tick.h"

#include <cstdint>

namespace jdksmidi {

MIDIDriver::MIDIDriver(int queue_size)
    : _in_queue(queue_size)
    , _out_queue(queue_size)
    , _in_proc(nullptr)
    , _out_proc(nullptr)
    , _thru_proc(nullptr)
    , _thru_enable(false)
    , _tick_proc(nullptr)
{}

MIDIDriver::~MIDIDriver() = default;

void MIDIDriver::reset()
{
    _in_queue.clear();
    _out_queue.clear();
    _out_matrix.clear();
}

void MIDIDriver::all_notes_off(int chan)
{
    MIDITimedBigMessage msg;
    // send a note off for every note on in the out_matrix

    if (_out_matrix.get_channel_count(chan) > 0) {
        for (int note = 0; note < 128; ++note) {
            while (_out_matrix.get_note_count(chan, note) > 0) {
                // make a note off with note on msg, velocity 0
                msg.set_note_on(
                    static_cast<std::uint8_t>(chan), static_cast<std::uint8_t>(note), 0);
                output_message(msg);
            }
        }
    }

    msg.set_control_change(chan, C_DAMPER, 0);
    output_message(msg);
    msg.set_all_notes_off(static_cast<std::uint8_t>(chan));
    output_message(msg);
}

void MIDIDriver::all_notes_off()
{
    for (int i = 0; i < 16; ++i) {
        all_notes_off(i);
    }
}

bool MIDIDriver::hardware_msg_in(MIDITimedBigMessage& msg)
{
    // put input midi messages thru the in processor
    if (_in_proc) {
        if (_in_proc->process(&msg) == false) {
            // message was deleted, so ignore it.
            return true;
        }
    }

    // stick input into in queue

    if (_in_queue.can_put()) {
        _in_queue.put(msg);
    }

    else {
        return false;
    }

    // now stick it through the THRU processor

    if (_thru_proc) {
        if (_thru_proc->process(&msg) == false) {
            // message was deleted, so ignore it.
            return true;
        }
    }

    if (_thru_enable) {
        // stick this message into the out queue so the tick procedure
        // will play it out asap
        if (_out_queue.can_put()) {
            _out_queue.put(msg);
        }

        else {
            return false;
        }
    }

    return true;
}

void MIDIDriver::time_tick(std::uint32_t sys_time)
{
    // run the additional tick procedure if we need to
    if (_tick_proc) {
        _tick_proc->time_tick(sys_time);
    }

    // feed as many midi messages from out_queu to the hardware out port
    // as we can

    while (_out_queue.can_get()) {
        // use the peek() function to avoid allocating memory for
        // a duplicate sysex
        if (hardware_msg_out(*(_out_queue.peek())) == true) {
            // ok, got and sent a message - update our out_queue now
            _out_queue.next();
        }

        else {
            // cant send any more, stop now.
            break;
        }
    }
}

}  // namespace jdksmidi
