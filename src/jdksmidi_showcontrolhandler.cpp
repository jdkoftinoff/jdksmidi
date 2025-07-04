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

    switch (p.get_command()) {
        case MIDI_SC_GO:

            if (p.get_has_q_path())
                e = Go(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = Go(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = Go(p.get_q_number());

            else
                e = Go();

            break;
        case MIDI_SC_STOP:

            if (p.get_has_q_path())
                e = Stop(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = Stop(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = Stop(p.get_q_number());

            else
                e = Stop();

            break;
        case MIDI_SC_RESUME:

            if (p.get_has_q_path())
                e = Resume(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = Resume(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = Resume(p.get_q_number());

            else
                e = Resume();

            break;
        case MIDI_SC_LOAD:

            if (p.get_has_q_path())
                e = Load(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = Load(p.get_q_number(), p.get_q_list());

            else
                e = Load(p.get_q_number());

            break;
        case MIDI_SC_GO_OFF:

            if (p.get_has_q_path())
                e = GoOff(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = GoOff(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = GoOff(p.get_q_number());

            else
                e = GoOff();

            break;
        case MIDI_SC_GO_JAM:

            if (p.get_has_q_path())
                e = GoJam(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = GoJam(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = GoJam(p.get_q_number());

            else
                e = GoJam();

            break;
        case MIDI_SC_TIMED_GO:

            if (p.get_has_q_path())
                e = TimedGo(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_number(),
                    p.get_q_list(),
                    p.get_q_path());

            else if (p.get_has_q_list())
                e = TimedGo(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_number(),
                    p.get_q_list());

            else if (p.get_has_q_number())
                e = TimedGo(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_number());

            else
                e = TimedGo(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames());

            break;
        case MIDI_SC_SET:

            if (!p.get_has_time()) {
                e = Set(p.get_control_num(), p.get_control_val());
            }

            else {
                e =
                    Set(p.get_control_num(),
                        p.get_control_val(),
                        p.get_hours(),
                        p.get_minutes(),
                        p.get_seconds(),
                        p.get_frames(),
                        p.get_fract_frames());
            }

            break;
        case MIDI_SC_FIRE:
            e = Fire((std::uint8_t)p.get_macro_num());
            break;
        case MIDI_SC_ALL_OFF:
            e = AllOff();
            break;
        case MIDI_SC_RESTORE:
            e = Restore();
            break;
        case MIDI_SC_RESET:
            e = reset();
            break;
        case MIDI_SC_STANDBY_PLUS:

            if (p.get_has_q_list())
                e = StandbyPlus(p.get_q_list());

            else
                e = StandbyPlus();

            break;
        case MIDI_SC_STANDBY_MINUS:

            if (p.get_has_q_list())
                e = StandbyMinus(p.get_q_list());

            else
                e = StandbyMinus();

            break;
        case MIDI_SC_SEQUENCE_PLUS:

            if (p.get_has_q_list())
                e = SequencePlus(p.get_q_list());

            else
                e = SequencePlus();

            break;
        case MIDI_SC_SEQUENCE_MINUS:

            if (p.get_has_q_list())
                e = SequenceMinus(p.get_q_list());

            else
                e = SequenceMinus();

            break;
        case MIDI_SC_START_CLOCK:

            if (p.get_has_q_list())
                e = StartClock(p.get_q_list());

            else
                e = StartClock();

            break;
        case MIDI_SC_STOP_CLOCK:

            if (p.get_has_q_list())
                e = StopClock(p.get_q_list());

            else
                e = StopClock();

            break;
        case MIDI_SC_ZERO_CLOCK:

            if (p.get_has_q_list())
                e = ZeroClock(p.get_q_list());

            else
                e = ZeroClock();

            break;
        case MIDI_SC_MTC_CHASE_ON:

            if (p.get_has_q_list())
                e = MTCChaseOn(p.get_q_list());

            else
                e = MTCChaseOn();

            break;
        case MIDI_SC_MTC_CHASE_OFF:

            if (p.get_has_q_list())
                e = MTCChaseOff(p.get_q_list());

            else
                e = MTCChaseOff();

            break;
        case MIDI_SC_SET_CLOCK:

            if (p.get_has_q_list())
                e = SetClock(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_list());

            else
                e = SetClock(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames());

        case MIDI_SC_OPEN_Q_LIST:
            e = OpenQList(p.get_q_list());
            break;
        case MIDI_SC_CLOSE_Q_LIST:
            e = CloseQList(p.get_q_list());
            break;
        case MIDI_SC_OPEN_Q_PATH:
            e = OpenQPath(p.get_q_path());
            break;
        case MIDI_SC_CLOSE_Q_PATH:
            e = CloseQPath(p.get_q_path());
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

bool MIDISCHandle::reset()
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
