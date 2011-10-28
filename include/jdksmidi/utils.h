/*
  libjdksmidi C++ Class Library for MIDI addendum

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program;
  if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com
// vrm@vmgames.com
//

#ifndef JDKSMIDI_UTILS_H
#define JDKSMIDI_UTILS_H

#include "jdksmidi/multitrack.h"

namespace jdksmidi
{

// copy events from src to dst multitrack for time interval from 0 to max_time_sec seconds
void ClipMultiTrack( const MIDIMultiTrack &src, MIDIMultiTrack &dst, double max_time_sec );

// copy src to dst without all ignore_channel events
void CopyWithoutChannel( const MIDIMultiTrack &src, MIDIMultiTrack &dst, int ignore_channel );

// skip pause before really start of music, ignore channel 0...15 events (9 for percussion)
void CompressStartPause( const MIDIMultiTrack &src, MIDIMultiTrack &dst, int ignore_channel = -1 );

// convert src music to dst solo melody, ignore channel 0...15 events (9 for percussion)
// this simple code works better for src MultiTrack with 1 track,
// if not, we can make before the call of CollapseMultiTrack()
void SoloMelodyConverter( const MIDIMultiTrack &src, MIDIMultiTrack &dst, int ignore_channel = -1 );

// collapse all tracks from src to dst track 0
void CollapseMultiTrack( const MIDIMultiTrack &src, MIDIMultiTrack &dst );

// collapse all src tracks to track 0 and than expand it to dst tracks 0-17:
// midi channel events to tracks 1-16, and all other types of events to track 0
void CollapseAndExpandMultiTrack( const MIDIMultiTrack &src, MIDIMultiTrack &dst );

bool ReadMidiFile(const char *file, MIDIMultiTrack &dst);
  
// write multitrack to midi file; note that src must contain right clks_per_beat value
bool WriteMidiFile(const MIDIMultiTrack &src, const char *file, bool use_running_status = true);

double GetMisicDurationInSeconds(const MIDIMultiTrack &mt);

std::string MultiTrackAsText(const MIDIMultiTrack &mt);

std::string EventAsText(const MIDITimedBigMessage &ev);

// add ticks time to all last track events (i.e. to all events with max time value)
void LastEventsProlongation( MIDIMultiTrack &tracks, int track_num, MIDIClockTime add_ticks );

// add "pause" after last track event
bool AddEndingPause( MIDIMultiTrack &tracks, int track_num, MIDIClockTime pause_ticks );

template <class I> inline void jdks_safe_delete_object(I *&obj)
{
    delete obj;
    obj = 0;
}

template <class I> inline void jdks_safe_delete_array(I *&arr)
{
    delete [] arr;
    arr = 0;
}

template <class D> inline int jdks_float2int(D d)
{
    return int( d >= D(0.) ? ( d + D(0.5) ):( d - D(0.5) ) );
}

}

#endif
