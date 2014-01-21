/*

  Example using the class MIDIMultiTrack for libJDKSmidi C++
  MIDI Library (console app, no GUI!)

  Copyright (C) 2014 N.Cassetta
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

#include "test_stepsequencer.h"

#include <iostream>
#include <string>
using namespace std;


//
// globals
//

string command_buf, command, par1, par2, par3;  // used by GetCommand() for parsing the user input
MIDISequencerGUIEventNotifierText notifier(stdout); // a text notifier: send messages to stdout
AdvancedSequencer sequencer(&notifier);         // an AdvancedSequencer
MIDIMultiTrack* multitrack = sequencer.GetMultiTrackAddress();
                                                // our multitrack which will be edited
position cur_pos(multitrack);                   // the cursor position
char filename[200];



void GetCommand()
// gets from the user the string command_buf, then parses it
// dividing it into command, par1, par2 and par3 substrings
{
    size_t pos1, pos2;

    cout << "\n=> ";
    getline( cin, command_buf );

    command = "";
    par1 = "";
    par2 = "";
    par3 = "";

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

    pos1 = command_buf.find_first_not_of ( " ", pos2);
    pos2 = command_buf.find_first_of (" ", pos1+1);
    if ( pos1 == string::npos )
    {
        return;
    }
    par3 = command_buf.substr (pos1, pos2 - pos1);
}


void DumpMIDIMultiTrack( MIDIMultiTrack *mlt )
// shows the MIDIMultiTrack content
{
    MIDIMultiTrackIterator i ( mlt );
    const MIDITimedBigMessage *msg;
    char s[200];
    fprintf ( stdout , "Clocks per beat: %d\n\n", mlt->GetClksPerBeat() );
    i.GoToTime ( 0 );

    int num_lines = 0;
    do
    {
        int trk_num;

        if ( i.GetCurEvent ( &trk_num, &msg ) )
        {
            fprintf ( stdout, "#%2d - %6ld - ", trk_num, msg->GetTime() );
            msg->MsgToText(s);
            fprintf ( stdout, s);
            fprintf ( stdout, "\n" );
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
    MIDITimedBigMessage msg;

    *filename = 0;
    multitrack->SetClksPerBeat( 120 );
    cur_pos.setstep ( 120 );
    cout << "Step sequencer example for jdksmidi library" << endl <<
            "Copyright 2014 Nicola Cassetta" << endl << endl <<
            "MultiTrack resolution is 120 clocks per beat" << endl <<
            "Current step size is 120 clocks" << endl << endl <<
            "TYPE help TO GET A LIST OF AVAILABLE COMMANDS" << endl << endl;


    // The main loop gets a command from the user and then execute it by mean of the AdvancedSequencer and
    // MIDIMultiTrack methods

    while ( command != "quit" )                     // main loop
    {
        cout << "Current cursor pos: Track: " << cur_pos.gettrack() << " Time: " << cur_pos.gettime() <<
                "  ---  Sequencer start time: " << sequencer.GetCurrentMIDIClockTime() << endl;
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

        else if ( command == "save")                    // saves current file
        {
            if ( par1.length() > 0 )
            {
                strcpy (filename, par1.c_str() );
            }
            if (strlen( filename ) == 0 )
            {
                cout << "File name not defined" << endl;
            }
            else
            {
                if ( WriteMidiFile( *multitrack, filename ) )
                {
                    cout << "File saved" << endl;
                }
                else
                {
                    cout << "Error writing file" << endl;
                }
            }
        }

        else if ( command == "outport")                 // changes the midi out port
        {
            int port = atoi( par1.c_str() );
            sequencer.SetOutputPort( port );
            cout << "Assigned out port n. " << sequencer.GetOutputPort();
        }

        else if ( command == "play")                    // starts playback
        {
            sequencer.Play();
            cout << "Sequencer started at measure: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }

        else if (command == "stop")                     // stops playback
        {
            sequencer.Stop();
            cout << "Sequencer stopped at measure: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
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
                cur_pos.settime( sequencer.GetCurrentMIDIClockTime() );
            }
        }

        else if ( command == "dump")                    // prints a dump of the sequencer contents
        {
            DumpMIDIMultiTrack( sequencer.GetMultiTrackAddress() );
        }

        else if ( command == "tempo")                   // inserts a tempo event in track 0
        {
            msg.SetTempo( atoi ( par1.c_str() ) );
            msg.SetTime ( cur_pos.gettime() );
            multitrack->GetTrack( 0 )->Insert( msg );
            sequencer.SetMltChanged();
        }

        else if ( command == "volume")                  // inserts a volume event
        {
            msg.SetControlChange( cur_pos.gettrack()-1, C_MAIN_VOLUME, atoi(par1.c_str()) );
            msg.SetTime( cur_pos.gettime() );
            multitrack->GetTrack( cur_pos.gettrack() )->Insert (msg);
            sequencer.SetMltChanged();
        }

        else if ( command == "pan")                     // inserts a pan event
        {
            msg.SetControlChange( cur_pos.gettrack()-1, C_PAN, atoi(par1.c_str()) );
            msg.SetTime( cur_pos.gettime() );
            multitrack->GetTrack( cur_pos.gettrack() )->Insert (msg);
            sequencer.SetMltChanged();
        }

        else if ( command == "control")                 // inserts a generic control event
        {
            msg.SetControlChange( cur_pos.gettrack()-1, atoi(par1.c_str()), atoi(par2.c_str()) );
            msg.SetTime( cur_pos.gettime() );
            multitrack->GetTrack( cur_pos.gettrack() )->Insert (msg);
            sequencer.SetMltChanged();
        }

        else if ( command == "patch")                   // inserts a patch event
        {
            msg.SetProgramChange( cur_pos.gettrack()-1, atoi(par1.c_str()) );
            msg.SetTime( cur_pos.gettime() );
            multitrack->GetTrack( cur_pos.gettrack() )->Insert (msg);
            sequencer.SetMltChanged();
        }

        else if ( command == "note")                    // inserts a note event
        {
            msg.SetNoteOn( cur_pos.gettrack()-1, atoi(par1.c_str()), atoi(par2.c_str()) );
            msg.SetTime( cur_pos.gettime() );
            multitrack->GetTrack( cur_pos.gettrack() )->Insert (msg);
            msg.SetNoteOff( cur_pos.gettrack()-1, atoi(par1.c_str()), 0);
            msg.SetTime ( cur_pos.gettime() + atoi(par3.c_str()));
            multitrack->GetTrack( cur_pos.gettrack() )->Insert (msg);
            sequencer.SetMltChanged();
        }

        else if ( command == "<<")                      // rewind
        {
            cur_pos.rewind();
            sequencer.GoToZero();
                // this has no effect if cur time is after sequencer end time
        }

        else if ( command == "<")                       // step backward
        {
            cur_pos.stepback();
            sequencer.GoToTime( cur_pos.gettime() );
                // this has no effect if cur time is after sequencer end time
        }

        else if ( command == ">")                       // step forward
        {
            cur_pos.stepforward();
            sequencer.GoToTime( cur_pos.gettime() );
        }

        else if ( command == "t<")                      // previous track
        {
            cur_pos.previoustrack();
        }

        else if ( command == "t>")                      // next track
        {
            cur_pos.nexttrack();
        }

        else if ( command == "step")                    // sets the step size
        {
            cur_pos.setstep(atoi(par1.c_str()));
            cout << "Step size set to " << cur_pos.getstep() << endl;
        }
        else if ( command == "help")                    // prints help screen
        {
            cout << helpstring;
        }

        else if ( command != "quit" )                   // unrecognized
        {
            cout << "Unrecognized command" << endl;
        }
    }

    return EXIT_SUCCESS;
}

