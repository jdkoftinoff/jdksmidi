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
#include "jdksmidi/queue.h"

#include <atomic>

namespace jdksmidi {

MIDIQueue::MIDIQueue(int num_msgs)
    : _buf(new MIDITimedBigMessage[num_msgs])
    , _bufsize(num_msgs)
    , _next_in(0)
    , _next_out(0)
{}

MIDIQueue::~MIDIQueue()
{
    delete[] _buf;
}

void MIDIQueue::clear()
{
    _next_in.store(0, std::memory_order_relaxed);
    _next_out.store(0, std::memory_order_relaxed);
}

bool MIDIQueue::can_put() const
{
    int current_in = _next_in.load(std::memory_order_acquire);
    int current_out = _next_out.load(std::memory_order_acquire);
    return current_out != ((current_in + 1) % _bufsize);
}

bool MIDIQueue::can_get() const
{
    int current_in = _next_in.load(std::memory_order_acquire);
    int current_out = _next_out.load(std::memory_order_acquire);
    return current_in != current_out;
}

}  // namespace jdksmidi
