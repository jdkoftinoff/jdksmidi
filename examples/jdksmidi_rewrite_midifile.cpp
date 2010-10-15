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


// delete all text events from multitrack object
void DeleteAllTracksText( MIDIMultiTrack &tracks ) // func by VRM@
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


int main ( int argc, char **argv )
{
  int return_code = -1;

  if ( argc > 2 )
  {
    const char *infile_name = argv[1];
    const char *outfile_name = argv[2];

    // the stream used to read the input file
    MIDIFileReadStreamFile rs ( infile_name );
    if ( !rs.IsValid() ) // VRM@
    {
      fprintf ( stderr, "Error opening file '%s'\n", infile_name );
      return return_code;
    }

    // the multitrack object which will hold all the tracks
    MIDIMultiTrack tracks;

    // the object which loads the tracks into the tracks object
    MIDIFileReadMultiTrack track_loader ( &tracks );

    // the object which parses the midifile and gives it to the multitrack loader
    MIDIFileRead reader ( &rs, &track_loader );

    // default value for optimize_tracks = true, see MIDIFileRead::MIDIFileRead()
    bool optimize_tracks = true;
    if ( argc > 3 )
    {
      optimize_tracks = atoi( argv[3] ) != 0;
      // don't change all tracks structure if optimize_tracks = false
      reader.SetOptimizeTracks( optimize_tracks ); // VRM@
    }

    // load the midifile into the multitrack object
    bool opt1 = reader.GetOptimizeTracks();
    if ( !reader.Parse() )
    {
      fprintf ( stderr, "Error parse file '%s'\n", infile_name ); // VRM@
      return return_code;
    }
    bool opt2 = reader.GetOptimizeTracks();
    if ( opt1 != opt2 ) fprintf ( stdout, "\nWarning: optimize tracks fail during '%s' parse\n", infile_name); // VRM@

    // delete all text events if exist any argv[4]
    if ( argc > 4 ) DeleteAllTracksText( tracks ); // VRM@

    // create the output stream
    MIDIFileWriteStreamFileName out_stream ( outfile_name );

    if ( out_stream.IsValid() )
    {
      // the object which takes the midi tracks and writes the midifile to the output stream
      MIDIFileWriteMultiTrack writer ( &tracks, &out_stream );

      // extract the original multitrack division
      int division = reader.GetDivision();
      
      // get really number of track, used in reader.Parse() process
      int num_tracks = tracks.GetNumTracksWithEvents(); // VRM@

      // write the output midi file
      if ( writer.Write ( num_tracks, division ) )
      {
        fprintf ( stdout, "\nNumber of tracks with events %i\n", num_tracks ); // VRM@
        return_code = 0;
      }
      else
        fprintf ( stderr, "Error writing file '%s'\n", outfile_name );
    }
    else
      fprintf ( stderr, "Error opening file '%s'\n", outfile_name );
  }
  else
    fprintf ( stderr, "usage:\n\tjdkmidi_rewrite_midifile IN.mid OUT.mid [optimize_tracks(0,1) [any arg for delete text]]\n" );
  
  return return_code;
}

