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

#include "jdksmidi/showcontrol.h"
#include "jdksmidi/showcontrolhandler.h"

#include <cstdint>

namespace jdksmidi {

MIDISCHandle::MIDISCHandle() = default;

MIDISCHandle::~MIDISCHandle() = default;

bool MIDISCHandle::dispatch(MIDIShowControlPacket const& p)
{
    bool e;

    switch (p.get_command()) {
        case MIDI_SC_GO:

            if (p.get_has_q_path())
                e = go(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = go(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = go(p.get_q_number());

            else
                e = go();

            break;
        case MIDI_SC_STOP:

            if (p.get_has_q_path())
                e = stop(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = stop(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = stop(p.get_q_number());

            else
                e = stop();

            break;
        case MIDI_SC_RESUME:

            if (p.get_has_q_path())
                e = resume(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = resume(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = resume(p.get_q_number());

            else
                e = resume();

            break;
        case MIDI_SC_LOAD:

            if (p.get_has_q_path())
                e = load(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = load(p.get_q_number(), p.get_q_list());

            else
                e = load(p.get_q_number());

            break;
        case MIDI_SC_GO_OFF:

            if (p.get_has_q_path())
                e = go_off(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = go_off(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = go_off(p.get_q_number());

            else
                e = go_off();

            break;
        case MIDI_SC_GO_JAM:

            if (p.get_has_q_path())
                e = go_jam(p.get_q_number(), p.get_q_list(), p.get_q_path());

            else if (p.get_has_q_list())
                e = go_jam(p.get_q_number(), p.get_q_list());

            else if (p.get_has_q_number())
                e = go_jam(p.get_q_number());

            else
                e = go_jam();

            break;
        case MIDI_SC_TIMED_GO:

            if (p.get_has_q_path())
                e = timed_go(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_number(),
                    p.get_q_list(),
                    p.get_q_path());

            else if (p.get_has_q_list())
                e = timed_go(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_number(),
                    p.get_q_list());

            else if (p.get_has_q_number())
                e = timed_go(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_number());

            else
                e = timed_go(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames());

            break;
        case MIDI_SC_SET:

            if (!p.get_has_time()) {
                e = set(p.get_control_num(), p.get_control_val());
            }

            else {
                e =
                    set(p.get_control_num(),
                        p.get_control_val(),
                        p.get_hours(),
                        p.get_minutes(),
                        p.get_seconds(),
                        p.get_frames(),
                        p.get_fract_frames());
            }

            break;
        case MIDI_SC_FIRE:
            e = fire((std::uint8_t)p.get_macro_num());
            break;
        case MIDI_SC_ALL_OFF:
            e = all_off();
            break;
        case MIDI_SC_RESTORE:
            e = restore();
            break;
        case MIDI_SC_RESET:
            e = reset();
            break;
        case MIDI_SC_STANDBY_PLUS:

            if (p.get_has_q_list())
                e = standby_plus(p.get_q_list());

            else
                e = standby_plus();

            break;
        case MIDI_SC_STANDBY_MINUS:

            if (p.get_has_q_list())
                e = standby_minus(p.get_q_list());

            else
                e = standby_minus();

            break;
        case MIDI_SC_SEQUENCE_PLUS:

            if (p.get_has_q_list())
                e = sequence_plus(p.get_q_list());

            else
                e = sequence_plus();

            break;
        case MIDI_SC_SEQUENCE_MINUS:

            if (p.get_has_q_list())
                e = sequence_minus(p.get_q_list());

            else
                e = sequence_minus();

            break;
        case MIDI_SC_START_CLOCK:

            if (p.get_has_q_list())
                e = start_clock(p.get_q_list());

            else
                e = start_clock();

            break;
        case MIDI_SC_STOP_CLOCK:

            if (p.get_has_q_list())
                e = stop_clock(p.get_q_list());

            else
                e = stop_clock();

            break;
        case MIDI_SC_ZERO_CLOCK:

            if (p.get_has_q_list())
                e = zero_clock(p.get_q_list());

            else
                e = zero_clock();

            break;
        case MIDI_SC_MTC_CHASE_ON:

            if (p.get_has_q_list())
                e = mtc_chase_on(p.get_q_list());

            else
                e = mtc_chase_on();

            break;
        case MIDI_SC_MTC_CHASE_OFF:

            if (p.get_has_q_list())
                e = mtc_chase_off(p.get_q_list());

            else
                e = mtc_chase_off();

            break;
        case MIDI_SC_SET_CLOCK:

            if (p.get_has_q_list())
                e = set_clock(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames(),
                    p.get_q_list());

            else
                e = set_clock(
                    p.get_hours(),
                    p.get_minutes(),
                    p.get_seconds(),
                    p.get_frames(),
                    p.get_fract_frames());
            break;
        case MIDI_SC_OPEN_Q_LIST:
            e = open_q_list(p.get_q_list());
            break;
        case MIDI_SC_CLOSE_Q_LIST:
            e = close_q_list(p.get_q_list());
            break;
        case MIDI_SC_OPEN_Q_PATH:
            e = open_q_path(p.get_q_path());
            break;
        case MIDI_SC_CLOSE_Q_PATH:
            e = close_q_path(p.get_q_path());
            break;
        default:
            e = false;
            break;
    }

    return e;
}

bool MIDISCHandle::go()
{
    return false;
}

bool MIDISCHandle::go(MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::go(MIDICue const& q_number, MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::go(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::stop()
{
    return false;
}

bool MIDISCHandle::stop(MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::stop(MIDICue const& q_number, MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::stop(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::resume()
{
    return false;
}

bool MIDISCHandle::resume(MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::resume(MIDICue const& q_number, MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::resume(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::timed_go(
    std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
{
    return false;
}

bool MIDISCHandle::timed_go(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::timed_go(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_number,
    MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::timed_go(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_number,
    MIDICue const& q_list,
    MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::load(MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::load(MIDICue const& q_number, MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::load(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::set(std::uint32_t ctrl_num, std::uint32_t ctrl_val)
{
    return false;
}

bool MIDISCHandle::set(
    std::uint32_t ctrl_num,
    std::uint32_t ctrl_val,
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff)
{
    return false;
}

bool MIDISCHandle::fire(std::uint8_t macro_num)
{
    return false;
}

bool MIDISCHandle::all_off()
{
    return false;
}

bool MIDISCHandle::restore()
{
    return false;
}

bool MIDISCHandle::reset()
{
    return false;
}

bool MIDISCHandle::go_off()
{
    return false;
}

bool MIDISCHandle::go_off(MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::go_off(MIDICue const& q_number, MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::go_off(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::go_jam()
{
    return false;
}

bool MIDISCHandle::go_jam(MIDICue const& q_number)
{
    return false;
}

bool MIDISCHandle::go_jam(MIDICue const& q_number, MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::go_jam(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::standby_plus()
{
    return false;
}

bool MIDISCHandle::standby_plus(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::standby_minus()
{
    return false;
}

bool MIDISCHandle::standby_minus(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::sequence_plus()
{
    return false;
}

bool MIDISCHandle::sequence_plus(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::sequence_minus()
{
    return false;
}

bool MIDISCHandle::sequence_minus(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::start_clock()
{
    return false;
}

bool MIDISCHandle::start_clock(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::stop_clock()
{
    return false;
}

bool MIDISCHandle::stop_clock(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::zero_clock()
{
    return false;
}

bool MIDISCHandle::zero_clock(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::set_clock(
    std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
{
    return false;
}

bool MIDISCHandle::set_clock(
    std::uint8_t hr,
    std::uint8_t mn,
    std::uint8_t sc,
    std::uint8_t fr,
    std::uint8_t ff,
    MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::mtc_chase_on()
{
    return false;
}

bool MIDISCHandle::mtc_chase_on(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::mtc_chase_off()
{
    return false;
}

bool MIDISCHandle::mtc_chase_off(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::open_q_list(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::close_q_list(MIDICue const& q_list)
{
    return false;
}

bool MIDISCHandle::open_q_path(MIDICue const& q_path)
{
    return false;
}

bool MIDISCHandle::close_q_path(MIDICue const& q_path)
{
    return false;
}

}  // namespace jdksmidi
