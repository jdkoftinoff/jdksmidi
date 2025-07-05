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

#ifndef JDKSMIDI_FILESHOW_H
#define JDKSMIDI_FILESHOW_H

#include "jdksmidi/fileread.h"

#include <cstdint>

namespace jdksmidi {

class MIDIFileShow : public MIDIFileEvents
{
  public:
    MIDIFileShow(FILE* out_);
    virtual ~MIDIFileShow();

  protected:
    virtual void show_time(MIDIClockTime time);

    virtual void mf_error(char const*);

    virtual void mf_starttrack(int trk);
    virtual void mf_endtrack(int trk);
    virtual void mf_header(int, int, int);

    //
    // The possible events in a MIDI Files
    //

    virtual void mf_system_mode(MIDITimedMessage const& msg);
    virtual void mf_note_on(MIDITimedMessage const& msg);
    virtual void mf_note_off(MIDITimedMessage const& msg);
    virtual void mf_poly_after(MIDITimedMessage const& msg);
    virtual void mf_bender(MIDITimedMessage const& msg);
    virtual void mf_program(MIDITimedMessage const& msg);
    virtual void mf_chan_after(MIDITimedMessage const& msg);
    virtual void mf_control(MIDITimedMessage const& msg);
    virtual void mf_sysex(MIDIClockTime time, MIDISystemExclusive const& ex);

    virtual void mf_arbitrary(MIDIClockTime time, int len, std::uint8_t* data);
    virtual void mf_metamisc(MIDIClockTime time, int, int, std::uint8_t*);
    virtual void mf_seqnum(MIDIClockTime time, int);
    virtual void mf_smpte(MIDIClockTime time, int, int, int, int, int);
    virtual void mf_timesig(MIDIClockTime time, int, int, int, int);
    virtual void mf_tempo(MIDIClockTime time, std::uint32_t tempo);
    virtual void mf_keysig(MIDIClockTime time, int, int);
    virtual void mf_sqspecific(MIDIClockTime time, int, std::uint8_t*);
    virtual void mf_text(MIDIClockTime time, int, int, std::uint8_t*);
    virtual void mf_eot(MIDIClockTime time);

    FILE* out;
    int division;

  private:
};

}  // namespace jdksmidi
#endif
