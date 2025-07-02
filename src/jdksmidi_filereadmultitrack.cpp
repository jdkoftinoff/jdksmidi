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
#include "jdksmidi/world.h"

namespace jdksmidi {

MIDIFileReadMultiTrack::MIDIFileReadMultiTrack(MIDIMultiTrack* mlttrk)
    : multitrack(mlttrk)
    , cur_track(-1)
{}

MIDIFileReadMultiTrack::~MIDIFileReadMultiTrack()
{}

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

void MIDIFileReadMultiTrack::AddEventToMultiTrack(
    MIDITimedMessage const& msg, MIDISystemExclusive* sysex, int dest_track)
{
    if (dest_track != -1 && dest_track < multitrack->GetNumTracks()) {
        auto t = multitrack->GetTrack(dest_track);

        if (t) {
            t->PutEvent(msg, sysex);
        }
    }
}

void MIDIFileReadMultiTrack::mf_header(int the_format_, int ntrks_, int division_)
{
    the_format = the_format_;
    num_tracks = ntrks_;
    division = division_;
    multitrack->SetClksPerBeat(division);
}

void MIDIFileReadMultiTrack::ChanMessage(MIDITimedMessage const& msg)
{
    if (the_format == 0 || cur_track == 0) {
        // split format 0 files into separate tracks, one for each channel,
        // keep track 0 for tempo and meta-events
        AddEventToMultiTrack(msg, 0, msg.GetChannel() + 1);
    }

    else {
        AddEventToMultiTrack(msg, 0, cur_track);
    }
}

void MIDIFileReadMultiTrack::mf_sysex(MIDIClockTime time, MIDISystemExclusive const& ex)
{
    MIDITimedMessage msg;
    msg.SetSysEx();
    msg.SetTime(time);
    auto sysex = new MIDISystemExclusive(ex);
    AddEventToMultiTrack(msg, sysex, cur_track);
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
    msg.SetTimeSig((std::uint8_t)num, (std::uint8_t)denom);
    msg.SetTime(time);
    auto sysex = new MIDISystemExclusive(4);
    sysex->PutByte((std::uint8_t)num);
    sysex->PutByte((std::uint8_t)denom_power);
    sysex->PutByte((std::uint8_t)clks_per_metro);
    sysex->PutByte((std::uint8_t)notated_32nd_per_quarter);
    AddEventToMultiTrack(msg, sysex, cur_track);
}

void MIDIFileReadMultiTrack::mf_tempo(MIDIClockTime time, unsigned long tempo)
{
    unsigned long tempo_bpm_times_32;

    if (tempo == 0)
        tempo = 1;

    // tempo is in microseconds per beat
    // calculate beats per second by
    float beats_per_second =
        static_cast<float>(1e6 / (double)tempo);  // 1 million microseconds per second
    float beats_per_minute = beats_per_second * 60;
    tempo_bpm_times_32 = static_cast<unsigned long>(beats_per_minute * 32.0);
    MIDITimedMessage msg;
    msg.SetTempo32(static_cast<unsigned short>(tempo_bpm_times_32));
    msg.SetTime(time);
    AddEventToMultiTrack(msg, 0, cur_track);
}

void MIDIFileReadMultiTrack::mf_keysig(MIDIClockTime time, int c, int v)
{
    MIDITimedMessage msg;
    msg.SetKeySig((std::uint8_t)c, (std::uint8_t)v);
    msg.SetTime(time);
    AddEventToMultiTrack(msg, 0, cur_track);
}

void MIDIFileReadMultiTrack::mf_sqspecific(MIDIClockTime time, int, std::uint8_t*)
{
    // ignore any sequencer specific messages
}

void MIDIFileReadMultiTrack::mf_text(MIDIClockTime time, int type, int len, std::uint8_t* s)
{
    MIDITimedMessage msg;
    msg.SetStatus(META_EVENT);
    msg.SetMetaType((std::uint8_t)type);  // remember - MF_*_TEXT* id codes match META_*_TEXT codes
    msg.SetTime(time);
    auto sysex = new MIDISystemExclusive(len);

    for (int i = 0; i < len; ++i) {
        sysex->PutSysByte(s[i]);
    }

    AddEventToMultiTrack(msg, sysex, cur_track);
}

void MIDIFileReadMultiTrack::mf_eot(MIDIClockTime time)
{
    MIDITimedMessage msg;
    msg.SetStatus(META_EVENT);
    msg.SetMetaType(META_END_OF_TRACK);
    msg.SetTime(time);
    AddEventToMultiTrack(msg, 0, cur_track);
}

}  // namespace jdksmidi
