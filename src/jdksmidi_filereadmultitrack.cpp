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

#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/track.h"

#include <cstdint>

namespace jdksmidi {

MIDIFileReadMultiTrack::MIDIFileReadMultiTrack(MIDIMultiTrack* mlttrk)
    : multitrack(mlttrk)
    , cur_track(-1)
{}

MIDIFileReadMultiTrack::~MIDIFileReadMultiTrack() = default;

void MIDIFileReadMultiTrack::mf_error(char const*)
{}

void MIDIFileReadMultiTrack::mf_starttrack(int trk)
{
    cur_track = trk;
}

void MIDIFileReadMultiTrack::mf_endtrack(int trk)
{
    cur_track = -1;
}

void MIDIFileReadMultiTrack::add_event_to_multi_track(
    MIDITimedMessage const& msg, MIDISystemExclusive* sysex, int dest_track)
{
    if (dest_track != -1 && dest_track < multitrack->get_num_tracks()) {
        auto t = multitrack->get_track(dest_track);

        if (t) {
            t->put_event(msg, sysex);
        }
    }
}

void MIDIFileReadMultiTrack::mf_header(int the_format_, int ntrks_, int division_)
{
    the_format = the_format_;
    num_tracks = ntrks_;
    division = division_;
    multitrack->set_clks_per_beat(division);
}

void MIDIFileReadMultiTrack::chan_message(MIDITimedMessage const& msg)
{
    if (the_format == 0 || cur_track == 0) {
        // split format 0 files into separate tracks, one for each channel,
        // keep track 0 for tempo and meta-events
        add_event_to_multi_track(msg, nullptr, msg.get_channel() + 1);
    }

    else {
        add_event_to_multi_track(msg, nullptr, cur_track);
    }
}

void MIDIFileReadMultiTrack::mf_sysex(MIDIClockTime time, MIDISystemExclusive const& ex)
{
    MIDITimedMessage msg;
    msg.set_sys_ex();
    msg.set_time(time);
    auto sysex = new MIDISystemExclusive(ex);
    add_event_to_multi_track(msg, sysex, cur_track);
}

void MIDIFileReadMultiTrack::mf_arbitrary(MIDIClockTime time, int len, std::uint8_t* data)
{
    // ignore arbitrary byte strings
}

void MIDIFileReadMultiTrack::mf_metamisc(MIDIClockTime time, int, int, std::uint8_t*)
{
    // ignore miscellaneous meta events
}

void MIDIFileReadMultiTrack::mf_seqnum(MIDIClockTime time, int)
{
    // ignore sequence number events
}

void MIDIFileReadMultiTrack::mf_smpte(MIDIClockTime time, int, int, int, int, int)
{
    // ignore smpte events
}

void MIDIFileReadMultiTrack::mf_timesig(
    MIDIClockTime time, int num, int denom_power, int clks_per_metro, int notated_32nd_per_quarter)
{
    MIDITimedMessage msg;
    int denom = 1 << denom_power;
    msg.set_time_sig((std::uint8_t)num, (std::uint8_t)denom);
    msg.set_time(time);
    auto sysex = new MIDISystemExclusive(4);
    sysex->put_byte((std::uint8_t)num);
    sysex->put_byte((std::uint8_t)denom_power);
    sysex->put_byte((std::uint8_t)clks_per_metro);
    sysex->put_byte((std::uint8_t)notated_32nd_per_quarter);
    add_event_to_multi_track(msg, sysex, cur_track);
}

void MIDIFileReadMultiTrack::mf_tempo(MIDIClockTime time, std::uint32_t tempo)
{
    std::uint32_t tempo_bpm_times_32;

    if (tempo == 0)
        tempo = 1;

    // tempo is in microseconds per beat
    // calculate beats per second by
    auto beats_per_second =
        static_cast<float>(1e6 / (double)tempo);  // 1 million microseconds per second
    float beats_per_minute = beats_per_second * 60;
    tempo_bpm_times_32 = static_cast<std::uint32_t>(beats_per_minute * 32.0);
    MIDITimedMessage msg;
    msg.set_tempo32(static_cast<std::uint16_t>(tempo_bpm_times_32));
    msg.set_time(time);
    add_event_to_multi_track(msg, nullptr, cur_track);
}

void MIDIFileReadMultiTrack::mf_keysig(MIDIClockTime time, int c, int v)
{
    MIDITimedMessage msg;
    msg.set_key_sig((std::uint8_t)c, (std::uint8_t)v);
    msg.set_time(time);
    add_event_to_multi_track(msg, nullptr, cur_track);
}

void MIDIFileReadMultiTrack::mf_sqspecific(MIDIClockTime time, int, std::uint8_t*)
{
    // ignore any sequencer specific messages
}

void MIDIFileReadMultiTrack::mf_text(MIDIClockTime time, int type, int len, std::uint8_t* s)
{
    MIDITimedMessage msg;
    msg.set_status(META_EVENT);
    msg.set_meta_type(
        (std::uint8_t)type);  // remember - MF_*_TEXT* id codes match META_*_TEXT codes
    msg.set_time(time);
    auto sysex = new MIDISystemExclusive(len);

    for (int i = 0; i < len; ++i) {
        sysex->put_sys_byte(s[i]);
    }

    add_event_to_multi_track(msg, sysex, cur_track);
}

void MIDIFileReadMultiTrack::mf_eot(MIDIClockTime time)
{
    MIDITimedMessage msg;
    msg.set_status(META_EVENT);
    msg.set_meta_type(META_END_OF_TRACK);
    msg.set_time(time);
    add_event_to_multi_track(msg, nullptr, cur_track);
}

}  // namespace jdksmidi
