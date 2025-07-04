/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004-2025  Jeffrey Koftinoff
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE MIT LICENSE ***
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifdef WIN32
#    include <windows.h>
#endif

#include "jdksmidi/fileread.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/filewritemultitrack.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/track.h"
#include "jdksmidi/world.h"

int main(int argc, char** argv)
{
    int return_code = -1;

    if (argc > 2) {
        char const* infile_name = argv[1];
        char const* outfile_name = argv[2];
        // the stream used to read the input file
        jdksmidi::MIDIFileReadStreamFile rs(infile_name);
        // the object which will hold all the tracks
        jdksmidi::MIDIMultiTrack tracks;
        // the object which loads the tracks into the tracks object
        jdksmidi::MIDIFileReadMultiTrack track_loader(&tracks);
        // the object which parses the midifile and gives it to the multitrack loader
        jdksmidi::MIDIFileRead reader(&rs, &track_loader);
        // load the midifile into the multitrack object
        reader.parse();
        // create the output stream
        jdksmidi::MIDIFileWriteStreamFileName out_stream(outfile_name);

        if (out_stream.is_valid()) {
            // the object which takes the midi tracks and writes the midifile to the output stream
            jdksmidi::MIDIFileWriteMultiTrack writer(&tracks, &out_stream);
            // extract the original multitrack division and number of tracks
            int num_tracks = reader.get_number_tracks();
            int division = reader.get_division();

            // write the output file
            if (writer.write(num_tracks, division)) {
                return_code = 0;
            }

            else {
                fprintf(stderr, "Error writing file '%s'\n", outfile_name);
            }
        }

        else {
            fprintf(stderr, "Error opening file '%s'\n", outfile_name);
        }
    }

    else {
        fprintf(stderr, "usage:\n\tjdksmidi_rewrite_midifile INFILE.mid OUTFILE.mid\n");
    }

    return return_code;
}
