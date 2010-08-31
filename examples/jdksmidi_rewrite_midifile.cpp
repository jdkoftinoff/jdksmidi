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

int main ( int argc, char **argv )
{
    int return_code = -1;

    if ( argc > 2 )
    {
        const char *infile_name = argv[1];
        const char *outfile_name = argv[2];
        // the stream used to read the input file
        jdksmidi::MIDIFileReadStreamFile rs ( infile_name );
        // the object which will hold all the tracks
        jdksmidi::MIDIMultiTrack tracks;
        // the object which loads the tracks into the tracks object
        jdksmidi::MIDIFileReadMultiTrack track_loader ( &tracks );
        // the object which parses the midifile and gives it to the multitrack loader
        jdksmidi::MIDIFileRead reader ( &rs, &track_loader );
        // load the midifile into the multitrack object
        reader.Parse();
        // create the output stream
        jdksmidi::MIDIFileWriteStreamFileName out_stream ( outfile_name );

        if ( out_stream.IsValid() )
        {
            // the object which takes the midi tracks and writes the midifile to the output stream
            jdksmidi::MIDIFileWriteMultiTrack writer (
                &tracks,
                &out_stream
            );
            // extract the original multitrack division and number of tracks
            int num_tracks = reader.GetNumberTracks();
            int division = reader.GetDivision();

            // write the output file
            if ( writer.Write ( num_tracks, division ) )
            {
                return_code = 0;
            }

            else
            {
                fprintf ( stderr, "Error writing file '%s'\n", outfile_name );
            }
        }

        else
        {
            fprintf ( stderr, "Error opening file '%s'\n", outfile_name );
        }
    }

    else
    {
        fprintf ( stderr, "usage:\n\tjdksmidi_rewrite_midifile INFILE.mid OUTFILE.mid\n" );
    }

    return return_code;
}
