/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004-2025  Jeffrey Koftinoff
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE MIT LICENSE ***
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
/*
** Copyright 1986 to 1998 By Jeffrey Koftinoff
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by Jeffrey Koftinoff
**
*/

#ifndef JDKSMIDI_TRACK_H
#define JDKSMIDI_TRACK_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <memory>

namespace jdksmidi {

///
/// MIDITrackChunkSize is a constant which specifies how many events are in one MIDITrackChunk.
///

int const MIDITrackChunkSize = 512;

///
/// A MIDITrack's events are allocated in these chunks in order to avoid memory fragmentation
/// in embedded systems or other systems lacking an MMU. Every item within a MIDITrackChunk
/// object is a MIDITimedBigMessage. To avoid unnecessary copied of big events, access
/// to these events is only done via the GetEventAddress() method.
///

class MIDITrackChunk
{
  public:
    ///
    /// GetEventAddress() const returns the address of the MIDITimedBigMessage referred to by
    /// event_num
    /// @param event_num an integer specifying an event number in the range 0 to MIDITrackChunkSize
    /// @returns The const pointer to the requested event.
    ///
    MIDITimedBigMessage const* GetEventAddress(int event_num) const;

    ///
    /// GetEventAddress()  returns the address of the MIDITimedBigMessage referred to by event_num
    /// @param event_num an integer specifying an event number in the range 0 to MIDITrackChunkSize
    /// @returns The non-const pointer to the requested event.
    ///

    MIDITimedBigMessage* GetEventAddress(int event_num);

  protected:
  private:
    MIDITimedBigMessage buf[MIDITrackChunkSize];
};

///
/// The MIDIChunksPerTrack constant specifies the maximum number of MIDITrackChunks that can be in
/// one track. The value MIDIChunksPerTrack * MIDITrackChunkSize is the total number of events. This
/// is a constant in order to avoid memory fragmentation in embedded systems or systems without an
/// MMU
///

int const MIDIChunksPerTrack = 512;

///
/// The MIDITrack class is a container that manages an array of MIDIChunk objects and provides an
/// interface to the user that is useful for managing a list of MIDITimedBigMessages. It internally
/// stores MIDITimedBigMessage objects. There is a fixed maximum number of events that it can store,
/// which is defined by MIDIChunksPerTrack * MIDITrackChunkSize.
///

class MIDITrack
{
  public:
    ///
    /// Construct a MIDITrack object with the specified number of events
    /// @param size The number of events, defaults to 0
    ///
    MIDITrack(int size = 0);

    ///
    /// Copy Constructor for a MIDITrack object
    /// @param t The reference to the MIDITrack object to copy
    ///
    MIDITrack(MIDITrack const& t);

    ///
    /// The MIDITrack Destructor, frees all chunks and referenced MIDITimedBigMessage's
    ///
    ~MIDITrack();

    ///
    /// Clear() sets the number of active events in the track to 0. It does NOT
    /// free any events. See the Shrink() method.
    ///
    void Clear();

    ///
    /// Shrink() frees any unused MIDITrackChunk objects and associated MIDITimedBigMessage events.
    ///
    void Shrink();

    ///
    /// ClearAndMerge() allows you to merge the events in two separate tracks into a third track.
    /// @param src1 Pointer to first track
    /// @param src2 Pointer to second track
    /// ClearAndMerge() assumes all events in both tracks are already ordered by time.
    ///
    void ClearAndMerge(MIDITrack const* src1, MIDITrack const* src2);

    // bool Insert( int start_event, int num_events );
    //    bool  Delete( int start_event, int num_events);
    //    void  Sort();

    bool Expand(int increase_amount = (MIDITrackChunkSize));

    MIDITimedBigMessage* GetEventAddress(int event_num);

    MIDITimedBigMessage const* GetEventAddress(int event_num) const;

    MIDITimedBigMessage const* GetEvent(int event_num) const;
    MIDITimedBigMessage* GetEvent(int event_num);
    bool GetEvent(int event_num, MIDITimedBigMessage* msg) const;

    bool PutEvent(MIDITimedBigMessage const& msg);
    bool PutEvent(MIDITimedMessage const& msg, MIDISystemExclusive* sysex);
    bool SetEvent(int event_num, MIDITimedBigMessage const& msg);

    bool MakeEventNoOp(int event_num);

    bool FindEventNumber(MIDIClockTime time, int* event_num) const;

    int GetBufferSize() const;
    int GetNumEvents() const;

  private:
    // void  QSort( int left, int right );

    std::unique_ptr<MIDITrackChunk> chunk[MIDIChunksPerTrack];

    int buf_size;
    int num_events;
};

}  // namespace jdksmidi

#endif
