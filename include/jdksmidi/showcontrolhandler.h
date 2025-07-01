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

    virtual bool Dispatch(MIDIShowControlPacket const& p);

    virtual bool Go();
    virtual bool Go(MIDICue const& q_number);
    virtual bool Go(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool Go(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool Stop();
    virtual bool Stop(MIDICue const& q_number);
    virtual bool Stop(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool Stop(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool Resume();
    virtual bool Resume(MIDICue const& q_number);
    virtual bool Resume(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool Resume(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool TimedGo(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff);
    virtual bool TimedGo(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number);
    virtual bool TimedGo(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list);

    virtual bool TimedGo(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list,
        MIDICue const& q_path);

    virtual bool Load(MIDICue const& q_number);
    virtual bool Load(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool Load(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool Set(std::uint32_t ctrl_num, std::uint32_t ctrl_val);

    virtual bool Set(
        std::uint32_t ctrl_num,
        std::uint32_t ctrl_val,
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff);

    virtual bool Fire(std::uint8_t macro_num);
    virtual bool AllOff();
    virtual bool Restore();
    virtual bool Reset();
    virtual bool GoOff();
    virtual bool GoOff(MIDICue const& q_number);
    virtual bool GoOff(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool GoOff(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool GoJam();
    virtual bool GoJam(MIDICue const& q_number);
    virtual bool GoJam(MIDICue const& q_number, MIDICue const& q_list);
    virtual bool GoJam(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path);
    virtual bool StandbyPlus();
    virtual bool StandbyPlus(MIDICue const& q_list);
    virtual bool StandbyMinus();
    virtual bool StandbyMinus(MIDICue const& q_list);
    virtual bool SequencePlus();
    virtual bool SequencePlus(MIDICue const& q_list);
    virtual bool SequenceMinus();
    virtual bool SequenceMinus(MIDICue const& q_list);
    virtual bool StartClock();
    virtual bool StartClock(MIDICue const& q_list);
    virtual bool StopClock();
    virtual bool StopClock(MIDICue const& q_list);
    virtual bool ZeroClock();
    virtual bool ZeroClock(MIDICue const& q_list);
    virtual bool SetClock(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff);
    virtual bool SetClock(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_list);
    virtual bool MTCChaseOn();
    virtual bool MTCChaseOn(MIDICue const& q_list);
    virtual bool MTCChaseOff();
    virtual bool MTCChaseOff(MIDICue const& q_list);
    virtual bool OpenQList(MIDICue const& q_list);
    virtual bool CloseQList(MIDICue const& q_list);
    virtual bool OpenQPath(MIDICue const& q_path);
    virtual bool CloseQPath(MIDICue const& q_path);
};

}  // namespace jdksmidi

#endif
