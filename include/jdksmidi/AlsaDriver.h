/*
	Originally developed for CFugue, adapted into jdksmidi with special permission

	Copyright (C) 2009-2014 Cenacle Research India Private Limited

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.

    $LastChangedDate: 2013-12-18 10:59:57 +0530 (Wed, 18 Dec 2013) $
    $Rev: 197 $
    $LastChangedBy: krishnapg $
*/
/* Some methods of this file were moved into the base class MIDIDriver
   by NC
*/

#ifndef __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__
#define __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__

#include "jdksmidi/msg.h"
#include "jdksmidi/driver.h"
#include "jdksmidi/sequencer.h"

#include <thread> // std::thread
#include <future> // std::future

class RtMidiIn;
class RtMidiOut;

namespace jdksmidi
{
	///<Summary>MIDI Driver for Linux Alsa based machines</Summary>
	class MIDIDriverAlsa : public MIDIDriver
	{
	public:
        enum Defaults { Timer_Resolution_ms = 20, ///< Default MIDI Timer resolution in MilliSeconds
		// NOTE BY NC: Win32 driver has default resolution = 1 ms; is it possible to lower this?
						Queue_Length = 128
					  };

		MIDIDriverAlsa ( const char* szClientName = NULL, int queue_size = Defaults::Queue_Length );

		virtual ~MIDIDriverAlsa()   {}  // destructor moved to MIDIDriver.h by NC

		void ResetMIDIOut();

        /// <Summary>
        /// Creates a background thread to pump MIDI events
        /// at the supplied timer resolution.
        /// Use WaitTillDone() to wait till the background processing completes.
        /// Use StopTimer() after the background processing is completed, to release resources.
        /// @param resolution_ms MIDI Timer resolution in milliseconds
        /// @return false if background thread cannot be started
        /// </Summary>
		bool StartTimer (int resolution_ms = Defaults::Timer_Resolution_ms);

		/// Waits (blocks) till the background thread created with StartTimer()
		/// completes its processing.
		/// After StartTimer() succeeds, use WaitTillDone() followed by StopTimer().
		/// Returns immediately if no background thread is running.
		void WaitTillDone();

        /// Call StopTimer() to release the resources used by the background
        /// procedure created with StartTimer(). StopTimer() Should be called
        /// <i>after</i> the background procedure is done (indicated by BGThreadStatus::COMPLETED).
        /// If background procedure is still running while StopTimer() is called, caller gets
        /// blocked till the background procedure completes.
        /// If no background procedure exists, returns immediately.
		void StopTimer();

/* THESE WERE MOVED INTO MIDIDriver.h by NC
		/// Opens the MIDI input port with the given ID
		/// @return false if the given input port cannot be opened
		bool OpenMIDIInPort ( int id );

        /// Opens the MIDI output port with the given ID
        /// @return false if the given output port cannot be opened
		bool OpenMIDIOutPort ( int id );

		/// Closed any previously opened MIDI Input port
		void CloseMIDIInPort();

		/// Closed any previously opened MIDI Output port
		void CloseMIDIOutPort();
*/

        // needed to implement pure virtual in base class
        void ResetMIDIOut() {}

// End of NC editing


		enum BGThreadStatus {   RUNNING,    ///< Async procedure is running - use WaitTillDone() to wait for completion
                                COMPLETED,  ///< Async procedure completed running - use StopTimer() to finish
                                INVALID     ///< No background procedure running - use StartTimer() to start one
                            };
	protected:
		bool HardwareMsgOut ( const jdksmidi::MIDITimedBigMessage &msg );

		//RtMidiIn*		        m_pMidiIn;
		//RtMidiOut*	        m_pMidiOut;
        std::future<bool>   m_bgTaskResult;

        std::thread* m_pThread;
        //std::string	 m_strClientName;
	};

} // namespace jdksmidi

#endif // __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__
