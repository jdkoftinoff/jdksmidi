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
#ifndef JDKSMIDI_QUEUE_H
#define JDKSMIDI_QUEUE_H

#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

namespace jdksmidi {

class MIDIQueue
{
  public:
    MIDIQueue(int num_msgs);
    virtual ~MIDIQueue();

    void clear();

    bool can_put() const;

    bool can_get() const;

    bool is_full() const { return !can_put(); }

    void put(MIDITimedBigMessage const& msg)
    {
        _buf[_next_in] = msg;
        _next_in = (_next_in + 1) % _bufsize;
    }

    MIDITimedBigMessage get() const { return MIDITimedBigMessage(_buf[_next_out]); }

    void next() { _next_out = (_next_out + 1) % _bufsize; }

    MIDITimedBigMessage const* peek() const { return &_buf[_next_out]; }

  protected:
    MIDITimedBigMessage* _buf;
    int _bufsize;
    int volatile _next_in;
    int volatile _next_out;
};

}  // namespace jdksmidi

#endif
