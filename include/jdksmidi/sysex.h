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

#ifndef JDKSMIDI_SYSEX_H
#define JDKSMIDI_SYSEX_H

#include "jdksmidi/midi.h"

namespace jdksmidi {

class MIDISystemExclusive
{
  public:
    MIDISystemExclusive(int size = 384);

    MIDISystemExclusive(MIDISystemExclusive const& e);

    MIDISystemExclusive(unsigned char* buf_, int max_len_, int cur_len_, bool deletable_)
    {
        buf = buf_;
        max_len = max_len_;
        cur_len = cur_len_;
        chk_sum = 0;
        deletable = deletable_;
    }

    virtual ~MIDISystemExclusive();

    void Clear()
    {
        cur_len = 0;
        chk_sum = 0;
    }
    void ClearChecksum() { chk_sum = 0; }

    void PutSysByte(unsigned char b)  // does not add to chksum
    {
        if (cur_len < max_len)
            buf[cur_len++] = b;
    }

    void PutByte(unsigned char b)
    {
        PutSysByte(b);
        chk_sum += b;
    }

    void PutEXC() { PutSysByte(SYSEX_START); }
    void PutEOX() { PutSysByte(SYSEX_END); }

    // low nibble first
    void PutNibblizedByte(unsigned char b)
    {
        PutByte((unsigned char)(b & 0xf));
        PutByte((unsigned char)(b >> 4));
    }

    // high nibble first
    void PutNibblizedByte2(unsigned char b)
    {
        PutByte((unsigned char)(b >> 4));
        PutByte((unsigned char)(b & 0xf));
    }

    void PutChecksum() { PutByte((unsigned char)(chk_sum & 0x7f)); }

    unsigned char GetChecksum() const { return (unsigned char)(chk_sum & 0x7f); }

    int GetLength() const { return cur_len; }

    unsigned char GetData(int i) const { return buf[i]; }

    bool IsFull() const { return cur_len >= max_len; }

    unsigned char* GetBuf() { return buf; }

    unsigned char const* GetBuf() const { return buf; }

  private:
    unsigned char* buf;
    int max_len;
    int cur_len;
    unsigned char chk_sum;
    bool deletable;
};
}  // namespace jdksmidi

#endif
