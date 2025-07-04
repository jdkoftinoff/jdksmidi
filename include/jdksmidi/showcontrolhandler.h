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

#ifndef JDKSMIDI_SHOWCONTROLHANDLER_H
#define JDKSMIDI_SHOWCONTROLHANDLER_H

#include "jdksmidi/showcontrol.h"

#include <cstdint>

namespace jdksmidi {

class MIDISCHandle
{
  public:
    MIDISCHandle();
    virtual ~MIDISCHandle();

    virtual bool dispatch(MIDIShowControlPacket const& p);

    virtual bool go();
    virtual bool go(MIDICue const& q_number);
    virtual bool go(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool go(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool stop();
    virtual bool stop(MIDICue const& q_number);
    virtual bool stop(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool stop(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool resume();
    virtual bool resume(MIDICue const& q_number);
    virtual bool resume(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool resume(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool timed_go(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff);
    virtual bool timed_go(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number);
    virtual bool timed_go(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list);

    virtual bool timed_go(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list,
        MIDICue const& q_path);

    virtual bool load(MIDICue const& q_number);
    virtual bool load(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool load(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool set(std::uint32_t ctrl_num, std::uint32_t ctrl_val);

    virtual bool set(
        std::uint32_t ctrl_num,
        std::uint32_t ctrl_val,
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff);

    virtual bool fire(std::uint8_t macro_num);
    virtual bool all_off();
    virtual bool restore();
    virtual bool reset();
    virtual bool go_off();
    virtual bool go_off(MIDICue const& q_number);
    virtual bool go_off(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool go_off(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool go_jam();
    virtual bool go_jam(MIDICue const& q_number);
    virtual bool go_jam(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool go_jam(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool standby_plus();
    virtual bool standby_plus(MIDICue const& q_list);
    virtual bool standby_minus();
    virtual bool standby_minus(MIDICue const& q_list);
    virtual bool sequence_plus();
    virtual bool sequence_plus(MIDICue const& q_list);
    virtual bool sequence_minus();
    virtual bool sequence_minus(MIDICue const& q_list);
    virtual bool start_clock();
    virtual bool start_clock(MIDICue const& q_list);
    virtual bool stop_clock();
    virtual bool stop_clock(MIDICue const& q_list);
    virtual bool zero_clock();
    virtual bool zero_clock(MIDICue const& q_list);
    virtual bool set_clock(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff);
    virtual bool set_clock(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_list);
    virtual bool mtc_chase_on();
    virtual bool mtc_chase_on(MIDICue const& q_list);
    virtual bool mtc_chase_off();
    virtual bool mtc_chase_off(MIDICue const& q_list);
    virtual bool open_q_list(MIDICue const& q_list);
    virtual bool close_q_list(MIDICue const& q_list);
    virtual bool open_q_path(MIDICue const& q_path);
    virtual bool close_q_path(MIDICue const& q_path);
};

}  // namespace jdksmidi

#endif
