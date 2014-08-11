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

#ifndef JDKSMIDI_PROCESS_H
#define JDKSMIDI_PROCESS_H

#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

namespace jdksmidi
{

class MIDIProcessor;
class MIDIMultiProcessor;
class MIDIProcessorTransposer;

///
/// This is the base class for objects that process MIDI messages (for ex.\ transpose,
/// velocity scale, mute, solo etc).
/// This class is embedded in the MIDIDriver for processing in, out and thru messages. Advanced classes like
/// MIDISequencer and AdvancedSequencer use a multi purpose derived class (see MIDISequencerTrackProcessor)
/// for their processing, however they allows you to process data without directly dealing with it.
/// You might want to subclass this to implement your own processing.
///

class MIDIProcessor
{
  public:
    /// The constructor
    MIDIProcessor();

    /// The destructor
    virtual ~MIDIProcessor();

    /// This must be overriden by your class
    virtual bool Process( MIDITimedBigMessage *msg ) = 0;
};

///
/// This class inherits from pure virtual MIDIProcessor and allows the user to set a 'chain' of MIDIProcessor
/// objects. You can set the max number of processors in the constructor and add or remove processors.
/// When you call the Process() method the output of a processor is passed to the input of the following.
///

class MIDIMultiProcessor : public MIDIProcessor
{
  public:
    /// The constructor.\ You must set the max number of MIDIProcessor of the chain
    MIDIMultiProcessor( int num_processors );

    /// The destructor.\ MIDIProcessor objects given to a MIDIMultiProcessor are NOT owned by it
    virtual ~MIDIMultiProcessor();

    /// Sets the MIDIProcessor _proc_ in the chain.
    /// \param position the position of the MIDIProcessor in the chain. You have not to worry about plugging them
    /// in adjacent positions since the processor skips unassigned positions. The range is 0 ... num_processors-1
    /// \param proc the MIDIProcessor (it is not owned by the MIDIMultiProcessor
    void SetProcessor( int position, MIDIProcessor *proc )
    {
        processors[position] = proc;
    }

    /// Returns a pointer to the MIDIProcessor at position _position_.\ (NULL if no processor)
    MIDIProcessor *GetProcessor( int position )
    {
        return processors[position];
    }

    const MIDIProcessor *GetProcessor( int position ) const
    {
        return processors[position];
    }

    // NEW by NC
    /// Removes the MIDIProcessor from the chain.
    /// This is equivalent to SetProcessor(_position_, 0), as the processors are not owned by the class
    void RemoveProcessor( int position )
    {
        processors[position] = 0;
    }

    /// This is the method inherited from the base class MIDIProcessor. It passes the MIDI message _msg_
    /// through the chain of processors, skipping unassigned positions
    virtual bool Process( MIDITimedBigMessage *msg );

  private:
    MIDIProcessor **processors; ///< An array of pointers to MIDIProcessor objects
    int num_processors;         ///< The size of the array
};

///
/// This class inherits from pure virtual MIDIProcessor and transposes MIDI note messages.
/// However, advanced classes like MIDISequencer and AdvancedSequencer implement their own transposer
/// (see MIDISequencerTrackProcessor) so this class is not directly used in the library.
///

class MIDIProcessorTransposer : public MIDIProcessor
{
  public:
    /// The constructor
    MIDIProcessorTransposer();

    /// The destructor
    virtual ~MIDIProcessorTransposer();

    /// Set channel transposing: messages of channel _chan_ will be transposed by _trans_ semitones)
    void SetTransposeChannel( int chan, int trans )
    {
        trans_amount[chan] = trans;
    }

    /// Returns the transposition amount for the given channel
    int GetTransposeChannel( int chan ) const
    {
        return trans_amount[chan];
    }

    /// Set the transposition regardless of the channel
    void SetAllTranspose( int trans );

    /// Transposes the message _msg_ (inherited from MIDIProcessor)
    virtual bool Process( MIDITimedBigMessage *msg );

  private:
    int trans_amount[16];
};

///
/// This class inherits from pure virtual MIDIProcessor and recbannelizes MIDI channel messages.
///

class MIDIProcessorRechannelizer : public MIDIProcessor
{
  public:
    /// The constructor
    MIDIProcessorRechannelizer();

    /// The destructor
    virtual ~MIDIProcessorRechannelizer();

    /// Set channel rechannelize: transposing: messages of channel _src_chan_ will be mapped into _dest_chan_
    /// (channels = 0 ... 15).
    void SetRechanMap( int src_chan, int dest_chan )
    {
        rechan_map[src_chan] = dest_chan;
    }

    /// Returns the destination channel for _src_chan_ messages (channels = 0 ... 15)
    int GetRechanMap( int src_chan ) const
    {
        return rechan_map[src_chan];
    }

    /// Sets rechannelization for all messages to _dest_chan_ regardless of the  original channel
    void SetAllRechan( int dest_chan );

    /// Rechannelizes the message _msg_ (inherited from MIDIProcessor)
    virtual bool Process( MIDITimedBigMessage *msg );

  private:

    int rechan_map[16];
};
}

#endif
