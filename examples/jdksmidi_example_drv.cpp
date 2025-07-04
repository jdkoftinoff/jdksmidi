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

#include "jdksmidi/driverdump.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/track.h"
#include "jdksmidi/world.h"

using namespace jdksmidi;

void DumpTrackNames(MIDISequencer* seq)
{
    fprintf(stdout, "TEMPO = %f\n", seq->get_track_state(0)->tempobpm);

    for (int i = 0; i < seq->get_num_tracks(); ++i) {
        fprintf(stdout, "TRK #%2d : NAME = '%s'\n", i, seq->get_track_state(i)->track_name);
    }
}

void PlayDumpManager(MIDIManager* mgr)
{
    MIDISequencer* seq = mgr->GetSeq();
    double pretend_clock_time = 0.0;
    seq->go_to_time((unsigned long)pretend_clock_time);
    mgr->SeqPlay();
    // simulate a clock going forward with 10ms resolution for 1 minute

    for (pretend_clock_time = 0.0; pretend_clock_time < 60.0 * 1000.0; pretend_clock_time += 100) {
        mgr->GetDriver()->time_tick((unsigned long)pretend_clock_time);
    }

    mgr->SeqStop();
    mgr->GetDriver()->all_notes_off();
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        MIDIFileReadStreamFile rs(argv[1]);
        MIDIMultiTrack tracks(64);
        MIDIFileReadMultiTrack track_loader(&tracks);
        MIDIFileRead reader(&rs, &track_loader);
        MIDISequencerGUIEventNotifierText gui(stdout);
        MIDISequencer seq(&tracks, &gui);
        MIDIDriverDump driver(128, stdout);
        MIDIManager mgr(&driver, &gui);
        reader.parse();
        seq.go_to_zero();
        mgr.SetSeq(&seq);
        DumpTrackNames(&seq);
        PlayDumpManager(&mgr);
    }

    return 0;
}
