/*

  AdvancedSequencer class example for libJDKSmidi C++ MIDI Library
  (console app, no GUI!)

  Copyright (C) 2013 N.Cassetta
  ncassetta@tiscali.it

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
// Copyright (C) 2013 N. Cassetta
// ncassetta@tiscali.it
//

#include "jdksmidi/advancedsequencer.h"
using namespace jdksmidi;

#include <iostream>
#include <string>
using namespace std;


//
// globals
//

string command_buf, command, par1, par2;    // used by GetCommand() for parsing the user input
AdvancedSequencer sequencer;                // an AdvancedSequencer (without GUI notifier)

const char helpstring[] =
"\nAvailable commands:\n\
   load filename       : Loads the file into the sequencer\n\
   ports               : Enumerates MIDI In and OUT ports\n\
   outport port        : Sets port as current output device\n\
                         (stops the sequencer)\n\
   inport port         : Sets port as current input device\n\
                         (stops the sequencer)\n\
   play                : Starts playback from current time\n\
   loop meas1 meas2    : Sets loop play (doesn't start it!) from\n\
                         meas1 to meas2. Input 0 0 for normal play\n\
   stop                : Stops playback\n\
   rew                 : Stops playback and go to the beginning\n\
   goto meas [beat]    : Move current time to given meas and beat\n\
                         (numbered from 0)\n\
   dump                : Prints a dump of all midi events in the file\n\
   solo track          : Soloes the given track. All other tracks are muted\n\
   unsolo              : Unsoloes all tracks\n\
   mute track          : Toggle on and off the muting of given track.\n\
                         First time muting is on\n\
   unmute              : Unmutes all tracks\n\
   tscale scale        : Sets global tempo scale. scale is in percent\n\
                         (ex. 200 = twice faster, 50 = twice slower)\n\
   vscale track scale  : Sets velocity scale for given track (scale as above)\n\
   trans track amount  : Sets transpose for given track.\n\
                         amount is in semitones (positive or negative)\n\
   tracknames          : Prints the names of all tracks of the file\n\
   b                   : (backward) Moves current time to the previous measure\n\
   f                   : (forward) Moves current time to the next measure\n\
   help                : Prints this help screen\n\
   quit                : Exits\n\
All commands can be given during playback\n";


void GetCommand()
// gets from the user the string command_buf, then parses it dividing it into command, par1 and par2 substrings
{
    size_t pos1, pos2;

    cout << "\n=> ";
    getline( cin, command_buf );

    command = "";
    par1 = "";
    par2 = "";

    for ( size_t i = 0; i < command_buf.size(); ++i)
    {
        command_buf[i] = tolower( command_buf[i]);
    }

    pos1 = command_buf.find_first_not_of ( " ");
    pos2 = command_buf.find_first_of (" ", pos1+1);
    if ( pos1 == string::npos )
    {
        return;
    }
    command = command_buf.substr (pos1, pos2 - pos1);

    pos1 = command_buf.find_first_not_of ( " ", pos2);
    pos2 = command_buf.find_first_of (" ", pos1+1);
    if ( pos1 == string::npos )
    {
        return;
    }
    par1 = command_buf.substr (pos1, pos2 - pos1);

    pos1 = command_buf.find_first_not_of ( " ", pos2);
    pos2 = command_buf.find_first_of (" ", pos1+1);
    if ( pos1 == string::npos )
    {
        return;
    }
    par2 = command_buf.substr (pos1, pos2 - pos1);
}


void DumpMIDITimedBigMessage( const MIDITimedBigMessage *msg )
{
    if ( msg )
    {
        char msgbuf[1024];

        // note that Sequencer generate SERVICE_BEAT_MARKER in files dump,
        // but files themselves not contain this meta event...
        // see MIDISequencer::beat_marker_msg.SetBeatMarker()
        if ( msg->IsBeatMarker() )
        {
            fprintf ( stdout, "%8ld : %s <------------------>", msg->GetTime(), msg->MsgToText ( msgbuf ) );
        }
        else
        {
            fprintf ( stdout, "%8ld : %s", msg->GetTime(), msg->MsgToText ( msgbuf ) );
        }

        if ( msg->IsSystemExclusive() )
        {
            fprintf ( stdout, "SYSEX length: %d", msg->GetSysEx()->GetLengthSE() );
        }

        fprintf ( stdout, "\n" );
    }
}

void DumpMIDIMultiTrack( MIDIMultiTrack *mlt )
{
    MIDIMultiTrackIterator i ( mlt );
    const MIDITimedBigMessage *msg;
    fprintf ( stdout , "Clocks per beat: %d\n\n", mlt->GetClksPerBeat() );
    i.GoToTime ( 0 );

    int num_lines = 0;
    do
    {
        int trk_num;

        if ( i.GetCurEvent ( &trk_num, &msg ) )
        {
            fprintf ( stdout, "#%2d - ", trk_num );
            DumpMIDITimedBigMessage ( msg );
        }
        num_lines++;
        if (num_lines == 100)
        {
            system ("PAUSE");
            num_lines = 0;
        }
    }
    while ( i.GoToNextEvent() );
}



int main( int argc, char **argv )
{
    cout << "TYPE help TO GET A LIST OF AVAILABLE COMMANDS" << endl << endl;
    sequencer.SetMIDIThruEnable( true );
    while ( command != "quit" )                     // main loop
    {
        GetCommand();                               // gets user input and parse it

        if( command == "load" )                     // loads a file
        {
            if ( sequencer.Load( par1.c_str() ))
            {
                cout << "Loaded file " << par1 << endl;
            }
            else
            {
                cout << "Error loading file" << endl;
            }
        }
        else if ( command == "ports")               // enumerates the midi ports
        {
            if ( MIDIDriverWin32::GetNumMIDIInDevs() )
            {using namespace jdksmidi;

                cout << "MIDI IN PORTS:" << endl;
                for ( unsigned int i = 0; i < MIDIDriverWin32::GetNumMIDIInDevs(); i++ )
                {
                    cout << i << ": " << MIDIDriverWin32::GetMIDIInDevName( i ) << endl;
                }
            }
            else
            {
                cout << "NO MIDI IN PORTS" << endl;
            }
            if ( MIDIDriverWin32::GetNumMIDIOutDevs() )
            {
                cout << "MIDI OUT PORTS:" << endl;
                for ( unsigned int i = 0; i < MIDIDriverWin32::GetNumMIDIOutDevs(); i++ )
                {
                    cout << i << ": " << MIDIDriverWin32::GetMIDIOutDevName( i ) << endl;
                }
            }
            else
            {
                cout << "NO MIDI OUT PORTS" << endl;
            }
        }
        else if ( command == "outport")                 // changes the midi out port
        {
            int port = atoi( par1.c_str() );
            sequencer.SetOutputPort( port );
            cout << "Assigned out port n. " << sequencer.GetOutputPort();
        }
        else if ( command == "inport" )                 // changes the midi in port
        {
            int port = atoi( par1.c_str() );
            sequencer.SetInputPort( port );
            cout << "Assigned in port n. " << sequencer.GetInputPort();
        }
        else if ( command == "play")                    // starts playback
        {
            sequencer.Play();
            cout << "Sequencer started at measure: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "loop" )                   // sets repeates play
        {
            int beg = atoi( par1.c_str() );
            int end = atoi( par2.c_str() );
            if ( !(beg == 0 && end == 0) )
            {
                sequencer.SetRepeatPlay( true, beg, end );
                cout << "Repeat play set from measure " << beg << " to measure " << end << endl;
            }
            else
            {
                sequencer.SetRepeatPlay(false, 0, 0);
                cout << "Repeat play cleared" << endl;
            }
        }
        else if (command == "stop")                     // stops playback
        {
            sequencer.Stop();
            cout << "Sequencer stopped at measure: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "rew")                     // stops and rewind to time 0
        {
            sequencer.GoToZero();
            cout << "Rewind to 0:0" << endl;
        }
        else if ( command == "goto")                    // goes to meas and beat
        {
            int measure = atoi( par1.c_str() );
            int beat = atoi ( par2.c_str() );
            if ( measure < 0 || measure > sequencer.GetNumMeasures() - 1)
            {
                cout << "Invalid position" << endl;
            }
            else
            {
                sequencer.GoToMeasure( measure, beat );
                cout << "Actual position: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
            }

        }
        else if ( command == "dump")                    // prints a dump of the sequencer contents
        {
            DumpMIDIMultiTrack( sequencer.GetMultiTrackAddress() );
        }
        else if ( command == "solo")                    // soloes a track
        {
            int track = atoi( par1.c_str() );
            sequencer.SoloTrack( track );
            cout << "Soloed track " << track << endl;
        }
        else if ( command == "unsolo")                  // unsoloes all tracks
        {
            sequencer.UnSoloTrack();
            cout << "Unsoloed all tracks" << endl;
        }
        else if ( command == "mute")                    // mutes a track
        {
            int track = atoi( par1.c_str() );
            sequencer.SetTrackMute( track, !sequencer.GetTrackMute( track ));
            if (sequencer.GetTrackMute( track))
            {
                cout << "Muted track " << track << endl;
            }
            else
            {
                cout << "Unmuted track " << track << endl;
            }
        }
        else if ( command == "unmute")                  // unmutes a track
        {
            sequencer.UnmuteAllTracks();
            cout << "Unmuted all tracks" << endl;
        }
        else if ( command == "tscale")                  // scales plsyback tempo
        {
            int scale = atoi( par1.c_str() );
            sequencer.SetTempoScale( (float)scale / 100 );
            cout << "Tempo scale : " << scale << "%  " <<
                    " Effective tempo: " << sequencer.GetTempoWithScale() << "bpm" << endl;
        }
        else if ( command == "vscale" )                 // scales velocity for a track
        {
            int track = atoi( par1.c_str() );
            int scale = atoi( par2.c_str() );
            sequencer.SetTrackVelocityScale( track, scale );
            cout << "Track " << track << " velocity scale set to " << scale << "%" << endl;
        }
        else if ( command == "trans" )                  // transposes a track
        {
            int track = atoi( par1.c_str() );
            int amount = atoi( par2.c_str() );
            sequencer.SetTrackTranspose( track, amount );
            cout << "Track " << track << " transposed by " << amount << " semitones " << endl;
        }
        else if ( command == "tracknames")              // prints track names
        {
            cout << "\nTrack Names:" << endl;
            for (int i = 1; i < sequencer.GetNumTracks(); i++)
            {
                cout << "Track " << i << ": " << sequencer.GetTrackName( i ) << endl;

            }
        }
        else if ( command == "b")                       // goes a measure backward
        {
            int meas = sequencer.GetMeasure();
            if ( meas > 0 )
            {
                sequencer.GoToMeasure(--meas);
            }
            cout << "Actual position: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "f")                       // goes a measure forward
        {
            int meas = sequencer.GetMeasure();
            if ( meas < sequencer.GetNumMeasures() )
            {
                sequencer.GoToMeasure(++meas);
            }
            cout << "Actual position: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "help")                    // prints help screen
        {
            cout << helpstring;
        }
        else if ( command != "quit" )                   // exits
        {
            cout << "Unrecognized command" << endl;
        }
    }
    return EXIT_SUCCESS;
}

