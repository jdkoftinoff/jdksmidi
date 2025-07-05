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

#include <cstdint>

#ifdef WIN32
#    include "jdksmidi/driverwin32.h"

namespace jdksmidi {

MIDISequencerGUIEventNotifierWin32::MIDISequencerGUIEventNotifierWin32(
    HWND w, DWORD msg, WPARAM wparam_value_)
    : dest_window(w)
    , window_msg(msg)
    , wparam_value(wparam_value_)
    , en(true)
{}

MIDISequencerGUIEventNotifierWin32::~MIDISequencerGUIEventNotifierWin32()
{}

void MIDISequencerGUIEventNotifierWin32::Notify(MIDISequencer const* seq, MIDISequencerGUIEvent e)
{
    if (en) {
        PostMessage(dest_window, window_msg, wparam_value, static_cast<unsigned long> e);
    }
}

bool MIDISequencerGUIEventNotifierWin32::GetEnable() const
{
    return en;
}

void MIDISequencerGUIEventNotifierWin32::SetEnable(bool f)
{
    en = f;
}

MIDIDriverWin32::MIDIDriverWin32(int queue_size)
    : MIDIDriver(queue_size)
    , in_handle(0)
    , out_handle(0)
    , in_open(false)
    , out_open(false)
    , timer_open(false)
{}

MIDIDriverWin32::~MIDIDriverWin32()
{
    StopTimer();
    CloseMIDIInPort();
    CloseMIDIOutPort();
}

void MIDIDriverWin32::ResetMIDIOut()
{
    if (out_open) {
        midiOutReset(out_handle);
    }
}

bool MIDIDriverWin32::StartTimer(int res)
{
    if (!timer_open) {
        TIMECAPS tc;

        if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
            return false;
        }

        timer_res = res;

        if (timer_res < static_cast<int>(tc.wPeriodMin))
            timer_res = static_cast<int>(tc.wPeriodMin);

        if (timer_res > static_cast<int>(tc.wPeriodMax))
            timer_res = static_cast<int>(tc.wPeriodMax);

        timeBeginPeriod(timer_res);
        timer_id =
            timeSetEvent(res, res, win32_timer, reinterpret_cast<DWORD>(this), TIME_PERIODIC);

        if (timer_id) {
            timer_open = true;
        }
    }

    return true;
}

void MIDIDriverWin32::StopTimer()
{
    if (timer_open) {
        timeKillEvent(timer_id);
        timeEndPeriod(timer_res);
        timer_open = false;
    }
}

bool MIDIDriverWin32::OpenMIDIInPort(int id)
{
    if (!in_open) {
        if (midiInOpen(
                &in_handle,
                id,
                reinterpret_cast<DWORD> win32_midi_in,
                reinterpret_cast<DWORD>(this),
                CALLBACK_FUNCTION) != 0) {
            return false;
        }

        midiInStart(in_handle);
        in_open = true;
    }

    return true;
}

bool MIDIDriverWin32::OpenMIDIOutPort(int id)
{
    if (!out_open) {
        int e = midiOutOpen(&out_handle, id, 0, 0, CALLBACK_NULL);

        if (e != 0) {
            return false;
        }

        out_open = true;
    }

    return true;
}

void MIDIDriverWin32::CloseMIDIInPort()
{
    if (in_open) {
        midiInStop(in_handle);
        midiInClose(in_handle);
        in_open = false;
    }
}

void MIDIDriverWin32::CloseMIDIOutPort()
{
    if (out_open) {
        midiOutClose(out_handle);
        out_open = false;
        Reset();
    }
}

bool MIDIDriverWin32::HardwareMsgOut(MIDITimedBigMessage const& msg)
{
    if (out_open) {
        // dont send sysex or meta-events
        if (msg.GetStatus() < 0xff && msg.GetStatus() != 0xf0) {
            DWORD winmsg;
            winmsg = ((reinterpret_cast<DWORD> msg.GetStatus() & 0xff) << 0) |
                ((reinterpret_cast<DWORD> msg.GetByte1() & 0xff) << 8) |
                ((reinterpret_cast<DWORD> msg.GetByte2() & 0xff) << 16);

            if (midiOutShortMsg(out_handle, winmsg) != 0) {
                return false;
            }
        }

        return true;
    }

    return false;
}

void CALLBACK
MIDIDriverWin32::win32_timer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    MIDIDriverWin32* self = reinterpret_cast<MIDIDriverWin32*>(dwUser);
    self->TimeTick(timeGetTime());
}

void CALLBACK MIDIDriverWin32::win32_midi_in(
    HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    MIDIDriverWin32* self = reinterpret_cast<MIDIDriverWin32*>(dwInstance);

    if (wMsg == MIM_DATA) {
        MIDITimedBigMessage msg;
        msg.SetStatus(static_cast<std::uint8_t>(dwParam1 & 0xff));
        msg.SetByte1(static_cast<std::uint8_t>((dwParam1 >> 8) & 0xff));
        msg.SetByte2(static_cast<std::uint8_t>((dwParam1 >> 16) & 0xff));
        msg.SetTime(timeGetTime());
        self->HardwareMsgIn(msg);
    }
}

}  // namespace jdksmidi
#endif
