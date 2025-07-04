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
#ifndef JDKSMIDI_SONG_H
#define JDKSMIDI_SONG_H

#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/sequencer.h"

namespace jdksmidi {
class MIDISong
{
  public:
    MIDISong(int max_tracks);
    virtual ~MIDISong();

    bool load(char const* fname);

    // bool Save( const char *fname );

    MIDIMultiTrack* get_multi_track() { return multitrack; }

    MIDIMultiTrack const* get_multi_track() const { return multitrack; }

    MIDISequencer* get_seq() { return sequencer; }

    MIDISequencer const* get_seq() const { return sequencer; }

  protected:
    MIDIMultiTrack track;
    MIDISequencer seq;

    char title;
};
}  // namespace jdksmidi

#endif
