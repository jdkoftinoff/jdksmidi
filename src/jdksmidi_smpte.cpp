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
/*
** Copyright 1986 to 1998 By Jeffrey Koftinoff
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by Jeffrey Koftinoff
**
*/

#include "jdksmidi/smpte.h"
#include "jdksmidi/world.h"

#ifndef DEBUG_MDSMPTE
#    define DEBUG_MDSMPTE 0
#endif

#if DEBUG_MDSMPTE
#    undef DBG
#    define DBG(a) a
#else
#    undef DBG
#    define DBG(a)
#endif

namespace jdksmidi {

std::uint8_t const smpte_max_frames[] = {24, 25, 30, 30, 30, 30};

double const smpte_smpte_rates[] = {24.0, 25.0, 30.0 / 1.001, 30.0 / 1.001, 30.0, 30.0};

double const smpte_smpte_rates_long[] = {2400, 2500, 3000 / 1.001, 3000 / 1.001, 3000, 3000};

double const smpte_sample_rates[] = {
    32000.0, 44100.0 / 1.001, 44100.0, 48000.0 / 1.001, 48000.0, 48000.0 * 1.001};

long const smpte_sample_rates_long[] = {
    320000,
    (long)(441000.0 / 1.001),
    441000,
    (long)(480000.0 / 1.001),
    480000,
    (long)(480000.0 * 1.001)};

SMPTE::SMPTE(SMPTE_RATE smpte_rate_, SAMPLE_RATE sample_rate_)
    : smpte_rate(smpte_rate_)
    , sample_rate(sample_rate_)
    , sample_number(0)
    , hours(0)
    , minutes(0)
    , seconds(0)
    , frames(0)
    , sub_frames(0)
    , sample_number_dirty(false)
{}

SMPTE::SMPTE(const SMPTE& s)
{
    copy(s);
}

void SMPTE::add_hours(char h)
{
    add_samples(
        get_sample_rate_long()  // samples per second times 10
        * h *
        (60    // seconds per minute
         * 60  // minutes per hour
         / 10  // compensate for freq*10
         ));
}

void SMPTE::add_minutes(char m)
{
    add_samples(
        get_sample_rate_long() * m *  // samples per second times 10
        (60                           // seconds per minute
         / 10)                        // compensate for freq*10
    );
}

void SMPTE::add_seconds(char s)
{
    add_samples(
        get_sample_rate_long()  // samples per second times 10
        * s                     // number of seconds
        / 10                    // compensate for freq*10
    );
}

void SMPTE::add_frames(char f)
{
    add_samples(
        get_sample_rate_long()   // samples per second times 10
        * f                      // number of frames
        * 10                     // times 10
        / get_smpte_rate_long()  // divide by smpte rate (frames per second) times 100
    );
}

void SMPTE::add_sub_frames(char sf)
{
    add_samples(
        get_sample_rate_long()   // samples per second times 10
        * sf                     // number of sub frames
        / get_smpte_rate_long()  // divide by smpte rate (frames per second) times 100
        / 10                     // divide by 10 to get hundredths of a frame
    );
}

void SMPTE::sample_to_time()
{
    //
    // make a temporary copy of the sample number
    //
    std::uint32_t tmp_sample = sample_number;
    //
    // keep track of the actual rates in use in doubles.
    //
    double the_smpte_rate = smpte_smpte_rates[smpte_rate];
    double the_sample_rate = smpte_sample_rates[sample_rate];
    //
    // keep track of the maximum frame number for this smpte format.
    //
    std::uint8_t max_frame = smpte_max_frames[smpte_rate];
    //
    // Calculate the number of samples per frame.
    //
    double samples_per_frame = smpte_sample_rates[sample_rate] / smpte_smpte_rates[smpte_rate];
    //
    // if the smpte rate is a drop frame type, calculate the number
    // of frames that must be dropped.
    //

    if (smpte_rate == SMPTE_RATE_30DF || smpte_rate == SMPTE_RATE_2997DF) {
        //
        // Calculate number of minutes that have gone by
        //
        // short num_minutes = (short)((double)tmp_sample/(smpte_sample_rates[sample_rate]))/60;
        int num_minutes = tmp_sample / (48000 * 60);
        DBG(printf("num_minutes=%d\n", (int)num_minutes));
        //
        // Calculate the number of tens of minutes that have gone by, including minute 00
        //
        int ten_minutes = num_minutes / 10;
        DBG(printf("ten_minutes=%d\n", (int)ten_minutes));
        //
        // Calculate the number of frames that are dropped by this
        // time.
        //
        int drops = (num_minutes - ten_minutes) * 2;
        DBG(printf("drops=%d\n", (int)drops));
        //
        // Offset the tmp_sample number by this amount of frames.
        //
        DBG(printf("tmp_sample before drops=%ld\n", (long)tmp_sample));
        tmp_sample += (std::uint32_t)(drops * samples_per_frame);
        DBG(printf("tmp_sample after drops=%ld\n", (long)tmp_sample));
    }

    //
    // Calculate the time in sub frames, frames, seconds, minutes, hours
    //
    std::uint32_t rounded_sub_frames =
        (std::uint32_t)((tmp_sample * the_smpte_rate * 100) / the_sample_rate + .5);
    DBG(printf("rounded_sub_frames = %ld\n", rounded_sub_frames));
    sub_frames = (std::uint8_t)((rounded_sub_frames) % 100);
    frames = (std::uint8_t)((rounded_sub_frames / 100) % max_frame);
    seconds = (std::uint8_t)((rounded_sub_frames / (100L * max_frame)) % 60);
    minutes = (std::uint8_t)((rounded_sub_frames / (100L * 60L * max_frame)) % 60);
    hours = (std::uint8_t)((rounded_sub_frames / (100L * 60L * 24L * max_frame)) % 24);
}

void SMPTE::time_to_sample()
{
    //
    // keep track of the actual rates in use in doubles.
    //
    double the_smpte_rate = smpte_smpte_rates[smpte_rate];
    double the_sample_rate = smpte_sample_rates[sample_rate];
    //
    // optimize a coupla similiar double divides by calculating it once.
    //
    double samples_per_frame = the_sample_rate / the_smpte_rate;
    //
    // calculate the sample number
    //
    double tmp_sample = (double)((
        (hours * the_sample_rate * (60 * 60)) + (minutes * the_sample_rate * 60) +
        (seconds * the_sample_rate) + (frames * samples_per_frame) +
        (sub_frames * samples_per_frame * (1.0 / 100.0)) + .5));
    //
    // Now compensate for Drop Frame mode if we are in drop frame mode.
    //

    if (smpte_rate == SMPTE_RATE_30DF || smpte_rate == SMPTE_RATE_2997DF) {
        //
        // Calculate number of minutes that have gone by
        //
        int num_minutes = (int)((double)tmp_sample / (smpte_sample_rates[sample_rate] * 60));
        DBG(printf("num_minutes=%d\n", (int)num_minutes));
        //
        // Calculate the number of tens of minutes that have gone by, including minute 00
        //
        int ten_minutes = num_minutes / 10;
        DBG(printf("ten_minutes=%d\n", (int)ten_minutes));
        //
        // Calculate the number of frames that are dropped by this
        // time.
        //
        int drops = (num_minutes - ten_minutes) * 2;
        DBG(printf("drops=%d\n", (int)drops));
        //
        // Offset the tmp_sample number by this amount of frames.
        //
        DBG(printf("tmp_sample before drops=%ld\n", (long)tmp_sample));
        tmp_sample -= drops * samples_per_frame;
        DBG(printf("tmp_sample after drops=%ld\n", (long)tmp_sample));
    }

    //
    // save the calculated sample number in self.
    //
    sample_number = (std::uint32_t)tmp_sample;
}

void SMPTE::copy(const SMPTE& s)
{
    smpte_rate = s.smpte_rate;
    sample_rate = s.sample_rate;
    sample_number = s.sample_number;
    hours = s.hours;
    minutes = s.minutes;
    seconds = s.seconds;
    frames = s.frames;
    sub_frames = s.sub_frames;
    sample_number_dirty = s.sample_number_dirty;
}

int SMPTE::compare(SMPTE& s)
{
    std::uint32_t a = get_sample_number();
    std::uint32_t b = s.get_sample_number();

    if (a < b)
        return -1;

    if (a > b)
        return 1;

    return 0;
}

void SMPTE::add(SMPTE& s)
{
    std::uint32_t a = get_sample_number();
    std::uint32_t b = s.get_sample_number();
    set_sample_number(a + b);
}

void SMPTE::subtract(SMPTE& s)
{
    std::uint32_t a = get_sample_number();
    std::uint32_t b = s.get_sample_number();
    set_sample_number(a - b);
}

}  // namespace jdksmidi
