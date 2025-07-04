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
#ifndef JDKSMIDI_DRIVER_H
#define JDKSMIDI_DRIVER_H

#include "jdksmidi/matrix.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/process.h"
#include "jdksmidi/queue.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/tick.h"

namespace jdksmidi {
class MIDIDriver : public MIDITick
{
  public:
    MIDIDriver(int queue_size);
    virtual ~MIDIDriver();

    virtual void reset();

    // to get the midi in queue
    MIDIQueue* input_queue() { return &_in_queue; }

    MIDIQueue const* input_queue() const { return &_in_queue; }

    // to get the midi out queue
    MIDIQueue* output_queue() { return &_out_queue; }

    MIDIQueue const* output_queue() const { return &_out_queue; }

    //
    // returns true if the output queue is not full
    bool can_output_message() const { return _out_queue.can_put(); }

    // processes message with the OutProcessor and then
    // puts the message in the out_queue
    void output_message(MIDITimedBigMessage& msg)
    {
        if ((_out_proc && _out_proc->process(&msg)) || !_out_proc) {
            _out_matrix.process(msg);
            _out_queue.put(msg);
        }
    }

    void set_thru_enable(bool f) { _thru_enable = f; }

    bool get_thru_enable() const { return _thru_enable; }

    // to set the midi processors used for thru, out, and in
    void set_thru_processor(MIDIProcessor* proc) { _thru_proc = proc; }

    void set_out_processor(MIDIProcessor* proc) { _out_proc = proc; }

    void set_in_processor(MIDIProcessor* proc) { _in_proc = proc; }

    void set_tick_proc(MIDITick* tick) { _tick_proc = tick; }

    // to send all notes off on selected midi chanel
    void all_notes_off(int chan);

    // to send all notes off on all midi channels
    void all_notes_off();

    // call handle midi in when a parsed midi message
    // comes in to the system. Can be called by a callback function
    // or by your time_tick() function.

    virtual bool hardware_msg_in(MIDITimedBigMessage& msg);

    // hardware_msg_out() must be overriden by a subclass - It must
    // take

    virtual bool hardware_msg_out(MIDITimedBigMessage const& msg) = 0;

    // the time tick procedure:
    //  manages in/out/thru to hardware
    // inherited from MIDITick.
    //
    // if you need to poll midi in hardware,
    // you can override this method - Call MIDIDriver::time_tick(t)
    // first, You may then poll the midi in
    // hardware, parse the bytes, form a message, and give the
    // resulting message to HandleMsgIn to process it and put it in
    // the in_queue.

    virtual void time_tick(unsigned long sys_time);

  protected:
    // the in and out queues
    MIDIQueue _in_queue;
    MIDIQueue _out_queue;

    // the processors
    MIDIProcessor* _in_proc;
    MIDIProcessor* _out_proc;
    MIDIProcessor* _thru_proc;

    bool _thru_enable;

    // additional TimeTick procedure

    MIDITick* _tick_proc;

    // to keep track of notes on going to MIDI out

    MIDIMatrix _out_matrix;
};

}  // namespace jdksmidi

#endif
