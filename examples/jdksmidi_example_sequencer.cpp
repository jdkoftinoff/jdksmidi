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

#include "jdksmidi/fileread.h"            // for MIDIFileRead, MIDIFileReadStreamFile
#include "jdksmidi/filereadmultitrack.h"  // for MIDIFileReadMultiTrack
#include "jdksmidi/msg.h"                 // for MIDITimedBigMessage
#include "jdksmidi/multitrack.h"          // for MIDIMultiTrack, MIDIMultiTrackIterator
#include "jdksmidi/sequencer.h"           // for MIDISequencer
#include "jdksmidi/sysex.h"               // for MIDISystemExclusive
#include "jdksmidi/track.h"               // for MIDITrack

#include <cinttypes>  // for PRIu32
#include <cstdio>     // for fprintf, stdout

using namespace jdksmidi;

void DumpMIDIBigMessage(MIDITimedBigMessage* msg)
{
    if (msg) {
        char msgbuf[1024];
        fprintf(stdout, "%s\n", msg->msg_to_text(msgbuf));

        if (msg->is_sys_ex()) {
            fprintf(stdout, "\tSYSEX length: %d\n", msg->get_sys_ex()->get_length());
        }
    }
}

void DumpMIDITimedBigMessage(MIDITimedBigMessage* msg)
{
    if (msg) {
        char msgbuf[1024];
        fprintf(stdout, "%8" PRIu32 " : %s\n", msg->get_time(), msg->msg_to_text(msgbuf));

        if (msg->is_sys_ex()) {
            fprintf(stdout, "\tSYSEX length: %d\n", msg->get_sys_ex()->get_length());
        }
    }
}

void DumpMIDITrack(MIDITrack* t)
{
    MIDITimedBigMessage* msg;

    for (int i = 0; i < t->get_num_events(); ++i) {
        msg = t->get_event_address(i);
        DumpMIDITimedBigMessage(msg);
    }
}

void DumpAllTracks(MIDIMultiTrack* mlt)
{
    fprintf(stdout, "Clocks per beat: %d\n\n", mlt->get_clks_per_beat());

    for (int i = 0; i < mlt->get_num_tracks(); ++i) {
        if (mlt->get_track(i)->get_num_events() > 0) {
            fprintf(stdout, "DUMP OF TRACK #%2d:\n", i);
            DumpMIDITrack(mlt->get_track(i));
            fprintf(stdout, "\n");
        }
    }
}

void DumpMIDIMultiTrack(MIDIMultiTrack* mlt)
{
    MIDIMultiTrackIterator i(mlt);
    MIDITimedBigMessage* msg;
    fprintf(stdout, "Clocks per beat: %d\n\n", mlt->get_clks_per_beat());
    i.go_to_time(0);

    do {
        int trk_num;

        if (i.get_cur_event(&trk_num, &msg)) {
            fprintf(stdout, "#%2d - ", trk_num);
            DumpMIDITimedBigMessage(msg);
        }
    } while (i.go_to_next_event());
}

void PlayDumpSequencer(MIDISequencer* seq)
{
    float pretend_clock_time = 0.0;
    float next_event_time = 0.0;
    MIDITimedBigMessage ev;
    int ev_track;
    seq->go_to_time_ms(pretend_clock_time);

    if (!seq->get_next_event_time_ms(&next_event_time)) {
        return;
    }

    // simulate a clock going forward with 10ms resolution for 1 minute

    for (; pretend_clock_time < 60.0 * 1000.0; pretend_clock_time += 10.0) {
        // find all events that came before or a the current time
        while (next_event_time <= pretend_clock_time) {
            if (seq->get_next_event(&ev_track, &ev)) {
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

                if (!seq->get_next_event_time_ms(&next_event_time)) {
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
        reader.parse();
        // DumpMIDIMultiTrack( &tracks );
        PlayDumpSequencer(&seq);
    }

    return 0;
}
