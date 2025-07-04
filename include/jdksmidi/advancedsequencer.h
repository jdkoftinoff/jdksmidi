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

    bool open_midi(int in_port, int out_port, int timer_resolution = 5);
    void close_midi();

    void set_midi_thru_enable(bool f);
    bool get_midi_thru_enable() const;

    void set_midi_thru_channel(int chan);
    int get_midi_thru_channel() const;

    void set_midi_thru_transpose(int val);
    int get_midi_thru_transpose() const;

    bool load(char const* fname);
    void reset();

    void go_to_measure(int measure, int beat = 0);
    void go_to_time(MIDIClockTime t);
    void play(int clock_offset = 0);
    void repeat_play(bool enable, int start_measure, int end_measure);
    void pause();
    void stop();

    bool is_play() { return mgr.is_seq_play(); }

    void unmute_all_tracks();
    void solo_track(int trk);
    void unsolo_track();
    void set_track_mute(int trk, bool f);

    void set_tempo_scale(double scale);
    double get_tempo_without_scale() const;
    double get_tempo_with_scale() const;

    int get_measure() const;
    int get_beat() const;

    int get_time_sig_numerator() const;
    int get_time_sig_denominator() const;

    int get_track_note_count(int trk) const;
    char const* get_track_name(int trk) const;
    int get_track_volume(int trk) const;

    void set_track_velocity_scale(int trk, int scale);
    int get_track_velocity_scale(int trk) const;

    void set_track_rechannelize(int trk, int chan);
    int get_track_rechannelize(int trk) const;

    void set_track_transpose(int trk, int trans);
    int get_track_transpose(int trk) const;

    void extract_markers(std::vector<std::string>* list);
    int get_current_marker() const;

    int find_first_channel_on_track(int trk);

    void extract_warp_positions();

    bool is_chain_mode() const { return chain_mode; }

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

    std::int32_t repeat_start_measure;
    std::int32_t repeat_end_measure;
    bool repeat_play_mode;

    int num_warp_positions;
    MIDISequencerState* warp_positions[MAX_WARP_POSITIONS];

    bool file_loaded;
    bool chain_mode;
};

}  // namespace jdksmidi

#endif
