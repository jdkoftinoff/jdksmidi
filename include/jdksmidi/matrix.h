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
** Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

//
// doxygen comments by N. Cassetta ncassetta@tiscali.it
//

#ifndef JDKSMIDI_MATRIX_H
#define JDKSMIDI_MATRIX_H


#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"

namespace jdksmidi
{

/// This class implements a matrix to keep track of notes on and hold pedal for every channel.
/// It is used by MIDIDriver and MIDISequencerTrackState
class  MIDIMatrix
{
public:

    /// The constructor creates an empty matrix
    MIDIMatrix();

    /// The destructor
    virtual ~MIDIMatrix();

    /// Processes the given MIDI message upgrading the matrix
    virtual bool Process ( const MIDIMessage &m );

    /// Resets the matrix (no notes on, no pedal hold)
    virtual void Clear();

    /// Returns the total number of notes on
    int GetTotalCount() const
    {
        return total_count;
    }

    /// Returns the number of notes on for given channel (channels from 0 to 15)
    int GetChannelCount ( int channel ) const
    {
        return channel_count[channel];
    }

    /// Gets the number of notes on given the channel and the note MIDI value
    int GetNoteCount ( int channel, int note ) const
    {
        return note_on_count[channel][note];
    }

    /// Returns *true* if pedal is holding on guven channel
    bool GetHoldPedal ( int channel ) const
    {
        return hold_pedal[channel];
    }


protected:

    /// Decrements the note count (currently _m_ is ignored)
    virtual void DecNoteCount ( const MIDIMessage &m, int channel, int note );

    /// Increments the note count (currently _m_ is ignored)
    virtual void IncNoteCount ( const MIDIMessage &m, int channel, int note );

    /// Clear the note count and the pedal on the given channel
    virtual void ClearChannel ( int channel );

    /// It is called by Process() for non note and non pedal messages (currently does nothing)
    virtual void OtherMessage ( const MIDIMessage &m );

    /// Sets the note count
    void SetNoteCount ( unsigned char chan, unsigned char note, unsigned char val )
    {
        note_on_count[chan][note] = val;
    }

    /// Sets the channel note count
    void SetChannelCount ( unsigned char chan, int val )
    {
        channel_count[chan] = val;
    }

private:
    unsigned char note_on_count[16][128];
    int channel_count[16];
    bool hold_pedal[16];
    int total_count;
};

}


#endif


