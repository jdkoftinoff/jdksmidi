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
#include "jdksmidi/filewrite.h"
#include "jdksmidi/filewritemultitrack.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/track.h"

namespace jdksmidi {

MIDIFileWriteMultiTrack::MIDIFileWriteMultiTrack(
    MIDIMultiTrack const* mlt_, MIDIFileWriteStream* strm_)
    : multitrack(mlt_)
    , writer(strm_)
{}

MIDIFileWriteMultiTrack::~MIDIFileWriteMultiTrack() = default;

bool MIDIFileWriteMultiTrack::write(int num_tracks, int division)
{
    bool f = true;

    if (!pre_write()) {
        return false;
    }

    // first, write the header.
    writer.write_file_header((num_tracks > 0), num_tracks, division);
    // now write each track

    for (int i = 0; i < num_tracks; ++i) {
        if (writer.error_occurred()) {
            f = false;
            break;
        }

        auto t = multitrack->get_track(i);

        writer.write_track_header(0);  // will be rewritten later

        if (t) {
            for (int event_num = 0; event_num < t->get_num_events(); ++event_num) {
                auto ev = t->get_event_address(event_num);

                if (ev && !ev->is_no_op()) {
                    if (!ev->is_data_end()) {
                        writer.write_event(*ev);

                        if (writer.error_occurred()) {
                            f = false;
                            break;
                        }
                    }
                }
            }
        }

        writer.write_end_of_track(0);
        writer.rewrite_track_length();
    }

    if (!post_write()) {
        return false;
    }

    return f;
}

bool MIDIFileWriteMultiTrack::pre_write()
{
    return true;
}

bool MIDIFileWriteMultiTrack::post_write()
{
    return true;
}

}  // namespace jdksmidi
