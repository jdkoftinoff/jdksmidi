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

#ifdef WIN32
#    include <windows.h>
#endif

#include "jdksmidi/fileread.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/track.h"
#include "jdksmidi/world.h"

using namespace jdksmidi;

void DumpMIDITimedBigMessage(MIDITimedBigMessage* msg)
{
    if (msg) {
        char msgbuf[1024];
        fprintf(stdout, "%8ld : %s\n", msg->GetTime(), msg->MsgToText(msgbuf));

        if (msg->IsSysEx()) {
            fprintf(stdout, "\tSYSEX length: %d\n", msg->GetSysEx()->GetLength());
        }
    }
}

void DumpMIDITrack(MIDITrack* t)
{
    MIDITimedBigMessage* msg;

    for (int i = 0; i < t->GetNumEvents(); ++i) {
        msg = t->GetEventAddress(i);
        DumpMIDITimedBigMessage(msg);
    }
}

void DumpAllTracks(MIDIMultiTrack* mlt)
{
    fprintf(stdout, "Clocks per beat: %d\n\n", mlt->GetClksPerBeat());

    for (int i = 0; i < mlt->GetNumTracks(); ++i) {
        if (mlt->GetTrack(i)->GetNumEvents() > 0) {
            fprintf(stdout, "DUMP OF TRACK #%2d:\n", i);
            DumpMIDITrack(mlt->GetTrack(i));
            fprintf(stdout, "\n");
        }
    }
}

void DumpMIDIMultiTrack(MIDIMultiTrack* mlt)
{
    MIDIMultiTrackIterator i(mlt);
    MIDITimedBigMessage* msg;
    fprintf(stdout, "Clocks per beat: %d\n\n", mlt->GetClksPerBeat());
    i.GoToTime(0);

    do {
        int trk_num;

        if (i.GetCurEvent(&trk_num, &msg)) {
            fprintf(stdout, "#%2d - ", trk_num);
            DumpMIDITimedBigMessage(msg);
        }
    } while (i.GoToNextEvent());
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        MIDIFileReadStreamFile rs(argv[1]);
        MIDIMultiTrack tracks;
        MIDIFileReadMultiTrack track_loader(&tracks);
        MIDIFileRead reader(&rs, &track_loader);
        reader.Parse();
        DumpMIDIMultiTrack(&tracks);
    }

    return 0;
}
