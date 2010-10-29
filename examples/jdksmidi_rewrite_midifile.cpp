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

#ifdef WIN32
#include <windows.h>
#endif

#include "jdksmidi/world.h"
#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/filewritemultitrack.h"
using namespace jdksmidi;

#include <iostream>
using namespace std;

// delete all text events from multitrack object
void DeleteAllTracksText( MIDIMultiTrack &tracks )
{
  int num_tracks = tracks.GetNumTracksWithEvents();

  for ( int nt = 0; nt < num_tracks; ++nt )
  {
    MIDITrack &trk = *tracks.GetTrack( nt );
    int num_events = trk.GetNumEvents();

    for ( int ne = 0; ne < num_events; ++ne )
    {
      MIDITimedBigMessage *msg = trk.GetEvent( ne );
      // convert any text midi event to META_NO_OPERATION event
      if ( msg->IsTextEvent() ) trk.MakeEventNoOp( ne );
    }
  }
}

void args_err()
{
   cerr << "usage:\n\tjdkmidi_rewrite_midifile INFILE.mid OUTFILE.mid [any arg for delete text]\n";
}

int main ( int argc, char **argv )
{
  int return_code = -1;

  if ( argc > 1 )
  {
    const char *infile_name = argv[1];

    // the stream used to read the input file
    MIDIFileReadStreamFile rs ( infile_name );
    if ( !rs.IsValid() )
    {
      cerr << "\nError opening file " << infile_name << endl;
      return return_code;
    }

    if ( argc <= 2 )
    {
      args_err();
      return return_code;
    }

    const char *outfile_name = argv[2];

    // the multitrack object which will hold all the tracks
    MIDIMultiTrack tracks( 1 ); // only 1 track in multitrack object - not enough for midifile format 1

    // the object which loads the tracks into the tracks object
    MIDIFileReadMultiTrack track_loader ( &tracks );

    // the object which parses the midifile and gives it to the multitrack loader
    MIDIFileRead reader ( &rs, &track_loader );

    // increase amount of of tracks if tracks.num_tracks < midifile_num_tracks
    int midifile_num_tracks = reader.ReadNumTracks();
    tracks.ExpandIfLess( midifile_num_tracks );

    // load the midifile into the multitrack object
    if ( !reader.Parse() )
    {
      cerr << "\nError Parse file " << infile_name << endl;
      return return_code;
    }

    // delete all text events if exist any argv[3]
    if ( argc > 3 ) DeleteAllTracksText( tracks );

    // create the output stream
    MIDIFileWriteStreamFileName out_stream ( outfile_name );

    if ( out_stream.IsValid() )
    {
      // the object which takes the midi tracks and writes the midifile to the output stream
      MIDIFileWriteMultiTrack writer ( &tracks, &out_stream );

      // extract the original multitrack division
      int division = reader.GetDivision();
      
      // get really number of track, used in reader.Parse() process
      int num_tracks = tracks.GetNumTracksWithEvents();

      // write the output midi file
      if ( writer.Write ( num_tracks, division ) )
      {
        cout << "\nAll OK. Number of tracks with events " << num_tracks << endl;
        return_code = 0;
      }
      else
        cerr << "\nError writing file " << outfile_name << endl;
    }
    else
      cerr << "\nError opening file " << outfile_name << endl;
  }
  else
    args_err();
  
  return return_code;
}

