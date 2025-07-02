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

#ifndef JDKSMIDI_FILE_H
#define JDKSMIDI_FILE_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/tempo.h"

#include <cstdint>

namespace jdksmidi {

//
// The MIDIFile class contains definitions and utilities to deal with
// reading and writing midi files.
//

unsigned long const _MThd = OSTYPE('M', 'T', 'h', 'd');
unsigned long const _MTrk = OSTYPE('M', 'T', 'r', 'k');

class MIDIFile
{
  public:
    MIDIFile();
    virtual ~MIDIFile();

    struct MIDIFileChunk
    {
        unsigned long id;
        unsigned long length;
    };

    struct MIDIFileHeader
    {
        short format;
        short ntrks;
        short division;
    };

    //
    // define all the different meta event message types.
    //

    enum
    {
        MF_SEQUENCE_NUMBER = 0,
        MF_TEXT_EVENT = 1,
        MF_COPYRIGHT = 2,
        MF_TRACK_NAME = 3,
        MF_INSTRUMENT_NAME = 4,
        MF_LYRIC = 5,
        MF_MARKER = 6,
        MF_CUE_POINT = 7,
        MF_GENERIC_TEXT_8 = 8,
        MF_GENERIC_TEXT_9 = 9,
        MF_GENERIC_TEXT_A = 0xA,
        MF_GENERIC_TEXT_B = 0xB,
        MF_GENERIC_TEXT_C = 0xC,
        MF_GENERIC_TEXT_D = 0xD,
        MF_GENERIC_TEXT_E = 0xE,
        MF_GENERIC_TEXT_F = 0xF,

        MF_OUTPUT_CABLE = 0x21,
        MF_TRACK_LOOP = 0x2E,
        MF_END_OF_TRACK = 0x2F,
        MF_TEMPO = 0x51,
        MF_SMPTE = 0x54,
        MF_TIMESIG = 0x58,
        MF_KEYSIG = 0x59,
        MF_SEQUENCER_SPECIFIC = 0x7F
    };

    //
    // ConvertTempoToFreq() returns the frequency of the required
    // tempo clock
    //

    static unsigned long ConvertTempoToFreq(short division, MIDITempo& tempo);

    //
    // Convert a four byte number to an unsigned long.
    //

    static unsigned long To32Bit(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d)
    {
        return ((unsigned long)a << 24) + ((unsigned long)b << 16) + ((unsigned long)c << 8) +
            ((unsigned long)d << 0);
    }

    //
    // Convert a two byte number to an unsigned short
    //

    static unsigned short To16Bit(std::uint8_t a, std::uint8_t b)
    {
        return (unsigned short)(((unsigned short)a << 8) + ((unsigned short)b << 0));
    }

    static unsigned long ReadVariableLengthNumber(std::uint8_t** in);

    static std::uint8_t* WriteVariableLengthNumber(unsigned long num, std::uint8_t* out);
};

}  // namespace jdksmidi

#endif
