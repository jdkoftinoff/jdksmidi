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
#include "jdksmidi/process.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

MIDIProcessor::MIDIProcessor()
{}

MIDIProcessor::~MIDIProcessor()
{}

MIDIMultiProcessor::MIDIMultiProcessor(int num)
    : processors(new MIDIProcessor*[num])
    , num_processors(num)
{
    for (int i = 0; i < num_processors; ++i) {
        processors[i] = 0;
    }
}

MIDIMultiProcessor::~MIDIMultiProcessor()
{
    delete[] processors;
}

bool MIDIMultiProcessor::Process(MIDITimedBigMessage* msg)
{
    for (int i = 0; i < num_processors; ++i) {
        if (processors[i]) {
            if (processors[i]->Process(msg) == false) {
                return false;
            }
        }
    }

    return true;
}

MIDIProcessorTransposer::MIDIProcessorTransposer()
{
    for (int i = 0; i < 16; ++i) {
        trans_amount[i] = 0;
    }
}

MIDIProcessorTransposer::~MIDIProcessorTransposer()
{}

void MIDIProcessorTransposer::SetAllTranspose(int val)
{
    for (int chan = 0; chan < 16; ++chan) {
        trans_amount[chan] = val;
    }
}

bool MIDIProcessorTransposer::Process(MIDITimedBigMessage* msg)
{
    if (msg->IsChannelMsg()) {
        if (msg->IsNoteOn() || msg->IsNoteOff() || msg->IsPolyPressure()) {
            int trans = trans_amount[msg->GetChannel()];
            int new_note = ((int)msg->GetNote()) + trans;

            if (trans > 127 || trans < 0) {
                // delete event if out of range
                return false;
            }

            else {
                // set new note number
                msg->SetNote((std::uint8_t)new_note);
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

MIDIProcessorRechannelizer::~MIDIProcessorRechannelizer()
{}

void MIDIProcessorRechannelizer::SetAllRechan(int dest_chan)
{
    for (int i = 0; i < 16; ++i) {
        rechan_map[i] = dest_chan;
    }
}

bool MIDIProcessorRechannelizer::Process(MIDITimedBigMessage* msg)
{
    if (msg->IsChannelMsg()) {
        int new_chan = rechan_map[msg->GetChannel()];

        if (new_chan == -1) {
            // this channel is to be deleted! return false
            return false;
        }

        msg->SetChannel((std::uint8_t)new_chan);
    }

    return true;
}

}  // namespace jdksmidi
