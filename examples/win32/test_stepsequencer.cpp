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

#include "test_stepsequencer.h"

#include <iostream>
#include <string>
using namespace std;

//
// globals
//

string command_buf, command, par1, par2, par3;        // used by GetCommand() for parsing the user input
MIDISequencerGUIEventNotifierText notifier( stdout ); // a text notifier: send messages to stdout
AdvancedSequencer sequencer( &notifier );             // an AdvancedSequencer
MIDIMultiTrack *multitrack = sequencer.GetMultiTrack();
// our multitrack which will be edited
position cur_pos( multitrack ); // the cursor position
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

    for ( size_t i = 0; i < command_buf.size(); ++i )
    {
        command_buf[i] = tolower( command_buf[i] );
    }

    pos1 = command_buf.find_first_not_of( " " );
    pos2 = command_buf.find_first_of( " ", pos1 + 1 );
    if ( pos1 == string::npos )
    {
        return;
    }
    command = command_buf.substr( pos1, pos2 - pos1 );

    pos1 = command_buf.find_first_not_of( " ", pos2 );
    pos2 = command_buf.find_first_of( " ", pos1 + 1 );
    if ( pos1 == string::npos )
    {
        return;
    }
    par1 = command_buf.substr( pos1, pos2 - pos1 );

    pos1 = command_buf.find_first_not_of( " ", pos2 );
    pos2 = command_buf.find_first_of( " ", pos1 + 1 );
    if ( pos1 == string::npos )
    {
        return;
    }
    par2 = command_buf.substr( pos1, pos2 - pos1 );

    pos1 = command_buf.find_first_not_of( " ", pos2 );
    pos2 = command_buf.find_first_of( " ", pos1 + 1 );
    if ( pos1 == string::npos )
    {
        return;
    }
    par3 = command_buf.substr( pos1, pos2 - pos1 );
}

unsigned char NameToValue( string s )
// Converts a string as "C6" or "a#5" into corresponding MIDI note value
{
    static const unsigned char noteoffsets[7] = {9, 11, 0, 2, 4, 5, 7, };

    int p = s.find_first_not_of( " " );
    char ch = tolower( s[p] );
    unsigned char note;
    unsigned char octave;
    if ( string( "abcdefg" ).find( ch ) == string::npos )
        return 0;
    note = noteoffsets[ch - 'a'];
    p = s.find_first_not_of( " ", p + 1 );
    if ( s[p] == '#' )
    {
        note++;
        p = s.find_first_not_of( " ", p + 1 );
    }
    else if ( s[p] == 'b' )
    {
        note--;
        p = s.find_first_not_of( " ", p + 1 );
    }
    if ( string( "0123456789" ).find( s[p] ) == string::npos )
        return 0;
    octave = s[p] - '0';
    return 12 * octave + note;
}

void DumpMIDIMultiTrack( MIDIMultiTrack *mlt, int trk = -1 )
// shows the MIDIMultiTrack content
{
    MIDIMultiTrackIterator i( mlt );
    const MIDITimedBigMessage *msg;
    char s[200];
    fprintf( stdout, "Clocks per beat: %d\n\n", mlt->GetClksPerBeat() );
    if ( trk != -1 )
        fprintf( stdout, "Dump of track %d\n", trk );
    i.GoToTime( 0 );

    int num_lines = 0;
    do
    {
        int trk_num;

        if ( i.GetCurEvent( &trk_num, &msg ) && ( trk_num == trk || trk == -1 ) )
        {
            fprintf( stdout, "#%2d - %6ld - ", trk_num, msg->GetTime() );
            msg->MsgToText( s );
            fprintf( stdout, s );
            fprintf( stdout, "\n" );
        }
        num_lines++;
        if ( num_lines == 100 )
        {
            system( "PAUSE" );
            num_lines = 0;
        }
    } while ( i.GoToNextEvent() );
}

void PrintResolution()
// prints info about current position and step size
{
    cout << "MultiTrack resolution is " << multitrack->GetClksPerBeat() << " clocks per beat" << endl;
    cout << "Current step size is " << cur_pos.getstep() << " clocks" << endl;
}

int main( int argc, char **argv )
{
    MIDITimedBigMessage msg;
    MIDITrack *trk = multitrack->GetTrack( cur_pos.gettrack() );
    MIDIClockTime last_note_length = 120;
    int last_note_vel = 100;
    int event_num;

    *filename = 0;
    multitrack->SetClksPerBeat( 120 );
    cur_pos.setstep( 120 );
    cout << "Step sequencer example for jdksmidi library" << endl << "Copyright 2014 Nicola Cassetta" << endl << endl;
    PrintResolution();
    cout << endl << "TYPE help TO GET A LIST OF AVAILABLE COMMANDS" << endl << endl;

    // The main loop gets a command from the user and then execute it by mean of the AdvancedSequencer and
    // MIDIMultiTrack methods

    while ( command != "quit" ) // main loop
    {
        cout << "*** Current cursor pos: Track: " << cur_pos.gettrack() << " Time: " << cur_pos.gettime() << endl;
        GetCommand(); // gets user input and parse it

        if ( command == "load" ) // loads a file
        {
            if ( sequencer.Load( par1.c_str() ) )
            {
                cout << "Loaded file " << par1 << endl;
                PrintResolution();
                strcpy( filename, par1.c_str() );
            }
            else
            {
                cout << "Error loading file" << endl;
            }
        }

        else if ( command == "save" ) // saves current file
        {
            if ( par1.length() > 0 )
            {
                strcpy( filename, par1.c_str() );
            }
            if ( strlen( filename ) == 0 )
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

        else if ( command == "outport" ) // changes the midi out port
        {
            int port = atoi( par1.c_str() );
            sequencer.SetOutputPort( port );
            cout << "Assigned out port n. " << sequencer.GetOutputPort();
        }

        else if ( command == "play" ) // starts playback
        {
            sequencer.GoToTime( cur_pos.gettime() );
            // this has no effect if cur_pos is after the sequencer last event
            sequencer.Play();
        }

        else if ( command == "stop" ) // stops playback
        {
            sequencer.Stop();
        }

        else if ( command == "dump" ) // prints a dump of the sequencer contents
        {
            int dump_trk = ( par1.length() == 0 ? -1 : atoi( par1.c_str() ) );
            DumpMIDIMultiTrack( sequencer.GetMultiTrack(), dump_trk );
        }

        else if ( command == "goto" ) // goes to meas and beat
        {
            int measure = atoi( par1.c_str() ) - 1;
            int beat = ( par2.length() == 0 ? 0 : atoi( par2.c_str() ) - 1 );
            if ( measure < 0 || measure > sequencer.GetNumMeasures() - 1 )
            {
                cout << "Invalid position" << endl;
            }
            else
            {
                sequencer.GoToMeasure( measure, beat );
                cur_pos.settime( sequencer.GetCurrentMIDIClockTime() );
            }
        }

        else if ( command == "<<" ) // rewind
        {
            cur_pos.rewind();
        }

        else if ( command == "<" ) // step backward
        {
            int steps = ( par1.length() == 0 ? 1 : atoi( par1.c_str() ) );
            for ( int i = 0; i < steps; i++ )
            {
                cur_pos.stepback();
            }
        }

        else if ( command == ">" ) // step forward
        {
            int steps = ( par1.length() == 0 ? 1 : atoi( par1.c_str() ) );
            for ( int i = 0; i < steps; i++ )
            {
                cur_pos.stepforward();
            }
        }

        else if ( command == "t<" ) // previous track
        {
            cur_pos.previoustrack();
            trk = multitrack->GetTrack( cur_pos.gettrack() );
        }

        else if ( command == "t>" ) // next track
        {
            cur_pos.nexttrack();
            trk = multitrack->GetTrack( cur_pos.gettrack() );
        }

        else if ( command == "step" ) // sets the step size
        {
            cur_pos.setstep( atoi( par1.c_str() ) );
            PrintResolution();
        }

        else if ( command == "note" ) // inserts a note event
        {
            msg.SetTime( cur_pos.gettime() );
            if ( par2 != "*" )
            {
                int vel = ( par2.length() == 0 ? last_note_vel : atoi( par2.c_str() ) );
                MIDIClockTime len = ( par3.length() == 0 ? last_note_length : atoi( par3.c_str() ) );
                msg.SetNoteOn( cur_pos.gettrack() - 1, NameToValue( par1 ), vel );
                trk->InsertNote( msg, len );
            }
            else
            {
                msg.SetNoteOn( cur_pos.gettrack() - 1, atoi( par1.c_str() ), 100 );
                if ( !trk->FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
                    cout << "Event not found" << endl;
                else
                {
                    msg = *trk->GetEvent( event_num );
                    trk->DeleteNote( msg );
                }
            }
            sequencer.SetChanged();
        }

        else if ( command == "volume" ) // inserts a volume event
        {
            msg.SetTime( cur_pos.gettime() );
            if ( par1 != "*" )
            {
                msg.SetControlChange( cur_pos.gettrack() - 1, C_MAIN_VOLUME, atoi( par1.c_str() ) );
                trk->InsertEvent( msg );
            }
            else
            {
                msg.SetControlChange( cur_pos.gettrack() - 1, C_MAIN_VOLUME, 0 );
                if ( !trk->FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
                    cout << "Event not found" << endl;
                else
                {
                    msg = *trk->GetEvent( event_num );
                    trk->DeleteEvent( msg );
                }
            }
            sequencer.SetChanged();
        }

        else if ( command == "pan" ) // inserts a pan event
        {
            msg.SetTime( cur_pos.gettime() );
            if ( par1 != "*" )
            {
                msg.SetControlChange( cur_pos.gettrack() - 1, C_PAN, atoi( par1.c_str() ) );
                trk->InsertEvent( msg );
            }
            else
            {
                msg.SetControlChange( cur_pos.gettrack() - 1, C_PAN, 0 );
                if ( !trk->FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
                    cout << "Event not found" << endl;
                else
                {
                    msg = *trk->GetEvent( event_num );
                    trk->DeleteEvent( msg );
                }
            }
            sequencer.SetChanged();
        }

        else if ( command == "control" ) // inserts a generic control event
        {
            msg.SetTime( cur_pos.gettime() );
            if ( par2 != "*" )
            {
                msg.SetControlChange( cur_pos.gettrack() - 1, atoi( par1.c_str() ), atoi( par2.c_str() ) );
                trk->InsertEvent( msg );
            }
            else
            {
                msg.SetControlChange( cur_pos.gettrack() - 1, atoi( par1.c_str() ), 0 );
                if ( !trk->FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
                    cout << "Event not found" << endl;
                else
                {
                    msg = *trk->GetEvent( event_num );
                    trk->DeleteEvent( msg );
                }
            }
            sequencer.SetChanged();
        }

        else if ( command == "patch" ) // inserts a patch event
        {
            msg.SetTime( cur_pos.gettime() );
            if ( par1 != "*" )
            {
                msg.SetProgramChange( cur_pos.gettrack() - 1, atoi( par1.c_str() ) );
                trk->InsertEvent( msg );
            }
            else
            {
                msg.SetProgramChange( cur_pos.gettrack() - 1, 0 );
                if ( !trk->FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
                    cout << "Event not found" << endl;
                else
                {
                    msg = *trk->GetEvent( event_num );
                    trk->DeleteEvent( msg );
                }
            }
            sequencer.SetChanged();
        }

        else if ( command == "tempo" ) // inserts a tempo event in track 0
        {
            msg.SetTime( cur_pos.gettime() );
            if ( par1 != "*" )
            {
                msg.SetTempo( atoi( par1.c_str() ) );
                multitrack->GetTrack( 0 )->InsertEvent( msg );
            }
            else
            {
                msg.SetTempo( 120 );
                if ( !multitrack->GetTrack( 0 )->FindEventNumber( msg, &event_num, COMPMODE_SAMEKIND ) )
                    cout << "Event not found" << endl;
                else
                {
                    msg = *multitrack->GetTrack( 0 )->GetEvent( event_num );
                    trk->DeleteEvent( msg );
                }
            }
            sequencer.SetChanged();
        }

        else if ( command == "help" ) // prints help screen
        {
            cout << helpstring;
        }

        else if ( command != "quit" ) // unrecognized
        {
            cout << "Unrecognized command" << endl;
        }
    }

    return EXIT_SUCCESS;
}
