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
/*
	Part of this file (see AlsaDriver.h)
	Originally developed for CFugue, adapted into jdksmidi with special permission

	Copyright (C) 2009-2014 Cenacle Research India Private Limited

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.

    $LastChangedDate: 2013-12-18 10:59:57 +0530 (Wed, 18 Dec 2013) $
    $Rev: 197 $
    $LastChangedBy: krishnapg $
*/

//
// MODIFIED by N. Cassetta ncassetta@tiscali.it
//

#ifndef JDKSMIDI_DRIVER_H
#define JDKSMIDI_DRIVER_H

#include <chrono>
#include <string>

#include "jdksmidi/msg.h"
#include "jdksmidi/matrix.h"
#include "jdksmidi/process.h"
#include "jdksmidi/queue.h"
#include "jdksmidi/tick.h"

#define AVOID_TIMESTAMPING
#include"../RtMidi/RtMidi.h"

namespace jdksmidi
{

///
/// This is the base class for objects which communicate with hardware MIDI ports.
/// It is a pure virtual because for effective I/O we need OS details. Currently only two specialized
/// classes are implemented: MIDIDriverWin32 (for MS Windows) and MIDIDriverDump (a dummy driver that only writes
/// its I/O to the screen).
/// The driver is capable of MIDI in and out by mean of two MIDIQueue buffers, and can process incoming and
/// outcoming messages with optional MIDIProcessor (for transposing, muting, etc.). It inherits from pure
/// virtual MIDITick, i.e. a class with a callback method TimeTick() to be called at every timer tick. You can
/// also set a function for further processing.
/// This class is used by the MIDIManager together with a MIDISequencer for playback; AdvancedSequencer uses
/// it too. See the example files.
///

class MIDIDriver : public MIDITick
{

public:

    /// The constructor. It allocates memory for the MIDI queues, and set to NULL the pointers to in, thru and out
    /// processors
    /// \param queue_size the length in bits of the in and out MIDIQueue
    MIDIDriver ( int queue_size );

    /// The destructor frees the memory allocated for the MIDIQueue.\ Eventual MIDIProcessor pointer are not owned.
    virtual ~MIDIDriver();

    /// Empties the in and out MIDIQueue and the MIDIMatrix
    virtual void Reset();

    /// Returns the midi in queue
    MIDIQueue * InputQueue()
    {
        return &in_queue;
    }

    const MIDIQueue * InputQueue() const
    {
        return &in_queue;
    }

    /// Returns the midi out queue
    MIDIQueue * OutputQueue()
    {
        return &out_queue;
    }

    const MIDIQueue * OutputQueue() const
    {
        return &out_queue;
    }

    /// Returns true if the output queue is not full
    bool CanOutputMessage() const
    {
        return out_queue.CanPut();
    }

    /// Enables the MIDI thru (incoming MIDI messages are echoed to the MIDI out)
    void SetThruEnable ( bool f )
    {
        thru_enable = f;
    }

    /// Returns *true* if MIDI thru is enabled
    bool GetThruEnable() const
    {
        return thru_enable;
    }

    /// Sets the thru MIDIprocessor
    void SetThruProcessor ( MIDIProcessor *proc )
    {
        thru_proc = proc;
    }

    /// Sets the out MIDIProcessor
    void SetOutProcessor ( MIDIProcessor *proc )
    {
        out_proc = proc;
    }

    /// Sets the in MIDIProcessor
    void SetInProcessor ( MIDIProcessor *proc )
    {
        in_proc = proc;
    }

    /// Sets an additional user procedure that can process MIDI data. You should create your own class,
    /// inherited from MIDITick, implementing the pure virtual TimeTick(). Then call this with a pointer
    /// to your instance and your TimeTick() will be called by the driver TimeTick() ( MIDIDriver also inherits
    /// from MIDITick ), allowing you to process incoming and outcoming data.
    void SetTickProc ( MIDITick *tick )
    {
        tick_proc = tick;
    }

    /// Processes the message _nsg_ with the out processor and the midi matrix and then puts it
    /// in the out_queue. The message is effectively sent to the MIDI hardware by the TimeTick() callback
    void OutputMessage ( MIDITimedBigMessage &msg );

    /// Sends a MIDI ALL NOTES OFF message on selected midi chanel
    void AllNotesOff ( int chan );

    /// Sends a MIDI ALL NOTES OFF message on all midi channels
    void AllNotesOff();

    /// This should be called (by an OS originated callback or by TimeTick() function) every time a MIDI message
    /// comes to the system (and you want get it).
    /// Your function should parse raw bytes, arrange them into a MIDITimedBigMessage and call this,
    /// which processes the message with the in MIDIProcessor, manages the MIDI thru and at last put it
    /// into the in queue
    /// \see TimeTick()
    virtual bool HardwareMsgIn ( MIDITimedBigMessage &msg );

    /* NEW BY NC:
	 * NOTE: In order to develop MIDI driver classes for other OS than Windows I started to
	 * integrate older MIDIDriverWin32 methods into the base class, giving them as pure virtual. So
	 * now every subclass of a MIDIDriver must implement these.
	 */

    /// Opens the MIDI in port _id_
    virtual bool OpenMIDIInPort ( int id );

    /// Opens the MIDI out port _id_
    virtual bool OpenMIDIOutPort ( int id );

    /// Closes the open MIDI in port
    virtual void CloseMIDIInPort();

    /// Closes the open MIDI out port
    virtual void CloseMIDIOutPort();

    /// Sends the MIDITimedBigMessage _msg_ to the open MIDI out port
    virtual bool HardwareMsgOut ( const MIDITimedBigMessage &msg );

    /// Resets open MIDI out port
    virtual void ResetMIDIOut() = 0;

    /// Starts the hardware timer for playing MIDI. Default time resolution is 1 ms
    virtual bool StartTimer ( int resolution_ms = DEFAULT_TIMER_RESOLUTION ) = 0;

    /// Stops the hardware timer
    virtual void StopTimer() = 0;


    /// This is the TimeTick (callback) procedure inherited from MIDITick; when the hardware timer is started, it is
    /// called at every timer tick.
    /// It currently runs the additional tick procedure (see SetTickProc()), then gets messages from the out queue
    /// and calls HardwareMsgOut() for them. You can use the tick procedure (or override this function) if you
    /// need to poll MIDI in hardware: your function should poll the hardware, parse the bytes, form a message,
    /// and give it to HardwareMsgIn()
    /// \note This class gives no callback mechanism: it must be implemented in subclasses accordingly OS details
    virtual void TimeTick ( unsigned long sys_time );

    /* Moreover, now the driver keeps track statically of the MIDI devices installed on the computer
     * so, by these method, you can get them
     */

    /// Gets the nunber of MIDI in ports present on the computer.
    static unsigned int GetNumMIDIInDevs();

    /// Gets the number of MIDI out ports present on the computer.
    static unsigned int GetNumMIDIOutDevs();

	/// Gets the name of the MIDI in port _id_.
	static std::string GetMIDIInDevName(unsigned int id);

    /// Gets the name of the MIDI out port _id_.
	static std::string GetMIDIOutDevName(unsigned int id);

    /// This is used internally to get the system time in msecs, from the start of the program.
    static unsigned long GetSystemTime();

    static const int DEFAULT_TIMER_RESOLUTION = 1; ///< The default timer resolution is 1 msec
    // public: used by AdvancedSequencer

protected:

    RtMidiIn*   m_pMidiIn;          ///< The RtMidi abstraction for the MIDI in port
    RtMidiOut*	m_pMidiOut;         ///< The RtMidi abstraction for the MIDI out port
    std::string	 m_strClientName;   ///< Ab optional name for the MIDI driver

    MIDIQueue in_queue;             ///< The in queue
    MIDIQueue out_queue;            ///< The out queue

    MIDIProcessor *in_proc;         ///< The in processor
    MIDIProcessor *out_proc;        ///< The out processor
    MIDIProcessor *thru_proc;       ///< The thru processor

    bool thru_enable;               ///< **true** if thru is enabled

    bool timer_open;                ///< **true** if the time has been started

    MIDITick *tick_proc;            ///< An additional TimeTick object: see SetTickProc()

    MIDIMatrix out_matrix;          ///< Keeps track of notes on going to MIDI out

    static const int DEFAULT_BUFFER_SIZE = 384;

    int buffer_size;                ///< The size of the buffer
    unsigned char* buffer;          ///< The buffer used to send MIDI bytes

    static std::chrono::steady_clock::time_point session_start;
                                    ///< A time_point representing the start of the program
};

// TODO: (note to Jeff by NC) the out_matrix has no functions to get it, and it is currently unused by
// other classes. If we don't want to eliminate it, we could introduce an enable/disable processing
// feature to speed up

}

#endif
