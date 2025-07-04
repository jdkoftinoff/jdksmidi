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
#ifndef JDKSMIDI_DRIVERWIN32_H
#define JDKSMIDI_DRIVERWIN32_H

#include "jdksmidi/driver.h"
#include "jdksmidi/sequencer.h"

#ifdef WIN32
#    include "mmsystem.h"
#    include "windows.h"

namespace jdksmidi {

class MIDISequencerGUIEventNotifierWin32 : public MIDISequencerGUIEventNotifier
{
  public:
    MIDISequencerGUIEventNotifierWin32(HWND w, DWORD wmmsg, WPARAM wparam_value_ = 0);

    virtual ~MIDISequencerGUIEventNotifierWin32();

    virtual void notify(MIDISequencer const* seq, MIDISequencerGUIEvent e);
    virtual bool get_enable() const;
    virtual void set_enable(bool f);

  private:
    HWND dest_window;
    DWORD window_msg;
    WPARAM wparam_value;
    bool en;
};

class MIDIDriverWin32 : public MIDIDriver
{
  public:
    MIDIDriverWin32(int queue_size);
    virtual ~MIDIDriverWin32();

    void reset_midi_out();

    bool start_timer(int resolution_ms);
    bool open_midi_in_port(int id);
    bool open_midi_out_port(int id);

    void stop_timer();
    void close_midi_in_port();
    void close_midi_out_port();

    bool hardware_msg_out(MIDITimedBigMessage const& msg);

  protected:
    static void CALLBACK win32_timer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

    static void CALLBACK
    win32_midi_in(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

    HMIDIIN in_handle;
    HMIDIOUT out_handle;
    int timer_id;
    int timer_res;

    bool in_open;
    bool out_open;
    bool timer_open;
};

}  // namespace jdksmidi
#endif

#endif
