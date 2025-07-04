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
#include "jdksmidi/world.h"

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
    ENTER("MIDIKeySignature::MIDIKeySignature()");
    use_sharps = true;
    sharp_flat = 0;
    major = true;
    reset();
}

MIDIKeySignature::MIDIKeySignature(MIDIKeySignature const& k)
{
    ENTER("MIDIKeySignature::MIDIKeySignature()");
    use_sharps = k.use_sharps;
    sharp_flat = k.sharp_flat;
    major = k.major;
    reset();
}

//
// reset() generates the sharp/flat list based on sharp_flat, major,
// and use_sharps.
//

void MIDIKeySignature::reset()
{
    ENTER("MIDIKeySignature::reset()");

    if (sharp_flat < -7)
        sharp_flat = -7;

    if (sharp_flat > 7)
        sharp_flat = 7;

    for (int note = 0; note < 7; ++note)
        state[note] = ACCNatural;

    if (sharp_flat == 0) {
        // Key of C has no sharps or flats.
        // and any accidentals will be sharp
        use_sharps = true;
    }

    else if (sharp_flat > 0) {
        //
        // this key has a number of sharps in it.
        //
        use_sharps = true;

        for (short i = 0; i < sharp_flat; ++i) {
            state[sharp_list[i]] = ACCSharp;
        }
    }

    else if (sharp_flat < 0) {
        //
        // this key has flats in it.
        // -sharp_flat is how many flats.
        //
        int flats = -sharp_flat;
        use_sharps = false;

        for (int i = 0; i < flats; ++i) {
            state[flat_list[i]] = ACCFlat;
        }
    }
}

bool MIDIKeySignature::ProcessWhiteNote(int in_note, int* out_note)
{
    ENTER("MIDIKeySignature::ProcessWhiteNote()");
    //
    // check to see if this white note is allowed in the current
    // state.
    //

    if (state[in_note] == ACCNatural) {
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
        state[in_note] = ACCNatural;
        //
        // return true because it needed an accidental
        //
        return true;
    }
}

bool MIDIKeySignature::ProcessBlackNote(int in_note, int* out_note)
{
    ENTER("MIDIKeySignature::ProcessBlackNote()");
    //
    // if this note is already sharped,
    // return the note unchanged and return false
    // because no accidental was required

    if (state[in_note] == ACCSharp) {
        *out_note = in_note;
        return false;
    }

    //
    // if the next note is flatted, then we could use it
    // instead.
    //

    if (state[in_note + 1] == ACCFlat) {
        *out_note = in_note + 1;
        return false;
    }

    //
    // Couldn't find a black note. we gotta make one.
    // make a sharp if use_sharps==1

    if (use_sharps) {
        //
        // make this white note a sharp.
        //
        state[in_note] = ACCSharp;
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
        state[in_note + 1] = ACCFlat;
        *out_note = in_note + 1;
        //
        // Accidental required. Return true.
        //
        return true;
    }
}

//
// ConvertMIDINote() takes a real MIDI note number and converts it to a
// white-key note number. it returns true if an Accidental is required.
//

bool MIDIKeySignature::ConvertMIDINote(int in_note, int* out_note)
{
    ENTER("MIDIKeySignature::ConvertMIDINote()");
    int octave = in_note / 12;
    int midi_note = in_note % 12;
    int actual_note = 0;
    bool changed = false;

    switch (midi_note) {
        case 0:  // C
            changed = ProcessWhiteNote(0, &actual_note);
            break;
        case 2:  // D
            changed = ProcessWhiteNote(1, &actual_note);
            break;
        case 4:  // E
            changed = ProcessWhiteNote(2, &actual_note);
            break;
        case 5:  // F
            changed = ProcessWhiteNote(3, &actual_note);
            break;
        case 7:  // G
            changed = ProcessWhiteNote(4, &actual_note);
            break;
        case 9:  // A
            changed = ProcessWhiteNote(5, &actual_note);
            break;
        case 11:  // B
            changed = ProcessWhiteNote(6, &actual_note);
            break;
        case 1:  // C#
            changed = ProcessBlackNote(0, &actual_note);
            break;
        case 3:  // D#
            changed = ProcessBlackNote(1, &actual_note);
            break;
        case 6:  // F#
            changed = ProcessBlackNote(3, &actual_note);
            break;
        case 8:  // G#
            changed = ProcessBlackNote(4, &actual_note);
            break;
        case 10:  // A#
            changed = ProcessBlackNote(5, &actual_note);
            break;
    };

    *out_note = (octave * 7) + actual_note;

    return changed;
}

}  // namespace jdksmidi
