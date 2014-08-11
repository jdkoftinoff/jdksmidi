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
// doxygen comments by N. Cassetta ncassetta@tiscali.it
//

#ifndef JDKSMIDI_QUEUE_H
#define JDKSMIDI_QUEUE_H

#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

namespace jdksmidi
{

///
/// This is a simple circular buffer containing MIDI messages.
/// The MIDIDriver class contains a in and out queue, used as a temporary storage for incoming and outcoming
/// MIDI messages
///

class MIDIQueue
{
  public:

    /// The constructor creates a queue of size _num_msgs_
    MIDIQueue( int num_msgs );

    /// The destructor frees allocated memory
    virtual ~MIDIQueue();

    /// Empties the queue
    void Clear();

    /// Returns *true* if the queue is not full (we can put a message into the queue)
    bool CanPut() const;

    /// Returns *true* if the queue is not empty (we can get a message from the queue)
    bool CanGet() const;

    /// Return *true* if the queue is full
    bool IsFull() const
    {
        return !CanPut();
    }

    /// Puts the MIDI message _msg_ in the queue
    void Put( const MIDITimedBigMessage &msg )
    {
        buf[next_in] = msg;
        next_in = ( next_in + 1 ) % bufsize;
    }

    /// Get next message from the queue
    MIDITimedBigMessage Get() const
    {
        return MIDITimedBigMessage( buf[next_out] );
    }

    /// Moves the next_out pointer without getting messages
    void Next()
    {
        next_out = ( next_out + 1 ) % bufsize;
    }

    /// Returns a pointer to the next message wirhout popping it from the queue
    const MIDITimedBigMessage *Peek() const
    {
        return &buf[next_out];
    }

  protected:
    MIDITimedBigMessage *buf; ///< The array of MIDI messages
    int bufsize;              ///< The size of the array
    volatile int next_in;     ///< The next free index for putting messages
    volatile int next_out;    ///< The index of the next message to get
};
}

#endif
