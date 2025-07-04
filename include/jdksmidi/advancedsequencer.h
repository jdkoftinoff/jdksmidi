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

#ifndef JDKSMIDI_ADVANCEDSEQUENCER_H

#define JDKSMIDI_ADVANCEDSEQUENCER_H

#include "jdksmidi/driver.h"
#include "jdksmidi/driverdump.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/sysex.h"

#include <string>
#include <vector>

#define MAX_WARP_POSITIONS (128)
#define MEASURES_PER_WARP (4)

namespace jdksmidi {
class AdvancedSequencer
{
  public:
    AdvancedSequencer();
    virtual ~AdvancedSequencer();

    bool OpenMIDI(int in_port, int out_port, int timer_resolution = 5);
    void CloseMIDI();

    void SetMIDIThruEnable(bool f);
    bool GetMIDIThruEnable() const;

    void SetMIDIThruChannel(int chan);
    int GetMIDIThruChannel() const;

    void SetMIDIThruTranspose(int val);
    int GetMIDIThruTranspose() const;

    bool Load(char const* fname);
    void reset();

    void go_to_measure(int measure, int beat = 0);
    void go_to_time(MIDIClockTime t);
    void Play(int clock_offset = 0);
    void RepeatPlay(bool enable, int start_measure, int end_measure);
    void Pause();
    void Stop();

    bool IsPlay() { return mgr.IsSeqPlay(); }

    void UnmuteAllTracks();
    void SoloTrack(int trk);
    void UnSoloTrack();
    void SetTrackMute(int trk, bool f);

    void SetTempoScale(double scale);
    double GetTempoWithoutScale() const;
    double GetTempoWithScale() const;

    int GetMeasure() const;
    int GetBeat() const;

    int get_time_sig_numerator() const;
    int get_time_sig_denominator() const;

    int GetTrackNoteCount(int trk) const;
    char const* GetTrackName(int trk) const;
    int GetTrackVolume(int trk) const;

    void SetTrackVelocityScale(int trk, int scale);
    int GetTrackVelocityScale(int trk) const;

    void SetTrackRechannelize(int trk, int chan);
    int GetTrackRechannelize(int trk) const;

    void SetTrackTranspose(int trk, int trans);
    int GetTrackTranspose(int trk) const;

    void ExtractMarkers(std::vector<std::string>* list);
    int GetCurrentMarker() const;

    int FindFirstChannelOnTrack(int trk);

    void ExtractWarpPositions();

    bool IsChainMode() const { return chain_mode; }

    MIDIMultiProcessor thru_processor;
    MIDIProcessorTransposer thru_transposer;
    MIDIProcessorRechannelizer thru_rechannelizer;

    MIDIDriverDump driver;

    MIDIMultiTrack tracks;

    MIDISequencerGUIEventNotifierText notifier;

    MIDISequencer seq;

    MIDIClockTime marker_times[1024];
    int num_markers;

    MIDIManager mgr;

    long repeat_start_measure;
    long repeat_end_measure;
    bool repeat_play_mode;

    int num_warp_positions;
    MIDISequencerState* warp_positions[MAX_WARP_POSITIONS];

    bool file_loaded;
    bool chain_mode;
};

}  // namespace jdksmidi

#endif
