/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

#include "jdksmidi/advancedsequencer.h"
using namespace jdksmidi;

#include <iostream>
#include <string>
using namespace std;


string command_buf, command, par1, par2;
AdvancedSequencer sequencer;        // a sequencer without GUI notifier

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
    while ( command != "quit" )
    {
        GetCommand();

        if( command == "load" )
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
        else if ( command == "ports")
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
        else if ( command == "outport")
        {
            int port = atoi( par1.c_str() );
            sequencer.SetOutputPort( port );
            cout << "Assigned out port n. " << sequencer.GetOutputPort();
        }
        else if ( command == "inport" )
        {
            int port = atoi( par1.c_str() );
            sequencer.SetInputPort( port );
            cout << "Assigned in port n. " << sequencer.GetInputPort();
        }
        else if ( command == "play")
        {
            sequencer.Play();
            cout << "Sequencer started at measure: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "loop" )
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
        else if (command == "stop")
        {
            sequencer.Stop();
            cout << "Sequencer stopped at measure: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "rew")
        {
            sequencer.GoToZero();
            cout << "Rewind to 0:0" << endl;
        }
        else if ( command == "goto")
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
        else if ( command == "dump")
        {
            DumpMIDIMultiTrack( sequencer.GetMultiTrackAddress() );
        }
        else if ( command == "solo")
        {
            int track = atoi( par1.c_str() );
            sequencer.SoloTrack( track );
            cout << "Soloed track " << track << endl;
        }
        else if ( command == "unsolo")
        {
            sequencer.UnSoloTrack();
            cout << "Unsoloed all tracks" << endl;
        }
        else if ( command == "mute")
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
        else if ( command == "unmute")
        {
            sequencer.UnmuteAllTracks();
            cout << "Unmuted all tracks" << endl;
        }
        else if ( command == "tscale")
        {
            int scale = atoi( par1.c_str() );
            sequencer.SetTempoScale( (float)scale / 100 );
            cout << "Tempo scale : " << scale << "%  " <<
                    " Effective tempo: " << sequencer.GetTempoWithScale() << "bpm" << endl;
        }
        else if ( command == "vscale" )
        {
            int track = atoi( par1.c_str() );
            int scale = atoi( par2.c_str() );
            sequencer.SetTrackVelocityScale( track, scale );
            cout << "Track " << track << " velocity scale set to " << scale << "%" << endl;
        }
        else if ( command == "trans" )
        {
            int track = atoi( par1.c_str() );
            int amount = atoi( par2.c_str() );
            sequencer.SetTrackTranspose( track, amount );
            cout << "Track " << track << " transposed by " << amount << " semitones " << endl;
        }
        else if ( command == "tracknames")
        {
            cout << "\nTrack Names:" << endl;
            for (int i = 1; i < sequencer.GetNumTracks(); i++)
            {
                cout << "Track " << i << ": " << sequencer.GetTrackName( i ) << endl;

            }
        }
        else if ( command == "b")
        {
            int meas = sequencer.GetMeasure();
            if ( meas > 0 )
            {
                sequencer.GoToMeasure(--meas);
            }
            cout << "Actual position: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "f")
        {
            int meas = sequencer.GetMeasure();
            if ( meas < sequencer.GetNumMeasures() )
            {
                sequencer.GoToMeasure(++meas);
            }
            cout << "Actual position: " << sequencer.GetMeasure() << ":"
                 << sequencer.GetBeat() << endl;
        }
        else if ( command == "help")
        {
            cout << helpstring;
        }
        else if ( command != "quit" )
        {
            cout << "Unrecognized command" << endl;
        }
    }
    return EXIT_SUCCESS;
}

