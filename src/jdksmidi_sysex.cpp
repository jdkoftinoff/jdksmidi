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

#include "jdksmidi/sysex.h"
#include "jdksmidi/world.h"

#ifndef DEBUG_MDSYSEX
#    define DEBUG_MDSYSEX 0
#endif

#if DEBUG_MDSYSEX
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDISystemExclusive::MIDISystemExclusive(int size_)
{
    ENTER("MIDISystemExclusive::MIDISystemExclusive");
    buf = new std::uint8_t[size_];

    if (buf)
        max_len = size_;

    else
        max_len = 0;

    cur_len = 0;
    chk_sum = 0;
    deletable = true;
}

MIDISystemExclusive::MIDISystemExclusive(MIDISystemExclusive const& e)
{
    buf = new unsigned char[e.max_len];
    max_len = e.max_len;
    cur_len = e.cur_len;
    chk_sum = e.chk_sum;
    deletable = true;

    for (int i = 0; i < cur_len; ++i) {
        buf[i] = e.buf[i];
    }
}

MIDISystemExclusive::~MIDISystemExclusive()
{
    ENTER("MIDISystemExclusive::~MIDISystemExclusive");

    if (deletable)
        delete[] buf;
}

}  // namespace jdksmidi
