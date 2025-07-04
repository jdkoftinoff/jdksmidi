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

#ifndef JDKSMIDI_SMPTE_H
#define JDKSMIDI_SMPTE_H

#include <cstdint>

namespace jdksmidi {

enum class SMPTEFormat
{
    Rate24 = 0,
    Rate25,
    Rate2997,
    Rate2997DF,
    Rate30,
    Rate30DF
};

enum SAMPLE_RATE
{
    SAMPLE_32000 = 0,
    SAMPLE_44056,
    SAMPLE_44100,
    SAMPLE_47952,
    SAMPLE_48000,
    SAMPLE_48048
};

//
// MDGetSMPTERateFrequency() converts the SMPTEFormat enum to a double frequency.
//

inline double get_smpte_rate_frequency(SMPTEFormat r)
{
    extern double const smpte_smpte_rates[];
    return smpte_smpte_rates[static_cast<int>(r)];
}

//
// MDGetSMPTERateFrequencyLong() convert the SMPTEFormat enum to an int
// frequency times 100
//

inline long get_smpte_rate_frequency_long(SMPTEFormat r)
{
    extern double const smpte_smpte_rates_long[];
    return static_cast<long>(smpte_smpte_rates_long[static_cast<int>(r)]);
}

//
// get_sample_rate_frequency() convert the SAMPLE_RATE enum to a double frequency
//

inline double get_sample_rate_frequency(SAMPLE_RATE r)
{
    extern double const smpte_sample_rates[];
    return smpte_sample_rates[static_cast<int>(r)];
}

//
// MDGetSampleRateFrequencyLong() converts the SAMPLE_RATE enum to a long
// frequency times 10
//

inline long get_sample_rate_frequency_long(SAMPLE_RATE r)
{
    // return the sample rate as a long word of the frequency times 10.
    extern long const smpte_sample_rates_long[];
    return smpte_sample_rates_long[static_cast<int>(r)];
}

class SMPTE
{
  public:
    SMPTE(SMPTEFormat smpte_rate = SMPTEFormat::Rate30, SAMPLE_RATE sample_rate = SAMPLE_48000);

    SMPTE(const SMPTE& s);

    void set_smpte_rate(SMPTEFormat r)
    {
        _smpte_rate = r;
        _sample_number_dirty = true;
    }
    SMPTEFormat get_smpte_rate() { return _smpte_rate; }

    void set_sample_rate(SAMPLE_RATE r)
    {
        _sample_rate = r;
        _sample_number_dirty = true;
    }
    SAMPLE_RATE get_sample_rate() { return _sample_rate; }

    void set_sample_number(std::uint32_t n)
    {
        _sample_number = n;
        sample_to_time();
    }
    std::uint32_t get_sample_number()
    {
        if (_sample_number_dirty)
            time_to_sample();

        return _sample_number;
    }

    void set_time(
        std::uint8_t h, std::uint8_t m, std::uint8_t s, std::uint8_t f = 0, std::uint8_t sf = 0)
    {
        _hours = h;
        _minutes = m;
        _seconds = s;
        _frames = f;
        _sub_frames = sf;
        _sample_number_dirty = true;
    }

    std::uint8_t get_hours() { return _hours; }
    std::uint8_t get_minutes() { return _minutes; }
    std::uint8_t get_seconds() { return _seconds; }
    std::uint8_t get_frames() { return _frames; }
    std::uint8_t get_sub_frames() { return _sub_frames; }

    void set_hours(std::uint8_t h)
    {
        _hours = h;
        _sample_number_dirty = true;
    }
    void set_minutes(std::uint8_t m)
    {
        _minutes = m;
        _sample_number_dirty = true;
    }
    void set_seconds(std::uint8_t s)
    {
        _seconds = s;
        _sample_number_dirty = true;
    }
    void set_frames(std::uint8_t f)
    {
        _frames = f;
        _sample_number_dirty = true;
    }
    void set_sub_frames(std::uint8_t sf)
    {
        _sub_frames = sf;
        _sample_number_dirty = true;
    }

    void add_hours(char h);
    void add_minutes(char m);
    void add_seconds(char s);
    void add_frames(char f);
    void add_sub_frames(char sf);
    void add_samples(long n)
    {
        _sample_number = get_sample_number() + n;
        sample_to_time();
    }

    void inc_hours() { add_hours(1); }
    void inc_minutes() { add_minutes(1); }
    void inc_seconds() { add_seconds(1); }
    void inc_frames() { add_frames(1); }
    void inc_sub_frames() { add_sub_frames(1); }
    void inc_samples() { add_samples(1); }

    void dec_hours() { add_hours(-1); }
    void dec_minutes() { add_minutes(-1); }
    void dec_seconds() { add_seconds(-1); }
    void dec_frames() { add_frames(-1); }
    void dec_sub_frames() { add_sub_frames(-1); }
    void dec_samples() { add_samples(-1); }

    const SMPTE& operator=(const SMPTE& s)
    {
        copy(s);
        return *this;
    }
    bool operator==(SMPTE& s) { return compare(s) == 0; }
    bool operator!=(SMPTE& s) { return compare(s) != 0; }
    bool operator<(SMPTE& s) { return compare(s) < 0; }
    bool operator>(SMPTE& s) { return compare(s) > 0; }
    bool operator<=(SMPTE& s) { return compare(s) <= 0; }
    bool operator>=(SMPTE& s) { return compare(s) >= 0; }

    const SMPTE& operator+=(SMPTE& s)
    {
        add(s);
        return *this;
    }
    const SMPTE& operator-=(SMPTE& s)
    {
        subtract(s);
        return *this;
    }

  protected:
    void sample_to_time();
    void time_to_sample();

    void copy(const SMPTE& s);
    int compare(SMPTE& s);
    void add(SMPTE& s);
    void subtract(SMPTE& s);

    long get_sample_rate_long() { return get_sample_rate_frequency_long(_sample_rate); }

    int get_smpte_rate_long() { return get_smpte_rate_frequency_long(_smpte_rate); }

  private:
    SMPTEFormat _smpte_rate;
    SAMPLE_RATE _sample_rate;
    std::uint32_t _sample_number;

    std::uint8_t _hours;
    std::uint8_t _minutes;
    std::uint8_t _seconds;
    std::uint8_t _frames;
    std::uint8_t _sub_frames;
    std::uint8_t _sample_number_dirty;

    friend SMPTE operator+(SMPTE a, SMPTE b);
    friend SMPTE operator-(SMPTE a, SMPTE b);
};

inline SMPTE operator+(SMPTE a, SMPTE b)
{
    SMPTE c(a);
    c += b;
    return c;
}

inline SMPTE operator-(SMPTE a, SMPTE b)
{
    SMPTE c(a);
    c -= b;
    return c;
}

}  // namespace jdksmidi

#endif
