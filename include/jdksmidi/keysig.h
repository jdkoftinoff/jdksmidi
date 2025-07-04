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

#ifndef JDKSMIDI_KEYSIG_H
#define JDKSMIDI_KEYSIG_H

#include "jdksmidi/midi.h"

namespace jdksmidi {
enum MIDIAccidentalType
{
    ACCFlat = 0,
    ACCNatural,
    ACCSharp
};

class MIDIKeySignature
{
  public:
    MIDIKeySignature();
    MIDIKeySignature(MIDIKeySignature const& k);

    void reset();

    bool is_major() { return major; }

    void set_sharp_flats(int sf, bool maj = true)
    {
        sharp_flat = sf;
        major = maj;
        reset();
    }

    int get_sharp_flats() { return sharp_flat; }

    MIDIAccidentalType get_note_status(int white_note) { return state[white_note % 7]; }

    bool convert_midi_note(int in_note, int* out_note);

  protected:
    bool process_white_note(int in_note, int* out_note);
    bool process_black_note(int in_note, int* out_note);

    MIDIAccidentalType state[7];
    bool use_sharps;
    int sharp_flat;
    bool major;

    static int sharp_list[7];
    static int flat_list[7];
};
}  // namespace jdksmidi

#endif
