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

#ifndef JDKSMIDI_PARSER_H
#define JDKSMIDI_PARSER_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <cstdint>
#include <memory>

namespace jdksmidi {

class MIDIParser
{
  public:
    MIDIParser(std::uint16_t max_sysex_size = 384);
    virtual ~MIDIParser();

    void Clear() { state = FIND_STATUS; }

    virtual bool Parse(std::uint8_t b, MIDIMessage* msg);

    MIDISystemExclusive* GetSystemExclusive() const { return sysex.get(); }

  protected:
    //
    // The states used for parsing messages.
    //

    enum State
    {
        FIND_STATUS,         // ignore data bytes
        FIRST_OF_ONE,        // read first data byte of a one data byte msg
        FIRST_OF_TWO,        // read first data byte of two data byte msg
        SECOND_OF_TWO,       // read second data byte of two data byte msg
        FIRST_OF_ONE_NORUN,  // read one byte message, do not allow
        // running status (for MTC)
        SYSEX_DATA  // read sysex data byte
    };

    MIDIMessage tmp_msg;
    std::unique_ptr<MIDISystemExclusive> sysex;
    State state;

    bool ParseSystemByte(std::uint8_t b, MIDIMessage* msg);
    bool ParseDataByte(std::uint8_t b, MIDIMessage* msg);
    void ParseStatusByte(std::uint8_t b);
};

}  // namespace jdksmidi

#endif
