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
// modified by N. Cassetta ncassetta@tiscali.it
//

#ifndef JDKSMIDI_DRIVERDUMP_H
#define JDKSMIDI_DRIVERDUMP_H

#include "jdksmidi/driver.h"

namespace jdksmidi
{

///
/// This class inherits from the pure virtual MIDIDriver and it is designed for debugging purposes.
/// It doesn't send messages to MIDI ports, but prints them to a FILE object as they come, so you
/// can examine what is exactly sent to the driver.
/// \note As MIDIDriver has some pure virtual methods, we must here implement them as dummy functions that
/// do nothing.
///

class MIDIDriverDump : public MIDIDriver
{

public:

    /// The constructor.
    MIDIDriverDump ( int queue_size, FILE *outfile );

    /// The destructor.
    virtual ~MIDIDriverDump();

    /// The following must be implemented because they are pure virtual in base class. However they are dummy
    virtual bool OpenMIDIInPort ( int id = 0 ) { return true; }

    virtual bool OpenMIDIOutPort ( int id = 0 ) { return true; }

    virtual void CloseMIDIInPort() {}

    virtual void CloseMIDIOutPort() {}

    virtual void ResetMIDIOut() {}

    virtual bool StartTimer ( int res = 0 ) { return true; }

    virtual void StopTimer() {}

    /// Prints the MIDI message _msg_ to the file
    virtual bool HardwareMsgOut ( const MIDITimedBigMessage &msg );

    /// Callback function
    virtual void TimeTick ( unsigned long sys_time );

protected:

    FILE *f;            ///< The output file
};

}

#endif
