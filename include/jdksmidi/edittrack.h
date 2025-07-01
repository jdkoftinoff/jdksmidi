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

#ifndef JDKSMIDI_EDITTRACK_H
#define JDKSMIDI_EDITTRACK_H

#include "jdksmidi/matrix.h"
#include "jdksmidi/process.h"
#include "jdksmidi/track.h"

namespace jdksmidi {

class MIDIEditTrackEventMatcher
{
  public:
    MIDIEditTrackEventMatcher();
    virtual ~MIDIEditTrackEventMatcher();

    virtual bool Match(MIDITimedBigMessage const& ev) = 0;
};

class MIDIEditTrack
{
  public:
    MIDIEditTrack(MIDITrack* track_);
    virtual ~MIDIEditTrack();

    //
    // Process applies a MIDI process to all events that are matched
    //

    void Process(
        MIDIClockTime start_time,
        MIDIClockTime end_time,
        MIDIProcessor* process,
        MIDIEditTrackEventMatcher* match);

    //
    // Truncate erases all events after a certain time. then
    // adds appropriate note off's
    //
    void Truncate(MIDIClockTime start_time);

    //
    // this merge function merges two other tracks into this track.
    // this is the faster form of merge
    //
    void Merge(
        MIDITrack* trk1,
        MIDITrack* trk2,
        MIDIEditTrackEventMatcher* match1,
        MIDIEditTrackEventMatcher* match2);

    //
    // this erase function will erase all events from start to end time
    // and can be jagged or not.
    //
    void Erase(
        MIDIClockTime start,
        MIDIClockTime end,
        bool jagged = true,
        MIDIEditTrackEventMatcher* match = 0);

    //
    // this delete function will delete all events like erase and then
    // shift the events over
    //
    void Delete(
        MIDIClockTime start,
        MIDIClockTime end,
        bool jagged = true,
        MIDIEditTrackEventMatcher* match = 0);

    //
    // this insert function will insert 'length' clicks starting at
    // the events at start time.
    //
    void Insert(MIDIClockTime start, MIDIClockTime length);

    //
    // this shift function will shift all event times by an offset.
    //
    void Shift(signed long offset, MIDIEditTrackEventMatcher* match = 0);

  protected:
    MIDIMatrix matrix;
    MIDITrack* track;

  private:
};

}  // namespace jdksmidi

#endif
