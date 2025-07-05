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

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/track.h"

#include <memory>

namespace jdksmidi {
class MIDISystemExclusive;
}

#ifndef DEBUG_MDTRACK
#    define DEBUG_MDTRACK 0
#endif

#if DEBUG_MDTRACK
#    undef DBG
#    define DBG(a) a
#endif

namespace jdksmidi {

MIDITimedBigMessage const* MIDITrackChunk::get_event_address(int event_num) const
{
    return &buf[event_num];
}

MIDITimedBigMessage* MIDITrackChunk::get_event_address(int event_num)
{
    return &buf[event_num];
}

MIDITrack::MIDITrack(int size)
{
    buf_size = 0;
    num_events = 0;

    for (auto& i : chunk)
        i = nullptr;

    if (size) {
        expand(size);
    }
}

MIDITrack::MIDITrack(MIDITrack const& t)
{
    buf_size = 0;
    num_events = 0;

    for (int i = 0; i < t.get_num_events(); ++i) {
        MIDITimedBigMessage const* src;
        src = t.get_event_address(i);
        put_event(*src);
    }
}

MIDITrack::~MIDITrack() = default;

void MIDITrack::clear()
{
    num_events = 0;
}

void MIDITrack::clear_and_merge(MIDITrack const* src1, MIDITrack const* src2)
{
    clear();
    int cur_trk1ev = 0;
    int num_trk1ev = src1->get_num_events();
    int cur_trk2ev = 0;
    int num_trk2ev = src2->get_num_events();
    MIDIClockTime last_data_end_time = 0;

    while (cur_trk1ev < num_trk1ev || cur_trk2ev < num_trk2ev) {
        // skip any NOPs on track 1
        auto ev1 = src1->get_event_address(cur_trk1ev);
        auto ev2 = src2->get_event_address(cur_trk2ev);
        bool has_ev1 = (cur_trk1ev < num_trk1ev) && ev1;
        bool has_ev2 = (cur_trk2ev < num_trk2ev) && ev2;

        if (has_ev1 && ev1->is_no_op()) {
            cur_trk1ev++;
            continue;
        }

        // skip any NOPs on track 2

        if (has_ev2 && ev2->is_no_op()) {
            cur_trk2ev++;
            continue;
        }

        // skip all data end

        if (has_ev1 && ev1->is_data_end()) {
            if (ev1->get_time() > last_data_end_time) {
                last_data_end_time = ev1->get_time();
            }

            cur_trk1ev++;
            continue;
        }

        if (has_ev2 && ev2->is_data_end()) {
            if (ev2->get_time() > last_data_end_time) {
                last_data_end_time = ev2->get_time();
            }

            cur_trk2ev++;
            continue;
        }

        if ((has_ev1 && !has_ev2)) {
            // nothing left on trk 2
            if (!ev1->is_no_op()) {
                if (ev1->get_time() > last_data_end_time) {
                    last_data_end_time = ev1->get_time();
                }

                put_event(*ev1);
                ++cur_trk1ev;
            }
        }

        else if ((!has_ev1 && has_ev2)) {
            // nothing left on trk 1
            if (!ev2->is_no_op()) {
                put_event(*ev2);
                ++cur_trk2ev;
            }
        }

        else if (has_ev1 && has_ev2) {
            int trk = 1;

            if ((ev1->get_time() <= ev2->get_time())) {
                trk = 1;
            }

            else {
                trk = 2;
            }

            if (trk == 1) {
                if (ev1->get_time() > last_data_end_time) {
                    last_data_end_time = ev1->get_time();
                }

                put_event(*ev1);
                ++cur_trk1ev;
            }

            else {
                if (ev2->get_time() > last_data_end_time) {
                    last_data_end_time = ev2->get_time();
                }

                put_event(*ev2);
                ++cur_trk2ev;
            }
        }
    }

    // put single final data end event
    MIDITimedBigMessage dataend;
    dataend.set_time(last_data_end_time);
    dataend.set_data_end();
    put_event(dataend);
}

#if 0
bool MIDITrack::insert ( int start_event, int num )
{
    // TODO: Insert
    return true;
}

bool  MIDITrack::delete ( int start_event, int num )
{
    // TODO: Delete
    return true;
}

void MIDITrack::QSort ( int left, int right )
{
    int i, j;
    MIDITimedBigMessage *x, y;
    i = left;
    j = right;
    // search for a non NOP message for our median
    int pos = ( left + right ) / 2;

    for ( ; pos <= right; ++pos )
    {
        x = get_event_address ( pos );

        if ( x && !x->is_no_op() )
            break;
    }

    if ( get_event_address ( pos )->is_no_op() )
    {
        for ( pos = ( left + right ) / 2; pos >= left; --pos )
        {
            x = get_event_address ( pos );

            if ( x && !x->is_no_op() )
                break;
        }
    }

    if ( x && x->is_no_op() )
        return;

    do
    {
        while ( MIDITimedMessage::compare_events ( *get_event_address ( i ), *x ) == 2 &&
                i < right )
            ++i;

        while ( MIDITimedMessage::compare_events ( *x, *get_event_address ( j ) ) == 2 &&
                j > left )
            --j;

        if ( i <= j )
        {
            y = *get_event_address ( i );
            *get_event_address ( i ) = *get_event_address ( j );
            *get_event_address ( j ) = y;
            ++i;
            --j;
        }
    }
    while ( i <= j );

    if ( left < j )
    {
        QSort ( left, j );
    }

    if ( i < right )
    {
        QSort ( i, right );
    }
}


void MIDITrack::Sort()
{
//
// A simple single buffer sorting algorithm.
//
// first, see if we need sorting by checking each element
// with the next. they should all be in order.
//
// if not, do qsort algorithm
    unsigned int i;
    unsigned int first_out_of_order_item = 0;

    for ( i = 0; i < num_events - 1; ++i )
    {
        first_out_of_order_item = i + 1;

        if ( MIDITimedMessage::compare_events (
                    *get_event_address ( i ),
                    *get_event_address ( first_out_of_order_item )
                ) == 1 )
            break;
    }

    if ( first_out_of_order_item >= num_events - 1 )
    {
//  return;  // no need for sort
    }

    QSort ( 0, num_events - 1 );
}

#endif

void MIDITrack::shrink()
{
    int num_chunks_used = static_cast<int>((num_events / MIDITrackChunkSize) + 1);
    int num_chunks_alloced = static_cast<int>(buf_size / MIDITrackChunkSize);

    if (num_chunks_used < num_chunks_alloced) {
        for (int i = num_chunks_used; i < num_chunks_alloced; ++i) {
            chunk[i].reset();
        }

        buf_size = num_chunks_used * MIDITrackChunkSize;
    }
}

bool MIDITrack::expand(int increase_amount)
{
    int num_chunks_to_expand = static_cast<int>((increase_amount / MIDITrackChunkSize) + 1);
    int num_chunks_alloced = static_cast<int>(buf_size / MIDITrackChunkSize);
    int new_last_chunk_num = static_cast<int>(num_chunks_to_expand + num_chunks_alloced);

    if (new_last_chunk_num >= MIDIChunksPerTrack) {
        return false;
    }

    for (int i = num_chunks_alloced; i < new_last_chunk_num; ++i) {
        chunk[i] = std::make_unique<MIDITrackChunk>();

        if (!chunk[i]) {
            buf_size = (i - 1) * MIDITrackChunkSize;
            return false;
        }
    }

    buf_size = new_last_chunk_num * MIDITrackChunkSize;
    return true;
}

MIDITimedBigMessage* MIDITrack::get_event_address(int event_num)
{
    return chunk[event_num / (MIDITrackChunkSize)]->get_event_address(
        (event_num % MIDITrackChunkSize));
}

MIDITimedBigMessage const* MIDITrack::get_event_address(int event_num) const
{
    return chunk[event_num / (MIDITrackChunkSize)]->get_event_address(
        (event_num % MIDITrackChunkSize));
}

bool MIDITrack::put_event(MIDITimedBigMessage const& msg)
{
    if (num_events >= buf_size) {
        if (!expand())
            return false;
    }

    get_event_address(num_events++)->copy(msg);
    return true;
}

bool MIDITrack::put_event(MIDITimedMessage const& msg, MIDISystemExclusive* sysex)
{
    if (num_events >= buf_size) {
        if (!expand())
            return false;
    }

    MIDITimedBigMessage* e = get_event_address(num_events);
    e->copy(msg);
    e->copy_sys_ex(sysex);
    ++num_events;
    return true;
}

bool MIDITrack::get_event(int event_num, MIDITimedBigMessage* msg) const
{
    if (event_num >= num_events) {
        return false;
    }

    else {
        msg->copy(*get_event_address(event_num));
        return true;
    }
}

bool MIDITrack::set_event(int event_num, MIDITimedBigMessage const& msg)
{
    if (event_num >= num_events) {
        return false;
    }

    else {
        get_event_address(event_num)->copy(msg);
        return true;
    }
}

bool MIDITrack::make_event_no_op(int event_num)
{
    if (event_num >= num_events) {
        return false;
    }

    else {
        MIDITimedBigMessage* ev = get_event_address(event_num);

        if (ev) {
            ev->clear_sys_ex();
            ev->set_no_op();
        }

        return true;
    }
}

bool MIDITrack::find_event_number(MIDIClockTime time, int* event_num) const
{
    // TO DO: try make this a binary search

    for (int i = 0; i < num_events; ++i) {
        MIDITimedBigMessage const* msg = get_event_address(i);

        if (msg->get_time() >= time) {
            *event_num = i;
            return true;
        }
    }

    *event_num = num_events;
    return false;
}

MIDITimedBigMessage const* MIDITrack::get_event(int event_num) const
{
    if (event_num >= num_events) {
        return nullptr;
    }

    else {
        return get_event_address(event_num);
    }
}

MIDITimedBigMessage* MIDITrack::get_event(int event_num)
{
    if (event_num >= num_events) {
        return nullptr;
    }

    else {
        return get_event_address(event_num);
    }
}

int MIDITrack::get_buffer_size() const
{
    return buf_size;
}

int MIDITrack::get_num_events() const
{
    return num_events;
}

}  // namespace jdksmidi
