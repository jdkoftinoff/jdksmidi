/*

  Rewrite Midifile  example for  libJDKSmidi C++ MIDI Library

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

// delete all text events from multitrack object
bool DeleteAllTracksText( MIDIMultiTrack &tracks )
{
    bool text_deleted = false;
    int num_tracks = tracks.GetNumTracksWithEvents();

    for ( int nt = 0; nt < num_tracks; ++nt )
    {
        MIDITrack &trk = *tracks.GetTrack( nt );
        int num_events = trk.GetNumEvents();

        for ( int ne = 0; ne < num_events; ++ne )
        {
            MIDITimedBigMessage *msg = trk.GetEvent( ne );
            // convert any text midi event to NoOp event
            if ( msg->IsTextEvent() )
            {
                trk.MakeEventNoOp( ne );
                text_deleted = true;
            }
        }
    }

    return text_deleted;
}

void args_err()
{
    cerr << "\nusage:  jdkmidi_rewrite_midifile  INFILE.mid  OUTFILE.mid  ['1' for reduce outfile size]\n";
    cerr <<   "                                                           ['2' for delete start pause]\n\n";
}

int main ( int argc, char **argv )
{
    int retcode = -1;

    if ( argc <= 2 )
    {
        args_err();
        return retcode;
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];

    int mode = 0;
    if ( argc > 3 )
        mode = abs ( atol( argv[3] ) );

    MIDIMultiTrack tracks, tracks2;

    if ( !ReadMidiFile( infile, tracks ) )
    {
        cerr << "\nError reading file " << infile << endl;
        return retcode;
    }

    if ( mode%2 == 1 ) // need to reduce outfile size
    {
        // delete all text events from all tracks
        if ( DeleteAllTracksText( tracks ) )
        {
          cout << "\nAll midi text events deleted." << endl;
        }

        if ( tracks.GetNumTracksWithEvents() == 1 )
        {
            // remake multitrack object and optimize new tracks content:
            // move all channal events to tracks 1-16, and all other types of events to track 0
            // and reduce midifile size because of increase number of events with running status
            tracks.AssignEventsToTracks( 0 );
            cout << "\nAll midi channal events moved to tracks 1-16." << endl;
        }
    }

    MIDIMultiTrack *outtracks = &tracks;
    if ( mode >= 2 ) // need to delete start pause
    {
        CompressStartPause( tracks, tracks2 );
        outtracks = &tracks2;
        cout << "\nStart pause deleted (decreased)." << endl;
    }

    if ( WriteMidiFile( *outtracks, outfile) )
    {
        int num_tracks = outtracks->GetNumTracksWithEvents();
        cout << "\nAll OK. Number of tracks with events " << num_tracks << endl;
        retcode = 0;
    }
    else
    {
        cerr << "\nError writing file " << outfile << endl;
    }

    return retcode;
}

