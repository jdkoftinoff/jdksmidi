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
#include "jdksmidi/sequencer.h"
#include "jdksmidi/track.h"
#include "jdksmidi/world.h"

using namespace jdksmidi;

void DumpMIDIBigMessage(MIDITimedBigMessage* msg)
{
    if (msg) {
        char msgbuf[1024];
        fprintf(stdout, "%s\n", msg->MsgToText(msgbuf));

        if (msg->IsSysEx()) {
            fprintf(stdout, "\tSYSEX length: %d\n", msg->GetSysEx()->GetLength());
        }
    }
}

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

void PlayDumpSequencer(MIDISequencer* seq)
{
    float pretend_clock_time = 0.0;
    float next_event_time = 0.0;
    MIDITimedBigMessage ev;
    int ev_track;
    seq->GoToTimeMs(pretend_clock_time);

    if (!seq->GetNextEventTimeMs(&next_event_time)) {
        return;
    }

    // simulate a clock going forward with 10ms resolution for 1 minute

    for (; pretend_clock_time < 60.0 * 1000.0; pretend_clock_time += 10.0) {
        // find all events that came before or a the current time
        while (next_event_time <= pretend_clock_time) {
            if (seq->GetNextEvent(&ev_track, &ev)) {
                // found the event!
                // show it to stdout
                fprintf(
                    stdout,
                    "tm=%06.0f : evtm=%06.0f :trk%02d : ",
                    pretend_clock_time,
                    next_event_time,
                    ev_track);
                DumpMIDITimedBigMessage(&ev);
                // now find the next message

                if (!seq->GetNextEventTimeMs(&next_event_time)) {
                    // no events left so end
                    fprintf(stdout, "End\n");
                    return;
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        MIDIFileReadStreamFile rs(argv[1]);
        MIDIMultiTrack tracks(64);
        MIDIFileReadMultiTrack track_loader(&tracks);
        MIDIFileRead reader(&rs, &track_loader);
        //    MIDISequencerGUIEventNotifierText notifier( stdout );
        //    MIDISequencer seq( &tracks, &notifier );
        MIDISequencer seq(&tracks);
        reader.Parse();
        // DumpMIDIMultiTrack( &tracks );
        PlayDumpSequencer(&seq);
    }

    return 0;
}
