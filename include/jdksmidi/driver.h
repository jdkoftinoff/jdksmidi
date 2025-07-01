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

    virtual void Reset();

    // to get the midi in queue
    MIDIQueue* InputQueue() { return &in_queue; }

    MIDIQueue const* InputQueue() const { return &in_queue; }

    // to get the midi out queue
    MIDIQueue* OutputQueue() { return &out_queue; }

    MIDIQueue const* OutputQueue() const { return &out_queue; }

    //
    // returns true if the output queue is not full
    bool CanOutputMessage() const { return out_queue.CanPut(); }

    // processes message with the OutProcessor and then
    // puts the message in the out_queue
    void OutputMessage(MIDITimedBigMessage& msg)
    {
        if ((out_proc && out_proc->Process(&msg)) || !out_proc) {
            out_matrix.Process(msg);
            out_queue.Put(msg);
        }
    }

    void SetThruEnable(bool f) { thru_enable = f; }

    bool GetThruEnable() const { return thru_enable; }

    // to set the midi processors used for thru, out, and in
    void SetThruProcessor(MIDIProcessor* proc) { thru_proc = proc; }

    void SetOutProcessor(MIDIProcessor* proc) { out_proc = proc; }

    void SetInProcessor(MIDIProcessor* proc) { in_proc = proc; }

    void SetTickProc(MIDITick* tick) { tick_proc = tick; }

    // to send all notes off on selected midi chanel
    void AllNotesOff(int chan);

    // to send all notes off on all midi channels
    void AllNotesOff();

    // call handle midi in when a parsed midi message
    // comes in to the system. Can be called by a callback function
    // or by your TimeTick() function.

    virtual bool HardwareMsgIn(MIDITimedBigMessage& msg);

    // HardwareMsgOut() must be overriden by a subclass - It must
    // take

    virtual bool HardwareMsgOut(MIDITimedBigMessage const& msg) = 0;

    // the time tick procedure:
    //  manages in/out/thru to hardware
    // inherited from MIDITick.
    //
    // if you need to poll midi in hardware,
    // you can override this method - Call MIDIDriver::TimeTick(t)
    // first, You may then poll the midi in
    // hardware, parse the bytes, form a message, and give the
    // resulting message to HandleMsgIn to process it and put it in
    // the in_queue.

    virtual void TimeTick(unsigned long sys_time);

  protected:
    // the in and out queues
    MIDIQueue in_queue;
    MIDIQueue out_queue;

    // the processors
    MIDIProcessor* in_proc;
    MIDIProcessor* out_proc;
    MIDIProcessor* thru_proc;

    bool thru_enable;

    // additional TimeTick procedure

    MIDITick* tick_proc;

    // to keep track of notes on going to MIDI out

    MIDIMatrix out_matrix;
};

}  // namespace jdksmidi

#endif
