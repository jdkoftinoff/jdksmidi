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
// Modified by N. Cassetta ncassetta@tiscali.it
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

///
/// This class inherits from pure virtual MIDISequencerGUIEventNotifier and it is a specialization of the class
/// for dealing with WIN32 environment.
/// It communicates with the GUI via the WIN32 function PostMessage(). The main thread WindowProc() can
/// examine these messages and upgrade the GUI accordingly
///

class MIDISequencerGUIEventNotifierWin32 :
    public MIDISequencerGUIEventNotifier
{

public:

    /// The constructor.
    /// \param w the windows handle to which send messages
    /// \param wmmsg the id of the message (must be a valid Windows id
    /// \param wparam_value optional parameter: currently unused
    MIDISequencerGUIEventNotifierWin32 (
        HWND w,
        DWORD wmmsg,
        WPARAM wparam_value_ = 0
    );

    /* NEW BY NC = auto sets msg and wparam_value */
    /// Other constructor.
    /// Ad above, but needs only the windows handle and auto gets from Windows a valid message id ( you can retrieve
    /// it by GetMsgId() and monitor it in your main WindowProc() )
    MIDISequencerGUIEventNotifierWin32 (
        HWND w
    );

    /// The destructor
    virtual ~MIDISequencerGUIEventNotifierWin32()
    {
    }

    /* NEW BY NC */
    /// Returns the Windows id of sent messages. It is the _message_ parameter in _w_ WindowProcedure and can be
    /// monitored to upgrade the GUI
    DWORD GetMsgId() const
    {
        return window_msg;
    }

    /// Sends the MIDISequencerGUIEvent _e_ to the window
    virtual void Notify ( const MIDISequencer *seq, MIDISequencerGUIEvent e );

    /// Returns the on/off status
    virtual bool GetEnable() const
    {
        return en;
    }

    /// Sets the on/off status
    virtual void SetEnable ( bool f )
    {
        en = f;
    }

private:

    HWND dest_window;
    DWORD window_msg;
    WPARAM wparam_value;
    bool en;
};


///
/// This class inherits from pure virtual MIDIDriver and it is a specialization of the class
/// for dealing with WIN32 environment.
/// It sends and receives MIDI messages from the hardware MIDI ports; moreover, it enumerates
/// MIDI ports and keep tracks of their names in a static array.
/// Currently, only one MIDI in and MIDI out ports can be opened (no multiport allowed)
/// \warning currently there are these limitations:
/// - multiple instances of this class are NOT safe: one driver could close a port used by another
/// - sysex messages are not sent
///
// TODO: fix the warning, implement a static control of open and closed ports. This is not simple
// as it could seem, caused to the Windows MIDI_MAPPER seen as port -1
// TODO: sens sysex

class MIDIDriverWin32 : public MIDIDriver
{

public:

    MIDIDriverWin32 ( int queue_size = DEFAULT_QUEUE_SIZE );
    virtual ~MIDIDriverWin32();

    /// Opens the MIDI in port _id_
    bool OpenMIDIInPort ( int id );

    /// Opens the MIDI out port _id_
    bool OpenMIDIOutPort ( int id );

    /// Closes the open MIDI in port
    void CloseMIDIInPort();

    /// Closes the open MIDI out port
    void CloseMIDIOutPort();

    /// Resets open MIDI out port
    void ResetMIDIOut();

    /// Start the hardware timer for playing MIDI. Default time resolution is 1 ms
    bool StartTimer ( int resolution_ms = DEFAULT_TIMER_RESOLUTION );

    /// Stops the hardware timer
    void StopTimer();

    /// Sends the MIDITimedBigMessage _msg_ to the open MIDI out port
    bool HardwareMsgOut ( const MIDITimedBigMessage &msg );

/* NEW BY NC: now the driver keep track statically of the MIDI devices installed on the computer
 * these functions get them
 */
    /// Gets the nunber of MIDI in ports present on the computer
    static unsigned int GetNumMIDIInDevs()
    {
        return num_in_devs;
    }

    /// Gets the number of MIDI out ports present on the computer
    static unsigned int GetNumMIDIOutDevs()
    {
        return num_out_devs;
    }

    /// Gets the name of the MIDI in port _i_
    static const char* GetMIDIInDevName(unsigned int i)
    {
        return in_dev_names[i];
    }

    /// Gets the name of the MIDI out port _i_
    static const char* GetMIDIOutDevName(unsigned int i)
    {
        return out_dev_names[i];
    }

    static const int DEFAULT_TIMER_RESOLUTION = 1;  // public: used by AdvancedSequencer
/* END OF NEW */

protected:

    static const int DEFAULT_QUEUE_SIZE = 256;  /* NEW BY NC */

    /// The callback function called to every timer tick when playing
    static void CALLBACK win32_timer (
        UINT wTimerID,
        UINT msg,
        DWORD dwUser,
        DWORD dw1,
        DWORD dw2
    );

    /// The callback function called for MIDI in
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
    static const int DEVICENAME_LEN = 80;

    static char** in_dev_names;
    static char** out_dev_names;
    static UINT num_in_devs;
    static UINT num_out_devs;

};


}
#endif

#endif
