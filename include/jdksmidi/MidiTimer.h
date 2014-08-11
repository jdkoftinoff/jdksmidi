/*
 This is part of CFugue, a C++ Runtime for MIDI Score Programming
 Copyright (C) 2009 Gopalakrishna Palem

 For links to further information, or to contact the author,
 see <http://cfugue.sourceforge.net/>.

    $LastChangedDate: 2014-05-10 10:56:34 +0530 (Sat, 10 May 2014) $
    $Rev: 203 $
    $LastChangedBy: krishnapg $
*/

#ifndef _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__
#define _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__

#include <chrono>
#include <thread>
namespace CFugue
{
///<Summary>Plays the role of a pseudo MIDI Timer for MIDI Sequencer</Summary>
class MidiTimer
{
  public:
    typedef std::chrono::steady_clock::time_point TimePoint;
    typedef std::chrono::milliseconds Duration;

    ///<Summary>
    /// Returns current time point for MIDI Sequencing.
    /// It is usually measure as the time elapsed since epoch.
    /// Taking the difference of two consequent calls of this gives the elapsed time for MIDI.
    /// @return psuedo time tick offset that is suitable for MIDI sequencer
    ///</Summary>
    static TimePoint Now()
    {
        return std::chrono::steady_clock::now();
    }

    ///<Summary>
    /// Causes the calling thread to sleep
    /// @param ms sleep duration (in milli-seconds)
    ///</Summary>
    static void Sleep( unsigned long ms )
    {
        std::this_thread::sleep_for( Duration( ms ) );
    }
};

} // namespace CFugue

#endif // _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__
