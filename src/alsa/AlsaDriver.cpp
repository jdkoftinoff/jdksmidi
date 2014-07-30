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

#if defined _WIN32 || defined WIN32

//#error "---- This Alsa Driver is not for Windows -----"   // Commented by NC

#else	// Below code is only for non_Windows

//#include "RtMidi.h"
#include "jdksmidi/AlsaDriver.h"
#include "MidiTimer.h"

namespace jdksmidi
{
	MIDIDriverAlsa::MIDIDriverAlsa ( const char* szClientName, int queue_size )
		:
		MIDIDriver ( queue_size ),
		m_pMidiIn ( 0 ),
		m_pMidiOut ( 0 ),
		m_pThread ( NULL ),
		m_strClientName(szClientName)
	{
		if(szClientName == NULL) // user did not supply any name for this driver client
		{
			m_strClientName = "Midi Driver Alsa Client"; //TODO: compute unique name based on pid
		}
	}

	MIDIDriverAlsa::~MIDIDriverAlsa()
	{
		StopTimer();
		CloseMIDIInPort();
		CloseMIDIOutPort();
	}

/* THESE WERE MOVED INTO MIDIDriver.cpp by NC

	bool MIDIDriverAlsa::OpenMIDIInPort ( int id )
	{
		if(m_pMidiIn != NULL) // close any open port
		{
			delete m_pMidiIn;
			m_pMidiIn = NULL;
		}
		if(m_pMidiIn == NULL)
		{
			try
			{
				m_pMidiIn = new RtMidiIn(m_strClientName);
			}
			catch ( RtError &error )
			{
				error.printMessage();
				return false;
			}
		}
		if(m_pMidiIn != NULL)
		{
			try
			{
				m_pMidiIn->openPort(id);
			}
			catch(RtError& error)
			{
				error.printMessage();
				return false;
			}
		}
		return true;
	}


	bool MIDIDriverAlsa::OpenMIDIOutPort ( int id )
	{
		if(m_pMidiOut != NULL) // close any open port
		{
			delete m_pMidiOut;
			m_pMidiOut = NULL;
		}
		if(m_pMidiOut == NULL)
		{
			try
			{
				m_pMidiOut = new RtMidiOut(m_strClientName);
			}
			catch(RtError &error)
			{
				error.printMessage();
				return false;
			}
		}
		if(m_pMidiOut != NULL)
		{
			try
			{
				m_pMidiOut->openPort(id);
			}
			catch(RtError &error)
			{
				error.printMessage();
				return false;
			}
		}
		return true;
	}

    bool MIDIDriverAlsa::HardwareMsgOut ( const jdkmidi::MIDITimedBigMessage &msg )
    {
        if(m_pMidiOut != NULL)
        {
            unsigned char status = msg.GetStatus();

            // dont send sysex or meta-events

            if ( status <0xff && status !=0xf0 )
            {
                unsigned char msgBytes[] = {status, msg.GetByte1(), msg.GetByte2(), msg.GetByte3()};

                std::vector<unsigned char> vec(msgBytes, msgBytes+3);

                m_pMidiOut->sendMessage(&vec);
            }

            return true;
        }
        return false;
    }

*/  // End of NC editing

    // This is thread procedure to pump MIDI events
    // We maintain the supplied Timer Resolution by adjusting the sleep duration
	bool AlsaDriverThreadProc(MIDIDriverAlsa* pAlsaDriver, int nTimerResMS)
	{
	    unsigned long nBefore, nAfter;
	    unsigned int nElapsed, nTimeToSleep;
	    while(true)
	    {
            nBefore = MidiTimer::CurrentTimeOffset();

            if(pAlsaDriver->TimeTick(nBefore) == false) break;

            nAfter = MidiTimer::CurrentTimeOffset();

            nElapsed = nAfter - nBefore;

            nTimeToSleep = (nElapsed > nTimerResMS ? 0 : nTimerResMS - nElapsed);

            std::this_thread::sleep_for(std::chrono::milliseconds(nTimeToSleep));
	    }

        return true;
}

	bool MIDIDriverAlsa::StartTimer ( int res )
	{
	    if(m_bgTaskResult.valid()) // Already running
            return false;

        m_bgTaskResult = std::async(std::launch::async, &AlsaDriverThreadProc, this, res);

        return m_bgTaskResult.valid();
	}

	void MIDIDriverAlsa::WaitTillDone()
	{
	    if(m_bgTaskResult.valid() == false) return; // if not running

        auto waitStatus = m_bgTaskResult.wait_for(std::chrono::milliseconds(0));

        while(waitStatus != std::future_status::ready)
        {
             waitStatus = m_bgTaskResult.wait_for(std::chrono::milliseconds(500));
        }
	}

	void MIDIDriverAlsa::StopTimer()
	{
	    // std::future requires get() to be called before it can be used again.
	    // valid() keeps returning true till get() is called. And get() can be
	    // called only once. Once it is called valid() becomes false again.
	    if(m_bgTaskResult.valid())
            m_bgTaskResult.get();
	}


/* THESE WERE MOVED INTO MIDIDriver.cpp by NC
	void MIDIDriverAlsa::CloseMIDIInPort()
	{
	    if(m_pMidiIn != NULL)
	    {
	        m_pMidiIn->closePort();
	        delete m_pMidiIn;
	        m_pMidiIn = NULL;
	    }
	}

	void MIDIDriverAlsa::CloseMIDIOutPort()
	{
	    if(m_pMidiOut != NULL)
	    {
	        m_pMidiOut->closePort();
	        delete m_pMidiOut;
	        m_pMidiOut = NULL;
	    }
	}
*/  // End of NC editing

} // namespace jdksmidi



#endif // _ifndef _Win32
