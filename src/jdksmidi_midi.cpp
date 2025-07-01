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

#include "jdksmidi/midi.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

signed char const lut_msglen[16] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    3,  // 0x80=note off, 3 bytes
    3,  // 0x90=note on, 3 bytes
    3,  // 0xa0=poly pressure, 3 bytes
    3,  // 0xb0=control change, 3 bytes
    2,  // 0xc0=program change, 2 bytes
    2,  // 0xd0=channel pressure, 2 bytes
    3,  // 0xe0=pitch bend, 3 bytes
    -1  // 0xf0=other things. may vary.
};

signed char const lut_sysmsglen[16] = {
    -1,  // 0xf0=sysex start. may vary
    2,   // 0xf1=MIDI Time Code. 2 bytes
    3,   // 0xf2=MIDI Song position. 3 bytes
    2,   // 0xf3=MIDI Song Select. 2 bytes.
    0,   // 0xf4=undefined
    0,   // 0xf5=undefined
    1,   // 0xf6=TUNE Request
    0,   // 0xf7=sysex end.
    1,   // 0xf8=timing clock. 1 byte
    1,   // 0xf9=proposed measure end?
    1,   // 0xfa=start. 1 byte
    1,   // 0xfb=continue. 1 byte
    1,   // 0xfc=stop. 1 byte
    0,   // 0xfd=undefined
    1,   // 0xfe=active sensing. 1 byte
    3    // 0xff= not reset, but a META-EVENT, which is always 3 bytes
};

bool const lut_is_white[12] = {
    //
    // C C#  D D#  E    F F# G G# A A# B
    //
    1,
    0,
    1,
    0,
    1,
    1,
    0,
    1,
    0,
    1,
    0,
    1};

}  // namespace jdksmidi
