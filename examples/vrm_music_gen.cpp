/*
  VRM Music Generator  based on  libJDKSmidi C++ MIDI Library
*/
const char *version = "1.23"; // from February 2011
/*
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
using namespace jdksmidi;

#include <iostream>
using namespace std;

template <class I, class D> inline void test_max(D &x, I ma)
{
    if ( x > (D)ma )
        x = (D)ma;
}

template <class I, class D> inline void test_min(D &x, I mi)
{
    if ( x < (D)mi )
        x = (D)mi;
}

template <class I1, class D, class I2> inline void mintestmax(I1 mi, D &x, I2 ma)
{
    test_max(x, ma);
    test_min(x, mi);
}

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

const bool ON = true,
           OFF = false;
bool AddNote( MIDIMultiTrack &tracks, int track_num, MIDIClockTime ticks, int chan, int note, int velocity, bool on )
{
    MIDITimedBigMessage m;
    m.SetTime( ticks );
    if ( on )
    {
        m.SetNoteOn(  chan, note, velocity );
    }
    else
    {
        m.SetNoteOff( chan, note, velocity );
    }
    return tracks.GetTrack( track_num )->PutEvent( m );
}

bool SetInstrument( MIDIMultiTrack &tracks, int track_num, MIDIClockTime ticks, int chan, int instr_num )
{
    MIDITimedBigMessage m;
    m.SetTime( ticks );
    m.SetProgramChange( chan, instr_num );
    return tracks.GetTrack( track_num )->PutEvent( m );
}

const char *Program = "VRM Music Generator based on libJDKSmidi C++ MIDI Library";
const char *Copyright = "Copyright (C) 2010 V.R.Madgazin";

// 1 tick = 1 msec if tempo value is 120 (by default), and
const int clks_per_beat = 500; // number of ticks in crotchet = 500
const MIDIClockTime mctime1sec = 1000; // midi ticks in 1 second

MIDIClockTime mctime(double seconds)
{
    return MIDIClockTime( 0.5 + mctime1sec * seconds );
}

const int MAX_INDEX = 70;
int notes_table[MAX_INDEX+1] = // notes number array: all "white" notes in 10 octaves
{  0, 2, 4, 5, 7, 9,11, 12, };
// 0  1  2  3  4  5  6   7  index
// C  D  E  F  G  A  B   C  notes
void setup_notes()
{
    for (int i = 7; i <= MAX_INDEX; i += 7)
        for (int j = 0; ( j < 7 && i+j <= MAX_INDEX ); ++j)
            notes_table[i+j] = 12 + notes_table[i+j-7];
}

int main ( int argc, char **argv )
{
    int retcode = -1;

    setup_notes();

    // music generator data
    seed = 3; // [-se] random seed value
    int instrument = 25; // [-in] midi instrument number, 25 = Acoustic Guitar (steel)
    int notes_min_index = 0 ; // [-n0] min index of notes array: 0 for C-dur, 5 for A-moll
    int notes_max_index = 14; // [-n1] max index of notes array: add N*7 to min index for N octaves diapason
    int transposition = 48; // [-tr] notes transposition
    int discrete_time = 1; // [-di] switch for discretization of all time intervals in note duration unit
    int channel = 1; // [-ch] channel number (1...16), 1 for melodic instruments, 10 for percussion instruments
    double music_dur = 43; // [-md] total music duration in seconds (approximately)
    // делим всё время music_dur на отрезки section_dur, чтобы не было слишком больших и малых плотностей нот,
    // а также чтобы при увеличении только одного параметра music_dur не менялись предыдущие отрезки музыки!
    double section_dur = 1; // [-sd] temporal section of music in seconds
    double note_dur = 0.5; // [-nd] note duration in seconds
    double notes_density = 1.5; // [-de] average notes number per note duration
    int prolongation = 4; // [-pr] last note (or last chord) prolongation time in note_dur

    // music generator arguments data parser
    for (int i = 1; i < argc; i += 2)
    {
        int ival = 0;
        double dval = 0.;
        if ( (i+1) < argc )
        {
            ival = atol( argv[i+1] );
            dval = atof( argv[i+1] );
        }
        string key = argv[i];
        if ( key == "-se" ) seed = uint32( ival );
        if ( key == "-in" ) instrument = ival;
        if ( key == "-n0" ) notes_min_index = ival;
        if ( key == "-n1" ) notes_max_index = ival;
        if ( key == "-tr" ) transposition = ival;
        if ( key == "-di" ) discrete_time = ival;
        if ( key == "-ch" ) channel = ival;
        if ( key == "-md" ) music_dur = dval;
        if ( key == "-sd" ) section_dur = dval;
        if ( key == "-nd" ) note_dur = dval;
        if ( key == "-de" ) notes_density = dval;
        if ( key == "-pr" ) prolongation = ival;
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
    velocity = 100;

    string text = "\n" + string(Program) + "\nversion " + string(version) + "  " + string(Copyright) + "\n";
    tracks.GetTrack( trk )->PutTextEvent( 0, META_GENERIC_TEXT, text.c_str() );

    // midifile channel num = "external" channel num - 1
    SetInstrument( tracks, trk, 0, channel-1, instrument );

    // incorrect data protection

    mintestmax( 0, notes_min_index, MAX_INDEX );
    mintestmax( notes_min_index, notes_max_index, MAX_INDEX );

    // convert seconds to ticks
    MIDIClockTime mc_music_dur = mctime( music_dur );
    MIDIClockTime mc_section_dur = mctime( section_dur );
    MIDIClockTime mc_note_dur = mctime( note_dur );

    if ( mc_music_dur < 1 )
        mc_music_dur = 1;
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
    cout << " [-ch] channel = " << channel << endl;
    cout << " [-md] .music_dur = " << music_dur << endl;
    cout << " [-sd] .section_dur = " << section_dur << endl;
    cout << " [-nd] .note_dur = " << note_dur << endl;
    cout << " [-de] .notes_density = " << notes_density << endl;
    cout << " [-pr] prolongation = " << prolongation << endl;

    // music generator's main loop
    for ( MIDIClockTime t = 0; t < mc_music_dur; t += mc_section_dur )
    {
        // current music section duration
        MIDIClockTime dur = mc_music_dur - t;
        if ( dur > mc_section_dur )
            dur = mc_section_dur;

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
            if ( discrete_time == 0 || mc_note_dur <= 1 )
                sub = 0;

            AddNote(tracks, trk, (t+dt)                    , channel-1, note, velocity, ON);
            if ( !AddNote(tracks, trk, (t+dt) + (mc_note_dur-sub), channel-1, note, velocity, OFF) )
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
    LastEventsProlongation( tracks, trk, prolongation*mc_note_dur );

    // add ending "silence"
    AddEndingPause( tracks, trk, 2*mc_note_dur );

    tracks.SetClksPerBeat( clks_per_beat );

    if ( WriteMidiFile( tracks, fname.c_str() ) )
    {
        cout << "\nOK writing file " << fname.c_str() << endl;
        retcode = 0;
    }
    else
    {
        cerr << "\nError writing file " << fname.c_str() << endl;
    }

    return retcode;
}

