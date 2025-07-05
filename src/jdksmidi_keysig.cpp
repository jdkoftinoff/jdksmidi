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

#include "jdksmidi/keysig.h"

#include <cstdint>

#ifndef DEBUG_MDKEYSIG
#    define DEBUG_MDKEYSIG 0
#endif

#if DEBUG_MDKEYSIG
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

int MIDIKeySignature::sharp_list[7] = {3, 0, 4, 1, 5, 2, 6};
int MIDIKeySignature::flat_list[7] = {6, 2, 5, 1, 4, 0, 3};

MIDIKeySignature::MIDIKeySignature()
{
    _use_sharps = true;
    _sharp_flat = 0;
    _major = true;
    reset();
}

MIDIKeySignature::MIDIKeySignature(MIDIKeySignature const& k)
{
    _use_sharps = k._use_sharps;
    _sharp_flat = k._sharp_flat;
    _major = k._major;
    reset();
}

//
// reset() generates the sharp/flat list based on sharp_flat, major,
// and use_sharps.
//

void MIDIKeySignature::reset()
{

    if (_sharp_flat < -7)
        _sharp_flat = -7;

    if (_sharp_flat > 7)
        _sharp_flat = 7;

    for (auto& note : _state)
        note = ACCNatural;

    if (_sharp_flat == 0) {
        // Key of C has no sharps or flats.
        // and any accidentals will be sharp
        _use_sharps = true;
    }

    else if (_sharp_flat > 0) {
        //
        // this key has a number of sharps in it.
        //
        _use_sharps = true;

        for (std::int16_t i = 0; i < _sharp_flat; ++i) {
            _state[sharp_list[i]] = ACCSharp;
        }
    }

    else if (_sharp_flat < 0) {
        //
        // this key has flats in it.
        // -sharp_flat is how many flats.
        //
        int flats = -_sharp_flat;
        _use_sharps = false;

        for (int i = 0; i < flats; ++i) {
            _state[flat_list[i]] = ACCFlat;
        }
    }
}

bool MIDIKeySignature::process_white_note(int in_note, int* out_note)
{
    //
    // check to see if this white note is allowed in the current
    // state.
    //

    if (_state[in_note] == ACCNatural) {
        //
        // yes it is allowed, return it.
        //
        *out_note = in_note;
        //
        // return false to signify that this note doesn't need
        // an accidental.
        //
        return false;
    }

    else {
        //
        // no it was not allowed. We must change our state.
        // to allow it. return it.
        //
        *out_note = in_note;
        //
        // change the desired note to a natural
        //
        _state[in_note] = ACCNatural;
        //
        // return true because it needed an accidental
        //
        return true;
    }
}

bool MIDIKeySignature::process_black_note(int in_note, int* out_note)
{
    //
    // if this note is already sharped,
    // return the note unchanged and return false
    // because no accidental was required

    if (_state[in_note] == ACCSharp) {
        *out_note = in_note;
        return false;
    }

    //
    // if the next note is flatted, then we could use it
    // instead.
    //

    if (_state[in_note + 1] == ACCFlat) {
        *out_note = in_note + 1;
        return false;
    }

    //
    // Couldn't find a black note. we gotta make one.
    // make a sharp if use_sharps==1

    if (_use_sharps) {
        //
        // make this white note a sharp.
        //
        _state[in_note] = ACCSharp;
        *out_note = in_note;
        //
        // Accidental required. return true.
        //
        return true;
    }

    else {
        //
        // make the next white note a flat.
        //
        _state[in_note + 1] = ACCFlat;
        *out_note = in_note + 1;
        //
        // Accidental required. Return true.
        //
        return true;
    }
}

//
// convert_midi_note() takes a real MIDI note number and converts it to a
// white-key note number. it returns true if an Accidental is required.
//

bool MIDIKeySignature::convert_midi_note(int in_note, int* out_note)
{
    int octave = in_note / 12;
    int midi_note = in_note % 12;
    int actual_note = 0;
    bool changed = false;

    switch (midi_note) {
        case 0:  // C
            changed = process_white_note(0, &actual_note);
            break;
        case 2:  // D
            changed = process_white_note(1, &actual_note);
            break;
        case 4:  // E
            changed = process_white_note(2, &actual_note);
            break;
        case 5:  // F
            changed = process_white_note(3, &actual_note);
            break;
        case 7:  // G
            changed = process_white_note(4, &actual_note);
            break;
        case 9:  // A
            changed = process_white_note(5, &actual_note);
            break;
        case 11:  // B
            changed = process_white_note(6, &actual_note);
            break;
        case 1:  // C#
            changed = process_black_note(0, &actual_note);
            break;
        case 3:  // D#
            changed = process_black_note(1, &actual_note);
            break;
        case 6:  // F#
            changed = process_black_note(3, &actual_note);
            break;
        case 8:  // G#
            changed = process_black_note(4, &actual_note);
            break;
        case 10:  // A#
            changed = process_black_note(5, &actual_note);
            break;
    };

    *out_note = (octave * 7) + actual_note;

    return changed;
}

}  // namespace jdksmidi
