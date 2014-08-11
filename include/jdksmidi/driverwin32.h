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

#if defined _WIN32 || defined WIN32
#include <windows.h>
#include <mmsystem.h>

namespace jdksmidi
{

///
/// This class inherits from pure virtual MIDISequencerGUIEventNotifier and it is a specialization of the class
/// for dealing with WIN32 environment.
/// It communicates with the GUI via the WIN32 function PostMessage(). The main thread WindowProc() can
/// examine these messages and upgrade the GUI accordingly
///

class MIDISequencerGUIEventNotifierWin32 : public MIDISequencerGUIEventNotifier
{

  public:

    /// The first form of the constructor.
    /// \param w the windows handle to which send messages
    /// \param wmmsg the id of the message (must be a valid Windows id)
    /// \param wparam_value_ optional parameter: currently unused
    MIDISequencerGUIEventNotifierWin32( HWND w, DWORD wmmsg, WPARAM wparam_value_ = 0 );

    /* NEW BY NC = auto sets msg and wparam_value */
    /// The second form of the constructor only needs the windows handle and auto gets from Windows a valid
    /// message id. You can retrieve the message id by GetMsgId() and monitor it in your main WindowProc() )
    MIDISequencerGUIEventNotifierWin32( HWND w );

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
    } /* END OF NEW */

    /// Sends the MIDISequencerGUIEvent _e_ to the window
    virtual void Notify( const MIDISequencer *seq, MIDISequencerGUIEvent e );

    /// Returns the on/off status
    virtual bool GetEnable() const
    {
        return en;
    }

    /// Sets the on/off status
    virtual void SetEnable( bool f )
    {
        en = f;
    }

  private:

    /* NEW BY NC */
    // returns a safe windows message id, so we can create the notifier without worrying about this
    static UINT GetSafeSystemMsgId()
    {
        static UINT base = WM_APP;
        return base++;
    } /* END OF NEW */

    HWND dest_window;
    DWORD window_msg;
    WPARAM wparam_value;
    bool en;
};

///
/// This class inherits from pure virtual MIDIDriver and is a specialization of the class
/// for dealing with WIN32 environment.
/// It sends and receives MIDI messages from the hardware MIDI ports; moreover, it enumerates
/// MIDI ports and keep track of their names in a static array.
/// Currently, only one MIDI in and MIDI out ports can be opened (no multiport allowed)
/// \warning currently multiple instances of this class are NOT safe: one driver could close a port
/// used by another.
///
// TODO: fix the warning, implement a static control of open and closed ports. This is not simple
// as it could seem, caused to the Windows MIDI_MAPPER seen as port -1

class MIDIDriverWin32 : public MIDIDriver
{

  public:

    /// The constructor. The queue size should be adeguate, you can increase if you get missing events.
    MIDIDriverWin32( int queue_size = DEFAULT_QUEUE_SIZE );

    /// The destructor
    virtual ~MIDIDriverWin32();

    // These are the implementations of pure virtual functions in base class

    /*
        /// Opens the MIDI in port _id_
        virtual bool OpenMIDIInPort ( int id = DEFAULT_IN_PORT );

        /// Opens the MIDI out port _id_
        virtual bool OpenMIDIOutPort ( int id = DEFAULT_OUT_PORT );

        /// Closes the open MIDI in port
        virtual void CloseMIDIInPort();

        /// Closes the open MIDI out port
        virtual void CloseMIDIOutPort();
    */

    /// Resets open MIDI out port
    virtual void ResetMIDIOut();

    /// Start the hardware timer for playing MIDI. Default time resolution is 1 ms
    virtual bool StartTimer( int resolution_ms = DEFAULT_TIMER_RESOLUTION );

    /// Stops the hardware timer
    virtual void StopTimer();

    //    /// Sends the MIDITimedBigMessage _msg_ to the open MIDI out port
    //    virtual bool HardwareMsgOut ( const MIDITimedBigMessage &msg );

  protected:

    static const int DEFAULT_IN_PORT = MIDI_MAPPER;  ///< The default in port
    static const int DEFAULT_OUT_PORT = MIDI_MAPPER; ///< The default out port
    static const int DEFAULT_QUEUE_SIZE = 256;       ///< The default queue size

    /* END OF NEW */

    /// The callback function called to every timer tick when playing
    static void CALLBACK win32_timer( UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2 );

    /// The callback function called for MIDI in
    static void CALLBACK win32_midi_in( HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 );

    // HMIDIIN in_handle;              ///< Windows handle to the MIDI in port
    // HMIDIOUT out_handle;            ///< Windows handle to the MIDI out port
    int timer_id;  ///< Windows id of the hardware timer
    int timer_res; ///< Resolution of the hardware timer
};
}

#endif // _WIN32

#endif // JDKSMIDI_DRIVERWIN32_H
