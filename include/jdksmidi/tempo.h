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

#ifndef JDKSMIDI_TEMPO_H
#define JDKSMIDI_TEMPO_H

//
// This class makes it easy to deal with Tempos as fixed point
// numbers.
//
// The actual tempo is stored times 256 for 1/256 bpm accuracy.
//
// The default operator int() etc., automatically convert the
// fixed point number so the value is in normal beats per minutes.
//
//

#include "jdksmidi/midi.h"

namespace jdksmidi {

class MIDITempo
{
  public:
    MIDITempo() { _tempo = 120 << 8; }
    MIDITempo(int a) { _tempo = static_cast<unsigned long>(a) << 8; }
    MIDITempo(unsigned int a) { _tempo = static_cast<unsigned long>(a) << 8; }
    MIDITempo(long a) { _tempo = static_cast<unsigned long>(a) << 8; }
    MIDITempo(unsigned long a) { _tempo = a << 8; }
    MIDITempo(float a) { _tempo = static_cast<unsigned long>(a * 256.0); }
    MIDITempo(MIDITempo const& a) { _tempo = a.get_full_tempo(); }

    operator short() { return static_cast<short>((_tempo + 0x80) >> 8); }
    operator unsigned short() { return static_cast<unsigned short>((_tempo + 0x80) >> 8); }

    operator int() { return static_cast<int>((_tempo + 0x80) >> 8); }
    operator unsigned int() { return static_cast<unsigned int>((_tempo + 0x80) >> 8); }
    operator long() { return static_cast<long>((_tempo + 0x80) >> 8); }
    operator unsigned long() { return static_cast<unsigned long>((_tempo + 0x80) >> 8); }
    operator float() { return static_cast<float>(_tempo) / 256.0f; }
    void operator=(unsigned short a) { _tempo = static_cast<unsigned long>(a) << 8; }
    void operator=(short a) { _tempo = static_cast<unsigned long>(a) << 8; }

    void operator=(unsigned int a) { _tempo = static_cast<unsigned long>(a) << 8; }
    void operator=(int a) { _tempo = static_cast<unsigned long>(a) << 8; }
    void operator=(unsigned long a) { _tempo = static_cast<unsigned long>(a) << 8; }
    void operator=(long a) { _tempo = static_cast<unsigned long>(a) << 8; }

    void operator=(float a) { _tempo = static_cast<unsigned long>(a * 256.0); }

    unsigned long get_full_tempo() const { return _tempo; }
    void set_full_tempo(unsigned long v) { _tempo = v; }

    unsigned long get_midi_file_tempo()
    {
        if (_tempo)
            return (60000000L / 256) / _tempo;

        else
            return (60000000L / 256) / (120 * 256);
    }

  protected:
    unsigned long _tempo;
};

}  // namespace jdksmidi

#endif
