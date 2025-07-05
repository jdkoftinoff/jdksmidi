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
#include "jdksmidi/msg.h"
#include "jdksmidi/process.h"

#include <cstdint>
#include <vector>

namespace jdksmidi {

MIDIProcessor::MIDIProcessor() = default;

MIDIProcessor::~MIDIProcessor() = default;

MIDIMultiProcessor::MIDIMultiProcessor(int num)
    : processors(num, nullptr)
{}

MIDIMultiProcessor::~MIDIMultiProcessor() = default;

bool MIDIMultiProcessor::process(MIDITimedBigMessage* msg)
{
    for (auto* processor : processors) {
        if (processor) {
            if (processor->process(msg) == false) {
                return false;
            }
        }
    }

    return true;
}

MIDIProcessorTransposer::MIDIProcessorTransposer()
{
    for (auto& i : trans_amount) {
        i = 0;
    }
}

MIDIProcessorTransposer::~MIDIProcessorTransposer() = default;

void MIDIProcessorTransposer::set_all_transpose(int val)
{
    for (auto& chan : trans_amount) {
        chan = val;
    }
}

bool MIDIProcessorTransposer::process(MIDITimedBigMessage* msg)
{
    if (msg->is_channel_msg()) {
        if (msg->is_note_on() || msg->is_note_off() || msg->is_poly_pressure()) {
            int trans = trans_amount[msg->get_channel()];
            int new_note = ((int)msg->get_note()) + trans;

            if (trans > 127 || trans < 0) {
                // delete event if out of range
                return false;
            }

            else {
                // set new note number
                msg->set_note((std::uint8_t)new_note);
            }
        }
    }

    return true;
}

MIDIProcessorRechannelizer::MIDIProcessorRechannelizer()
{
    for (int i = 0; i < 16; ++i) {
        rechan_map[i] = i;
    }
}

MIDIProcessorRechannelizer::~MIDIProcessorRechannelizer() = default;

void MIDIProcessorRechannelizer::set_all_rechan(int dest_chan)
{
    for (auto& i : rechan_map) {
        i = dest_chan;
    }
}

bool MIDIProcessorRechannelizer::process(MIDITimedBigMessage* msg)
{
    if (msg->is_channel_msg()) {
        int new_chan = rechan_map[msg->get_channel()];

        if (new_chan == -1) {
            // this channel is to be deleted! return false
            return false;
        }

        msg->set_channel((std::uint8_t)new_chan);
    }

    return true;
}

}  // namespace jdksmidi
