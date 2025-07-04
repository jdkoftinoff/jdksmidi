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

#ifndef JDKSMIDI_MATRIX_H
#define JDKSMIDI_MATRIX_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"

#include <cstdint>

namespace jdksmidi {

class MIDIMatrix
{
  public:
    MIDIMatrix();
    virtual ~MIDIMatrix();

    virtual bool process(MIDIMessage const& m);

    virtual void clear();

    int get_total_count() const { return _total_count; }
    int get_channel_count(int channel) const { return _channel_count[channel]; }

    int get_note_count(int channel, int note) const { return _note_on_count[channel][note]; }

    bool get_hold_pedal(int channel) const { return _hold_pedal[channel]; }

  protected:
    virtual void dec_note_count(MIDIMessage const& m, int channel, int note);
    virtual void inc_note_count(MIDIMessage const& m, int channel, int note);
    virtual void clear_channel(int channel);
    virtual void other_message(MIDIMessage const& m);

    void set_note_count(std::uint8_t chan, std::uint8_t note, std::uint8_t val)
    {
        _note_on_count[chan][note] = val;
    }
    void set_channel_count(std::uint8_t chan, int val) { _channel_count[chan] = val; }

  private:
    std::uint8_t _note_on_count[16][128];
    int _channel_count[16];
    bool _hold_pedal[16];
    int _total_count;
};

}  // namespace jdksmidi

#endif
