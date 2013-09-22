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

//
// Modified by N. Cassetta
//

#ifndef JDKSMIDI_DRIVERWIN32_H
#define JDKSMIDI_DRIVERWIN32_H

#include "jdksmidi/driver.h"
#include "jdksmidi/sequencer.h"

#ifdef WIN32
#include "windows.h"
#include "mmsystem.h"

namespace jdksmidi
{

unsigned int jdks_get_safe_system_msg_id();

inline unsigned long jdks_get_system_time_ms()
{
    return timeGetTime();
}

inline void jdks_wait( unsigned int ms )
{
    Sleep( ms );
}


class MIDISequencerGUIEventNotifierWin32 :
    public MIDISequencerGUIEventNotifier
{

public:

    MIDISequencerGUIEventNotifierWin32 (
        HWND w,
        DWORD wmmsg,
        WPARAM wparam_value_ = 0
    );

    /* NEW BY NC = auto sets msg and wparam_value */
    MIDISequencerGUIEventNotifierWin32 (
        HWND w
    );

    virtual ~MIDISequencerGUIEventNotifierWin32();

    /* NEW BY NC */
    DWORD GetMsgId() const;

    virtual void Notify ( const MIDISequencer *seq, MIDISequencerGUIEvent e );
    virtual bool GetEnable() const;
    virtual void SetEnable ( bool f );

private:

    HWND dest_window;
    DWORD window_msg;
    WPARAM wparam_value;
    bool en;
};



class MIDIDriverWin32 : public MIDIDriver
{

public:

    MIDIDriverWin32 ( int queue_size = DEFAULT_QUEUE_SIZE );
    virtual ~MIDIDriverWin32();

    void ResetMIDIOut();

    bool StartTimer ( int resolution_ms );
    bool OpenMIDIInPort ( int id );
    bool OpenMIDIOutPort ( int id );

    void StopTimer();
    void CloseMIDIInPort();
    void CloseMIDIOutPort();

    bool HardwareMsgOut ( const MIDITimedBigMessage &msg );

/* NEW BY NC: now the driver keep track statically of the MIDI devices installed on the computer
 * these functions get them
 */
    static unsigned int GetNumMIDIInDevs();
    static unsigned int GetNumMIDIOutDevs();
    static const char* GetMIDIInDevName(unsigned int i);
    static const char* GetMIDIOutDevName(unsigned int i);
/* END OF NEW */

protected:

    static const int DEFAULT_QUEUE_SIZE = 256;  /* NEW BY NC */

    static void CALLBACK win32_timer (
        UINT wTimerID,
        UINT msg,
        DWORD dwUser,
        DWORD dw1,
        DWORD dw2
    );

    static void CALLBACK win32_midi_in (
        HMIDIIN hMidiIn,
        UINT wMsg,
        DWORD dwInstance,
        DWORD dwParam1,
        DWORD dwParam2
    );

    static unsigned int FillMIDIInDevices();        /* NEW BY NC */
    static unsigned int FillMIDIOutDevices();       /* NEW BY NC */

    HMIDIIN in_handle;
    HMIDIOUT out_handle;
    int timer_id;
    int timer_res;

    bool in_open;
    bool out_open;
    bool timer_open;

/* NEW BY NC
 * these keep track of the MIDI devices present in the OS
 */
    static const int DEVICENAMELEN = 80;

    static char** in_dev_names;
    static char** out_dev_names;
    static UINT num_in_devs;
    static UINT num_out_devs;

};


}
#endif

#endif
