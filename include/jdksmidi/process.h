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
#ifndef JDKSMIDI_PROCESS_H
#define JDKSMIDI_PROCESS_H

#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

namespace jdksmidi {
class MIDIProcessor;
class MIDIMultiProcessor;
class MIDIProcessorTransposer;
class MIDIProcessor;

class MIDIProcessor
{
  public:
    MIDIProcessor();
    virtual ~MIDIProcessor();

    virtual bool process(MIDITimedBigMessage* msg) = 0;
};

class MIDIMultiProcessor : public MIDIProcessor
{
  public:
    MIDIMultiProcessor(int num_processors);
    virtual ~MIDIMultiProcessor();

    // MIDIProcessors given to a MIDIMultiProcessor are NOT owned
    // by MIDIMultiProcessor.

    void set_processor(int position, MIDIProcessor* proc) { processors[position] = proc; }

    MIDIProcessor* get_processor(int position) { return processors[position]; }

    MIDIProcessor const* get_processor(int position) const { return processors[position]; }

    virtual bool process(MIDITimedBigMessage* msg);

  private:
    MIDIProcessor** processors;
    int num_processors;
};

class MIDIProcessorTransposer : public MIDIProcessor
{
  public:
    MIDIProcessorTransposer();
    virtual ~MIDIProcessorTransposer();

    void set_transpose_channel(int chan, int trans) { trans_amount[chan] = trans; }

    int get_transpose_channel(int chan) const { return trans_amount[chan]; }

    void set_all_transpose(int trans);

    virtual bool process(MIDITimedBigMessage* msg);

  private:
    int trans_amount[16];
};

class MIDIProcessorRechannelizer : public MIDIProcessor
{
  public:
    MIDIProcessorRechannelizer();
    virtual ~MIDIProcessorRechannelizer();

    void set_rechan_map(int src_chan, int dest_chan) { rechan_map[src_chan] = dest_chan; }

    int get_rechan_map(int src_chan) const { return rechan_map[src_chan]; }

    void set_all_rechan(int dest_chan);

    virtual bool process(MIDITimedBigMessage* msg);

  private:
    int rechan_map[16];
};
}  // namespace jdksmidi

#endif
