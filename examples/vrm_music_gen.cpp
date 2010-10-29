/*
  
  VRM Music Generator  based on  libJDKSmidi C++ MIDI Library
  
  Copyright (C) 2010 V.R.Madgazin

  www.vmgames.com

  vrm@vmgames.com
  
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

#ifdef WIN32
#include <windows.h>
#endif

#include "jdksmidi/world.h"
#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filewritemultitrack.h"
using namespace jdksmidi;

#include <iostream>
using namespace std;

template <class I, class D> inline void test_max(D &x, I ma) { if ( x > (D)ma ) x = (D)ma; }
template <class I, class D> inline void test_min(D &x, I mi) { if ( x < (D)mi ) x = (D)mi; }
template <class I1, class D, class I2> inline void mintestmax(I1 mi, D &x, I2 ma) { test_max(x, ma); test_min(x, mi); }

typedef unsigned int uint32;

uint32 seed;
// random 32-bit generator
inline uint32 Randu(uint32 &seed)
{
  static const uint32 RANDUMUL = 1686629717ul;
  static const uint32 RANDUADD =  907633385ul;
  return (seed = RANDUMUL*seed+RANDUADD);
}

// return random number from minval to maxval, include these boundarys
inline int get_rand(int minval, int maxval)
{
  const double k = 1. / (256. * 256. * 256. * 256.);
  return minval + int( Randu(seed) * k * (maxval-minval+1) );
}

const bool ON = true, OFF = false;
bool AddNote( MIDIMultiTrack &tracks, int track_num, MIDIClockTime ticks, int chan, int note, int velocity, bool on )
{
  MIDITimedBigMessage m;
  m.SetTime( ticks );
  if ( on ) m.SetNoteOn( chan, note, velocity );
  else      m.SetNoteOff( chan, note, velocity );
  return tracks.GetTrack( track_num )->PutEvent( m );
}

bool SetInstrument( MIDIMultiTrack &tracks, int track_num, MIDIClockTime ticks, int chan, int instr_num )
{
  MIDITimedBigMessage m;
  m.SetTime( ticks );
  m.SetProgramChange( chan, instr_num );
  return tracks.GetTrack( track_num )->PutEvent( m );
}

// add "silence" after last track event
bool AddSilence( MIDIMultiTrack &tracks, int track_num, MIDIClockTime silence_ticks )
{
  int num = tracks.GetTrack( track_num )->GetNumEvents();
  MIDIClockTime tmax = tracks.GetTrack( track_num )->GetEvent( num-1 )->GetTime();
         AddNote(tracks, track_num, tmax + silence_ticks, 0, 0, 0, ON);
  return AddNote(tracks, track_num, tmax + silence_ticks, 0, 0, 0, OFF);
}

// add ticks time to all last track events (i.e. to all events with max time value)
void LastEventsProlongation( MIDIMultiTrack &tracks, int track_num, MIDIClockTime add_ticks )
{
  MIDITrack *track = tracks.GetTrack( track_num );
  int index = track->GetNumEvents() - 1;
  if ( index < 0 ) return;

  MIDITimedBigMessage *msg = track->GetEvent( index );
  MIDIClockTime tmax = msg->GetTime();

  while ( msg->GetTime() == tmax )
  {
    msg->SetTime( tmax + add_ticks );
    if ( --index < 0 ) break;
    msg = track->GetEvent( index );
  }
}

const char *Program = "VRM Music Generator based on libJDKSmidi C++ MIDI Library";
const char *Copyright = "Copyright (C) 2010 V.R.Madgazin";

// 1 tick = 1 msec if tempo value is 120 (by default), and
const int clks_per_beat = 500; // number of ticks in crotchet = 500
const MIDIClockTime mctime1sec = 1000; // midi ticks in 1 second

MIDIClockTime mctime(double seconds) { return MIDIClockTime( 0.5 + mctime1sec * seconds ); }

const int MAX_INDEX = 28;
const int notes_table[MAX_INDEX+1] = // notes number array: all "white" notes in 4 octaves
{  0, 2, 4, 5, 7, 9,11,  12,14,16,17,19,21,23,  24,26,28,29,31,33,35,  36,38,40,41,43,45,47,  48  };
// 0  1  2  3  4  5  6    7  8  9 10 11 12 13   14 15 16 17 18 19 20   21 22 23 24 25 26 27   28  index
// C  D  E  F  G  A  B    C  D  E  F  G  A  B    C  D  E  F  G  A  B    C  D  E  F  G  A  B    C  notes


void main ( int argc, char **argv )
{
  seed = 3; // [-se] random seed value

  int instrument = 25; // [-in] midi instrument number, 25 = Acoustic Guitar (steel)
  int notes_min_index = 0 ; // [-n0] min index of notes array: 0 for C-dur, 5 for A-moll
  int notes_max_index = 14; // [-n1] max index of notes array: add N*7 to min index for N octaves diapason
  int transposition = 48; // [-tr] notes transposition
  int discrete_time = 1; // [-di] switch for discretization of all time intervals in note duration unit
  
  double music_dur = 43; // [-md] total music duration in seconds (approximately)
  // делим всё время music_dur на отрезки section_dur, чтобы не было слишком больших и малых плотностей нот,
  // а также чтобы при увеличении только одного параметра music_dur не менялись предыдущие отрезки музыки!
  double section_dur = 1; // [-sd] temporal section of music in seconds
  double note_dur = 0.5; // [-nd] note duration in seconds
  double notes_density = 1.5; // [-de] average notes number per note duration

  // arguments parser
  for (int i = 1; i < argc; i += 2)
  {
    const char *key = argv[i];
    int ival = 0;
    double dval = 0.;
    if ( (i+1) < argc )
    {
      ival = atol( argv[i+1] );
      dval = atof( argv[i+1] );
    }
    switch ( key[1] )
    {
      case 'i': instrument = ival; break;
      case 't': transposition = ival; break;
      case 'm': music_dur = dval; break;
      case 's': switch ( key[2] )
                {
                  case 'e': seed = uint32( ival ); break;
                  case 'd': section_dur = dval; break;
                }
                break;
      case 'n': switch ( key[2] )
                {
                  case '0': notes_min_index = ival; break;
                  case '1': notes_max_index = ival; break;
                  case 'd': note_dur = dval; break;
                }
                break;
      case 'd': switch ( key[2] )
                {
                  case 'i': discrete_time = ival; break;
                  case 'e': notes_density = dval; break;
                }
                break;
    }
  }

  // make midi file name from program arguments
  string fname = "vrm";
  for (int i = 1; i < argc; i += 2)
  {
    const char *key1 = 1 + argv[i]; // delete "-" key sign
    fname += "_";
    fname += key1;
    if ( (i+1) < argc )
    {
      fname += "_";
      fname += argv[i+1];
    }
  }
  fname += ".mid";

  MIDIMultiTrack tracks(1);  // the object which will hold all the tracks
  const int trk = 0, // track number
    chan = 0, velocity = 100;

  string text = Program;
  text += ".  ";
  text += Copyright;
  tracks.GetTrack( trk )->PutTextEvent( 0, META_GENERIC_TEXT, text.c_str() );

  SetInstrument( tracks, trk, 0, chan, instrument );

  // incorrect data protection

  mintestmax( 0, notes_min_index, MAX_INDEX );
  mintestmax( notes_min_index, notes_max_index, MAX_INDEX );

  // convert seconds to ticks
  MIDIClockTime mc_music_dur = mctime( music_dur );
  MIDIClockTime mc_section_dur = mctime( section_dur );
  MIDIClockTime mc_note_dur = mctime( note_dur );

  if ( mc_music_dur < 1 ) mc_music_dur = 1;
  mintestmax( 1, mc_section_dur, mc_music_dur );
  mintestmax( 1, mc_note_dur, mc_section_dur );

  // discretization of all time intervals in note duration unit
  if ( discrete_time != 0 )
  {
    discrete_time = 1;
    mc_section_dur = mc_note_dur * ( mc_section_dur / mc_note_dur );
    mc_music_dur = mc_note_dur * ( mc_music_dur / mc_note_dur );
  }

  // reconvert ticks to seconds
  music_dur = mc_music_dur / (double) mctime( 1. );
  section_dur = mc_section_dur / (double) mctime( 1. );
  note_dur = mc_note_dur / (double) mctime( 1. );

  // revised data output
  cout << endl << Program << endl << endl;
  cout << Copyright << endl << endl;
  cout << " [-se] seed = " << seed << endl;
  cout << " [-in] instrument = " << instrument << endl;
  cout << " [-n0] notes_min_index = " << notes_min_index << endl;
  cout << " [-n1] notes_max_index = " << notes_max_index << endl;
  cout << " [-tr] transposition = " << transposition << endl;
  cout << " [-di] discrete_time = " << discrete_time << endl;
  cout << " [-md] .music_dur = " << music_dur << endl;
  cout << " [-sd] .section_dur = " << section_dur << endl;
  cout << " [-nd] .note_dur = " << note_dur << endl;
  cout << " [-de] .notes_density = " << notes_density << endl;

  // music generator's main loop
  for ( MIDIClockTime t = 0; t < mc_music_dur; t += mc_section_dur )
  {
    // current music section duration
    MIDIClockTime dur = mc_music_dur - t;
    if ( dur > mc_section_dur ) dur = mc_section_dur;

    int section_dur_in_note_dur = ( dur / mc_note_dur );

    int notes_in_section = int( 0.5 + (notes_density*dur)/mc_note_dur );

    bool stop = false; // track midi events overflow flag

    // generate all notes in current music section
    for (int n = 0; n < notes_in_section; ++n)
    {
      // generate absolute midi note number

      int index = get_rand( notes_min_index, notes_max_index );
      int note = transposition + notes_table[ index ];

      // generate absolute time of note appearance

      MIDIClockTime dt; // "note-on" time, relative to t

      if ( discrete_time != 0 )
      {
        int dt_in_note_dur = get_rand( 0, section_dur_in_note_dur - 1 );
        dt = dt_in_note_dur * mc_note_dur;
      }
      else // discrete_time = 0
      {
        dt = get_rand( 0, dur-1 );
      }

      // write note on-off events to track

      // if some note switch OFF in first "note on-off process" (two separate midi events)
      // and identical note switch ON in second "note on-off process"
      // and if two events (note-off from first and note-on from second) take place at the same time
      // during sort events order it is possible to generate not right events sequence:
      // first (note-on from second) and second (note-off from first), as a result only "note off"
      // we subtract 1 tick from note duration to avoid of such conflict!
      // there is efficiently only for (discrete_time != 0)
      MIDIClockTime sub = 1;
      if ( discrete_time == 0 || mc_note_dur <= 1 ) sub = 0;

            AddNote(tracks, trk, (t+dt)                    , chan, note, velocity, ON);
      if ( !AddNote(tracks, trk, (t+dt) + (mc_note_dur-sub), chan, note, velocity, OFF) )
      {
        stop = true; // track midi events overflow
        break;
      }
    }

    if ( stop ) break;
  }

  tracks.SortEventsOrder(); // it is absolutely necessary!

  // remove superfluous notes with identical number and time
  int removed = tracks.GetTrack( trk )->RemoveIdenticalEvents();

  cout << endl << "Removed events quantity = " << removed << endl;

  // last note (or last chord) prolongation
  LastEventsProlongation( tracks, trk, 4*mc_note_dur );

  // add silence ending
  AddSilence( tracks, trk, 2*mc_note_dur );

  MIDIFileWriteStreamFileName out_stream( fname.c_str() );

  if ( out_stream.IsValid() )
  {
    MIDIFileWriteMultiTrack writer( &tracks, &out_stream );

    int num_tracks = tracks.GetNumTracksWithEvents();

    if ( writer.Write( num_tracks, clks_per_beat ) )
    {
      cout << "\nOK writing file " << fname.c_str() << endl;
    }
    else
    {
      cerr << "\nError writing file " << fname.c_str() << endl;
    }
  }
}

