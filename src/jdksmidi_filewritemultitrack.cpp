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
#include "jdksmidi/filewritemultitrack.h"
#include "jdksmidi/world.h"

namespace jdksmidi {

MIDIFileWriteMultiTrack::MIDIFileWriteMultiTrack(
    MIDIMultiTrack const* mlt_, MIDIFileWriteStream* strm_)
    : multitrack(mlt_)
    , writer(strm_)
{}

MIDIFileWriteMultiTrack::~MIDIFileWriteMultiTrack()
{}

bool MIDIFileWriteMultiTrack::Write(int num_tracks, int division)
{
    bool f = true;

    if (!PreWrite()) {
        return false;
    }

    // first, write the header.
    writer.WriteFileHeader((num_tracks > 0), num_tracks, division);
    // now write each track

    for (int i = 0; i < num_tracks; ++i) {
        if (writer.ErrorOccurred()) {
            f = false;
            break;
        }

        MIDITrack const* t = multitrack->GetTrack(i);


        writer.WriteTrackHeader(0);  // will be rewritten later

        if (t) {
            for (int event_num = 0; event_num < t->GetNumEvents(); ++event_num) {
                MIDITimedBigMessage const* ev = t->GetEventAddress(event_num);

                if (ev && !ev->IsNoOp()) {
                    if (!ev->IsDataEnd()) {
                        writer.WriteEvent(*ev);

                        if (writer.ErrorOccurred()) {
                            f = false;
                            break;
                        }
                    }
                }
            }
        }

        writer.WriteEndOfTrack(0);
        writer.RewriteTrackLength();
    }

    if (!PostWrite()) {
        return false;
    }

    return f;
}

bool MIDIFileWriteMultiTrack::PreWrite()
{
    return true;
}

bool MIDIFileWriteMultiTrack::PostWrite()
{
    return true;
}

}  // namespace jdksmidi
