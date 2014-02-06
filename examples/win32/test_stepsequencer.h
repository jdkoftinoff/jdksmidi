/*
 *
 * Example using the classes MIDITrack and MIDIMultiTrack for
 * libJDKSmidi C++ MIDI Library.
 * A simple step sequencer: you can add, remove, edit MIDI
 * events and play and save your file (console app, no GUI!)
 *
 * Copyright (C) 2014 N.Cassetta
 * ncassetta@tiscali.it
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


/* This is a very basic, and not comfortable, step sequencer, made for demostrating
   editing capabilities of the jdksmidi library. It creates an AdvancedSequencer class instance,
   gets it MultiTrack, and allow the user to edit it.
   You can load and save MIDI files, play them, view the file content, edit the file.
   You can insert, delete or change these MIDI events: note, control (in particular volume and pan)
   patch and tempo. For changing an event, insert a new event (same note, control, patch, tempo) at
   same time position.
*/


#ifndef TEST_STEPSEQUENCER_H_INCLUDED
#define TEST_STEPSEQUENCER_H_INCLUDED


#include "jdksmidi/advancedsequencer.h"
using namespace jdksmidi;



static const char helpstring[] =
"\nAvailable commands:\n\
   load filename       : Loads the file into the sequencer\n\
   save filename       : Saves the file\n\
   outport port        : Sets port as current output device\n\
   play                : Starts playback from current time\n\
   stop                : Stops playback\n\
   dump                : Prints a dump of all midi events in the file\n\
   dump n              : Prints a dump of track n\n\
   goto meas [beat]    : Move current time to given meas and beat\n\
                         (numbered from 1)\n\
   <<                  : Rewind\n\
   < [n]               : Moves current time n steps backward\n\
                         (if omitted, one step)\n\
   > [n]               : Moves current time n steps forward (as above)\n\
   t<                  : Moves insert position to previous track\n\
   t>                  : Moves insert position to next track\n\
   step                : Sets the step length in MIDI clocks\n\
   note n [vel len]    : Inserts a note event: n note, vel velocity, len length\n\
                         (remembers last note vel and len, so you can omit\n\
                          them, or only len, if they are the same)\n\
   volume val          : Inserts a volume event at current position\n\
   pan val             : Inserts a pan event at current position\n\
   control nn val      : Inserts a control nn event at current position\n\
   patch val           : Inserts a patch event at current position\n\
   tempo val           : Inserts a tempo event at current position\n\
   note n *, volume *, etc...\n\
                       : Deletes the event (event must be at cur time and track)\
   help                : Prints this help screen\n\
   quit                : Exits\n\n\
   NOTE: when playing, the sequencer notifier will print beat messages,\n\
   messing up the program input prompt. You can still type your commands\n\n";


void GetCommand();
void DumpMIDIMultiTrack( MIDIMultiTrack *mlt );


class position {
public:

    position ( MIDIMultiTrack* t ) :
        time( 0 ), track( 1 ), step ( t->GetClksPerBeat() ), tracks( t ) {}
    MIDIClockTime gettime() const { return time; }
    void settime(MIDIClockTime t) { time = t; }
    int gettrack() const { return track; }
    void setstep(MIDIClockTime s) { step = s;}
    MIDIClockTime getstep() const { return step; }
    void rewind()   { time = 0; }
    void stepforward()  { time += step; }
    void stepback() { time = ( time > step ? time - step : 0 ); }
    void previoustrack()    { track = (track > 1 ? track - 1 : 1); }
    void nexttrack()    { track = (track < tracks->GetNumTracks()-1 ? track+1 : track); }

private:
    MIDIClockTime time;
    int track;
    MIDIClockTime step;
    MIDIMultiTrack* tracks;
};



#endif // TEST_STEPSEQUENCER_H_INCLUDED
