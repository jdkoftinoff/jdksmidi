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

#include "jdksmidi/file.h"

#include <cstdint>  // for uint32_t, uint8_t, int32_t, int16_t

#if DEBUG_MDFILE
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIFile::MIDIFile()
{}

MIDIFile::~MIDIFile()
{}

std::uint32_t MIDIFile::convert_tempo_to_freq(std::int16_t division, MIDITempo& tempo)
{
    if (division > 0) {
        std::int32_t clocks_per_beat = static_cast<std::int32_t>(division) * 1000;
        std::int32_t micro_sec_per_beat = tempo.get_midi_file_tempo() / 1000;
        return (std::uint32_t)clocks_per_beat / micro_sec_per_beat;
    }

    else {
        // TO DO: handle smpte frame rate references
        return 120;
    }
}

std::uint32_t MIDIFile::read_variable_length_number(std::uint8_t** in)
{
    std::uint32_t num = 0;
    std::uint8_t* t = *in;

    do {
        num <<= 7;
        num |= (*t);
    } while ((*t++) & 0x80);

    *in = t;
    return num;
}

std::uint8_t* MIDIFile::write_variable_length_number(std::uint32_t num, std::uint8_t* out)
{
    std::uint32_t buffer;
    buffer = num & 0x7f;

    while ((num >>= 7) > 0) {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (num & 0x7f);
    }

    do {
        *out++ = (std::uint8_t)buffer;

        if (buffer & 0x80)
            buffer >>= 8;

        else
            break;
    } while (true);

    return out;
}

}  // namespace jdksmidi
