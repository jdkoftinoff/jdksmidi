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

enum SMPTE_RATE
{
    SMPTE_RATE_24 = 0,
    SMPTE_RATE_25,
    SMPTE_RATE_2997,
    SMPTE_RATE_2997DF,
    SMPTE_RATE_30,
    SMPTE_RATE_30DF
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
// MDGetSMPTERateFrequency() converts the SMPTE_RATE enum to a double frequency.
//

inline double GetSMPTERateFrequency(SMPTE_RATE r)
{
    extern double const smpte_smpte_rates[];
    return smpte_smpte_rates[static_cast<int>(r)];
}

//
// MDGetSMPTERateFrequencyLong() convert the SMPTE_RATE enum to an int
// frequency times 100
//

inline long GetSMPTERateFrequencyLong(SMPTE_RATE r)
{
    extern double const smpte_smpte_rates_long[];
    return static_cast<long>(smpte_smpte_rates_long[static_cast<int>(r)]);
}

//
// GetSampleRateFrequency() convert the SAMPLE_RATE enum to a double frequency
//

inline double GetSampleRateFrequency(SAMPLE_RATE r)
{
    extern double const smpte_sample_rates[];
    return smpte_sample_rates[static_cast<int>(r)];
}

//
// MDGetSampleRateFrequencyLong() converts the SAMPLE_RATE enum to a long
// frequency times 10
//

inline long GetSampleRateFrequencyLong(SAMPLE_RATE r)
{
    // return the sample rate as a long word of the frequency times 10.
    extern long const smpte_sample_rates_long[];
    return smpte_sample_rates_long[static_cast<int>(r)];
}

class SMPTE
{
  public:
    SMPTE(SMPTE_RATE smpte_rate = SMPTE_RATE_30, SAMPLE_RATE sample_rate = SAMPLE_48000);

    SMPTE(const SMPTE& s);

    void SetSMPTERate(SMPTE_RATE r)
    {
        smpte_rate = r;
        sample_number_dirty = true;
    }
    SMPTE_RATE GetSMPTERate() { return smpte_rate; }

    void SetSampleRate(SAMPLE_RATE r)
    {
        sample_rate = r;
        sample_number_dirty = true;
    }
    SAMPLE_RATE GetSampleRate() { return sample_rate; }

    void SetSampleNumber(std::uint32_t n)
    {
        sample_number = n;
        SampleToTime();
    }
    std::uint32_t GetSampleNumber()
    {
        if (sample_number_dirty)
            TimeToSample();

        return sample_number;
    }

    void set_time(
        std::uint8_t h, std::uint8_t m, std::uint8_t s, std::uint8_t f = 0, std::uint8_t sf = 0)
    {
        hours = h;
        minutes = m;
        seconds = s;
        frames = f;
        sub_frames = sf;
        sample_number_dirty = true;
    }

    std::uint8_t get_hours() { return hours; }
    std::uint8_t get_minutes() { return minutes; }
    std::uint8_t get_seconds() { return seconds; }
    std::uint8_t get_frames() { return frames; }
    std::uint8_t get_sub_frames() { return sub_frames; }

    void set_hours(std::uint8_t h)
    {
        hours = h;
        sample_number_dirty = true;
    }
    void set_minutes(std::uint8_t m)
    {
        minutes = m;
        sample_number_dirty = true;
    }
    void set_seconds(std::uint8_t s)
    {
        seconds = s;
        sample_number_dirty = true;
    }
    void set_frames(std::uint8_t f)
    {
        frames = f;
        sample_number_dirty = true;
    }
    void set_sub_frames(std::uint8_t sf)
    {
        sub_frames = sf;
        sample_number_dirty = true;
    }

    void AddHours(char h);
    void AddMinutes(char m);
    void AddSeconds(char s);
    void AddFrames(char f);
    void AddSubFrames(char sf);
    void AddSamples(long n)
    {
        sample_number = GetSampleNumber() + n;
        SampleToTime();
    }

    void IncHours() { AddHours(1); }
    void IncMinutes() { AddMinutes(1); }
    void IncSeconds() { AddSeconds(1); }
    void IncFrames() { AddFrames(1); }
    void IncSubFrames() { AddSubFrames(1); }
    void IncSamples() { AddSamples(1); }

    void DecHours() { AddHours(-1); }
    void DecMinutes() { AddMinutes(-1); }
    void DecSeconds() { AddSeconds(-1); }
    void DecFrames() { AddFrames(-1); }
    void DecSubFrames() { AddSubFrames(-1); }
    void DecSamples() { AddSamples(-1); }

    const SMPTE& operator=(const SMPTE& s)
    {
        copy(s);
        return *this;
    }
    bool operator==(SMPTE& s) { return Compare(s) == 0; }
    bool operator!=(SMPTE& s) { return Compare(s) != 0; }
    bool operator<(SMPTE& s) { return Compare(s) < 0; }
    bool operator>(SMPTE& s) { return Compare(s) > 0; }
    bool operator<=(SMPTE& s) { return Compare(s) <= 0; }
    bool operator>=(SMPTE& s) { return Compare(s) >= 0; }

    const SMPTE& operator+=(SMPTE& s)
    {
        Add(s);
        return *this;
    }
    const SMPTE& operator-=(SMPTE& s)
    {
        Subtract(s);
        return *this;
    }

  protected:
    void SampleToTime();
    void TimeToSample();

    void copy(const SMPTE& s);
    int Compare(SMPTE& s);
    void Add(SMPTE& s);
    void Subtract(SMPTE& s);

    long GetSampleRateLong() { return GetSampleRateFrequencyLong(sample_rate); }

    int GetSMPTERateLong() { return GetSMPTERateFrequencyLong(smpte_rate); }

  private:
    SMPTE_RATE smpte_rate;
    SAMPLE_RATE sample_rate;
    std::uint32_t sample_number;

    std::uint8_t hours;
    std::uint8_t minutes;
    std::uint8_t seconds;
    std::uint8_t frames;
    std::uint8_t sub_frames;
    std::uint8_t sample_number_dirty;

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
