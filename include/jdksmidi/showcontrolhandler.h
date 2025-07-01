/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
