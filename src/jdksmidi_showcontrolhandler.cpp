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

#include "jdksmidi/showcontrolhandler.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

MIDISCHandle::MIDISCHandle()
{}

MIDISCHandle::~MIDISCHandle()
{}

bool MIDISCHandle::Dispatch(MIDIShowControlPacket const& p)
{
    bool e;

    switch (p.GetCommand()) {
        case MIDI_SC_GO:

            if (p.GetHasQPath())
                e = Go(p.GetQNumber(), p.GetQList(), p.GetQPath());

            else if (p.GetHasQList())
                e = Go(p.GetQNumber(), p.GetQList());

            else if (p.GetHasQNumber())
                e = Go(p.GetQNumber());

            else
                e = Go();

            break;
        case MIDI_SC_STOP:

            if (p.GetHasQPath())
                e = Stop(p.GetQNumber(), p.GetQList(), p.GetQPath());

            else if (p.GetHasQList())
                e = Stop(p.GetQNumber(), p.GetQList());

            else if (p.GetHasQNumber())
                e = Stop(p.GetQNumber());

            else
                e = Stop();

            break;
        case MIDI_SC_RESUME:

            if (p.GetHasQPath())
                e = Resume(p.GetQNumber(), p.GetQList(), p.GetQPath());

            else if (p.GetHasQList())
                e = Resume(p.GetQNumber(), p.GetQList());

            else if (p.GetHasQNumber())
                e = Resume(p.GetQNumber());

            else
                e = Resume();

            break;
        case MIDI_SC_LOAD:

            if (p.GetHasQPath())
                e = Load(p.GetQNumber(), p.GetQList(), p.GetQPath());

            else if (p.GetHasQList())
                e = Load(p.GetQNumber(), p.GetQList());

            else
                e = Load(p.GetQNumber());

            break;
        case MIDI_SC_GO_OFF:

            if (p.GetHasQPath())
                e = GoOff(p.GetQNumber(), p.GetQList(), p.GetQPath());

            else if (p.GetHasQList())
                e = GoOff(p.GetQNumber(), p.GetQList());

            else if (p.GetHasQNumber())
                e = GoOff(p.GetQNumber());

            else
                e = GoOff();

            break;
        case MIDI_SC_GO_JAM:

            if (p.GetHasQPath())
                e = GoJam(p.GetQNumber(), p.GetQList(), p.GetQPath());

            else if (p.GetHasQList())
                e = GoJam(p.GetQNumber(), p.GetQList());

            else if (p.GetHasQNumber())
                e = GoJam(p.GetQNumber());

            else
                e = GoJam();

            break;
        case MIDI_SC_TIMED_GO:

            if (p.GetHasQPath())
                e = TimedGo(
                    p.GetHours(),
                    p.GetMinutes(),
                    p.GetSeconds(),
                    p.GetFrames(),
                    p.GetFractFrames(),
                    p.GetQNumber(),
                    p.GetQList(),
                    p.GetQPath());

            else if (p.GetHasQList())
                e = TimedGo(
                    p.GetHours(),
                    p.GetMinutes(),
                    p.GetSeconds(),
                    p.GetFrames(),
                    p.GetFractFrames(),
                    p.GetQNumber(),
                    p.GetQList());

            else if (p.GetHasQNumber())
                e = TimedGo(
                    p.GetHours(),
                    p.GetMinutes(),
                    p.GetSeconds(),
                    p.GetFrames(),
                    p.GetFractFrames(),
                    p.GetQNumber());

            else
                e = TimedGo(
                    p.GetHours(),
                    p.GetMinutes(),
                    p.GetSeconds(),
                    p.GetFrames(),
                    p.GetFractFrames());

            break;
        case MIDI_SC_SET:

            if (!p.GetHasTime()) {
                e = Set(p.GetControlNum(), p.GetControlVal());
            }

            else {
                e =
                    Set(p.GetControlNum(),
                        p.GetControlVal(),
                        p.GetHours(),
                        p.GetMinutes(),
                        p.GetSeconds(),
                        p.GetFrames(),
                        p.GetFractFrames());
            }

            break;
        case MIDI_SC_FIRE:
            e = Fire((std::uint8_t)p.GetMacroNum());
            break;
        case MIDI_SC_ALL_OFF:
            e = AllOff();
            break;
        case MIDI_SC_RESTORE:
            e = Restore();
            break;
        case MIDI_SC_RESET:
            e = Reset();
            break;
        case MIDI_SC_STANDBY_PLUS:

            if (p.GetHasQList())
                e = StandbyPlus(p.GetQList());

            else
                e = StandbyPlus();

            break;
        case MIDI_SC_STANDBY_MINUS:

            if (p.GetHasQList())
                e = StandbyMinus(p.GetQList());

            else
                e = StandbyMinus();

            break;
        case MIDI_SC_SEQUENCE_PLUS:

            if (p.GetHasQList())
                e = SequencePlus(p.GetQList());

            else
                e = SequencePlus();

            break;
        case MIDI_SC_SEQUENCE_MINUS:

            if (p.GetHasQList())
                e = SequenceMinus(p.GetQList());

            else
                e = SequenceMinus();

            break;
        case MIDI_SC_START_CLOCK:

            if (p.GetHasQList())
                e = StartClock(p.GetQList());

            else
                e = StartClock();

            break;
        case MIDI_SC_STOP_CLOCK:

            if (p.GetHasQList())
                e = StopClock(p.GetQList());

            else
                e = StopClock();

            break;
        case MIDI_SC_ZERO_CLOCK:

            if (p.GetHasQList())
                e = ZeroClock(p.GetQList());

            else
                e = ZeroClock();

            break;
        case MIDI_SC_MTC_CHASE_ON:

            if (p.GetHasQList())
                e = MTCChaseOn(p.GetQList());

            else
                e = MTCChaseOn();

            break;
        case MIDI_SC_MTC_CHASE_OFF:

            if (p.GetHasQList())
                e = MTCChaseOff(p.GetQList());

            else
                e = MTCChaseOff();

            break;
        case MIDI_SC_SET_CLOCK:

            if (p.GetHasQList())
                e = SetClock(
                    p.GetHours(),
                    p.GetMinutes(),
                    p.GetSeconds(),
                    p.GetFrames(),
                    p.GetFractFrames(),
                    p.GetQList());

            else
                e = SetClock(
                    p.GetHours(),
                    p.GetMinutes(),
                    p.GetSeconds(),
                    p.GetFrames(),
                    p.GetFractFrames());

        case MIDI_SC_OPEN_Q_LIST:
            e = OpenQList(p.GetQList());
            break;
        case MIDI_SC_CLOSE_Q_LIST:
            e = CloseQList(p.GetQList());
            break;
        case MIDI_SC_OPEN_Q_PATH:
            e = OpenQPath(p.GetQPath());
            break;
        case MIDI_SC_CLOSE_Q_PATH:
            e = CloseQPath(p.GetQPath());
            break;
        default:
            e = false;
            break;
    }

    return e;
}

bool MIDISCHandle::Go()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Go(MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Go(MIDICue const& q_number, MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Go(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Stop()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Stop(MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Stop(MIDICue const& q_number, MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Stop(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Resume()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Resume(MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Resume(MIDICue const& q_number, MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Resume(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::TimedGo(
    std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::TimedGo(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::TimedGo(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_number,
    MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::TimedGo(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_number,
    MIDICue const& q_list,
    MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Load(MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Load(MIDICue const& q_number, MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Load(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Set(std::uint32_t ctrl_num, std::uint32_t ctrl_val)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Set(
    std::uint32_t ctrl_num,
    std::uint32_t ctrl_val,
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Fire(std::uint8_t macro_num)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::AllOff()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Restore()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::Reset()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoOff()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoOff(MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoOff(MIDICue const& q_number, MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoOff(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoJam()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoJam(MIDICue const& q_number)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoJam(MIDICue const& q_number, MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::GoJam(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StandbyPlus()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StandbyPlus(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StandbyMinus()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StandbyMinus(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::SequencePlus()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::SequencePlus(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::SequenceMinus()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::SequenceMinus(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StartClock()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StartClock(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StopClock()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::StopClock(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::ZeroClock()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::ZeroClock(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::SetClock(
    std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::SetClock(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::MTCChaseOn()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::MTCChaseOn(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::MTCChaseOff()
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::MTCChaseOff(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::OpenQList(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::CloseQList(MIDICue const& q_list)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::OpenQPath(MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

bool MIDISCHandle::CloseQPath(MIDICue const& q_path)
{
    bool e;
    e = false;
    return e;
}

}  // namespace jdksmidi
