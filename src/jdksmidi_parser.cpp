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

#include "jdksmidi/parser.h"

#include <cstdint>
#include <memory>

#ifndef DEBUG_MDPARSER
#    define DEBUG_MDPARSER 0
#endif

#if DEBUG_MDPARSER
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDIParser::MIDIParser(std::uint16_t max_sysex_size)
{
    sysex = std::make_unique<MIDISystemExclusive>(max_sysex_size);
    state = FIND_STATUS;
}

MIDIParser::~MIDIParser()
{}

bool MIDIParser::parse(std::uint8_t b, MIDIMessage* msg)
{
    //
    // No matter what state we are currently in we must deal
    // with bytes with the high bit set first.
    //

    if (b & 0x80) {
        //
        // check for system messages (>=0xf0)
        //
        std::uint8_t stat = (std::uint8_t)(b & 0xf0);

        if (stat == 0xf0) {
            //
            // System messages get parsed by
            // parse_system_byte()
            //
            return parse_system_byte(b, msg);
        }

        else {
            //
            // Otherwise, this is a new status byte.
            //
            parse_status_byte(b);
            return false;
        }
    }

    else {
        //
        // Try to parse the data byte
        //
        return parse_data_byte(b, msg);
    }
}

bool MIDIParser::parse_system_byte(std::uint8_t b, MIDIMessage* msg)
{

    switch (b) {
        case RESET: {
            //
            // a reset byte always re-initializes our state
            // machine.
            //
            state = FIND_STATUS;
            return false;
        }
        case SYSEX_START: {
            //
            // start receiving sys-ex data
            //
            state = SYSEX_DATA;
            //
            // Prepare sysex buffer.
            //
            sysex->clear();
            sysex->put_exc();
            return false;
        }
        case SYSEX_END: {
            //
            // We are finished receiving a sysex message.
            //
            //
            // If we were not in SYSEX_DATA mode, this
            // EOX means nothing.
            //
            if (state != SYSEX_DATA) {
                return false;
            }

            //
            // reset the state machine
            //
            state = FIND_STATUS;
            //
            // finish up sysex buffer
            //
            sysex->put_eox();
            //
            // return a MIDIMessage with status=SYSEX_START
            // so calling program can know to look at
            // the sysex buffer with get_system_exclusive().
            //
            msg->set_status(SYSEX_START);
            return true;
        }
        case MTC: {
            //
            // Go into FIRST_OF_ONE_NORUN state.
            // this is required because MTC (F1) is not
            // allowed to be running status.
            //
            tmp_msg.set_status(MTC);
            state = FIRST_OF_ONE_NORUN;
            return false;
        }
        case SONG_POSITION: {
            //
            // This is a two data byte message, so go into
            // FIRST_OF_TWO state.
            //
            state = FIRST_OF_TWO;
            tmp_msg.set_status(SONG_POSITION);
            return false;
        }
        case SONG_SELECT: {
            //
            // This is a one data byte message, so go into
            // the FIRST_OF_ONE state.
            //
            state = FIRST_OF_ONE;
            tmp_msg.set_status(SONG_SELECT);
            return false;
        }
        //
        // the one byte system messages.
        // these messages may interrupt any other message,
        // and therefore do not affect the current state or
        // running status.
        //
        case TUNE_REQUEST:
        case TIMING_CLOCK:
        case MEASURE_END:
        case START:
        case CONTINUE:
        case STOP:
        case ACTIVE_SENSE: {
            msg->set_status(b);
            return true;
        }
        default: {
            //
            // any other byte must be ignored.
            // It is either a communicatin error or
            // a new type of MIDI message.
            // go into FIND_STATUS state to ignore
            // any possible data bytes for this unknown message
            //
            state = FIND_STATUS;
            return false;
        }
    }
}

void MIDIParser::parse_status_byte(std::uint8_t b)
{
    char len = get_message_length(b);

    if (len == 2) {
        state = FIRST_OF_ONE;
        tmp_msg.set_status(b);
    }

    else if (len == 3) {
        state = FIRST_OF_TWO;
        tmp_msg.set_status(b);
    }

    else {
        state = FIND_STATUS;
        tmp_msg.set_status(0);
    }
}

bool MIDIParser::parse_data_byte(std::uint8_t b, MIDIMessage* msg)
{

    switch (state) {
        case FIND_STATUS: {
            //
            // just eat data bytes until we get a status byte
            //
            return false;
        }
        case FIRST_OF_ONE: {
            //
            // this is the only data byte of a message.
            // form the message and return it.
            //
            tmp_msg.set_byte1(b);
            *msg = tmp_msg;
            //
            // stay in this state for running status
            //
            return true;
        }
        case FIRST_OF_TWO: {
            //
            // this is the first byte of a two byte message.
            // read it in. go to SECOND_OF_TWO state. do not
            // return anything.
            //
            tmp_msg.set_byte1(b);
            state = SECOND_OF_TWO;
            return false;
        }
        case SECOND_OF_TWO: {
            //
            // this is the second byte of a two byte message.
            // read it in. form the message, and return in.
            // go back to FIRST_OF_TWO state to allow
            // running status.
            //
            tmp_msg.set_byte2(b);
            state = FIRST_OF_TWO;
            *msg = tmp_msg;
            return true;
        }
        case FIRST_OF_ONE_NORUN: {
            //
            // Single data byte system message, like MTC.
            // form the message, return it, and go to FIND_STATUS
            // state. Do not allow running status.
            //
            tmp_msg.set_byte1(b);
            state = FIND_STATUS;
            *msg = tmp_msg;
            return true;
        }
        case SYSEX_DATA: {
            //
            // store the byte into the sysex buffer. Stay
            // in this state. Only a status byte can
            // change our state.
            //
            sysex->put_byte(b);
            return false;
        }
        default: {
            //
            // UNKNOWN STATE! go into FIND_STATUS state
            //
            state = FIND_STATUS;
            return false;
        }
    }
}

}  // namespace jdksmidi
