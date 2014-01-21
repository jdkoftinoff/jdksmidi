#ifndef TEST_STEPSEQUENCER_H_INCLUDED
#define TEST_STEPSEQUENCER_H_INCLUDED


#include "jdksmidi/advancedsequencer.h"
using namespace jdksmidi;



static const char helpstring[] =
"\nAvailable commands:\n\
   load filename       : Loads the file into the sequencer\n\
   save filename       : Saves the file\n\
   outport port        : Sets port as current output device\n\
                         (stops the sequencer)\n\
   play                : Starts playback from current time\n\
   stop                : Stops playback\n\
   goto meas [beat]    : Move current time to given meas and beat\n\
                         (numbered from 0)\n\
   dump                : Prints a dump of all midi events in the file\n\
   <<                  : Rewind\n\
   <                   : Step backward\n\
   >                   : Step forward\n\
   t<                  : Previous track\n\
   t>                  : Next track\n\
   step                : Sets the step\n\
   tempo val           : Inserts a tempo event at current position\n\
   volume val          : Inserts a volume event at current position\n\
   pan val             : Inserts a pan event at current position\n\
   control nn val      : Inserts a control nn event at current position\n\
   patch val           : Inserts a patch event at current position\n\
   note n vel len      : Inserts a note event: n note, vel velocity, len length\n\
   help                : Prints this help screen\n\
   quit                : Exits\n";


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
