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
/*
** Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

#ifndef JDKSMIDI_SMPTE_H
#define JDKSMIDI_SMPTE_H

namespace jdksmidi
{

/// \defgroup rates "The smpte and sample rates"
/// These are the allowed smpte rates and sample rates allowed for SMPTE class
//@{
/// The smpte rates
enum SMPTE_RATE
{
    SMPTE_RATE_24 = 0, ///< 24 frames/sec
    SMPTE_RATE_25,     ///< 25 frames/sec
    SMPTE_RATE_2997,   ///< 29.97 frames/sec
    SMPTE_RATE_2997DF, ///< 29.97 frames/sec drop
    SMPTE_RATE_30,     ///< 30 frames/sec
    SMPTE_RATE_30DF    ///< 30 frames/sec drop
};

/// The sample rates
enum SAMPLE_RATE
{
    SAMPLE_32000 = 0, ///< 32000 samples/sec
    SAMPLE_44056,     ///< 44056 samples/sec
    SAMPLE_44100,     ///< 44100 samples/sec
    SAMPLE_47952,     ///< 47952 samples/sec
    SAMPLE_48000,     ///< 48000 samples/sec
    SAMPLE_48048      ///< 48048 samples/sec
};
//@}

//
// MDGetSMPTERateFrequency() converts the SMPTE_RATE enum to a double frequency.
//

extern const double smpte_smpte_rates[];

inline double GetSMPTERateFrequency( SMPTE_RATE r )
{
    return smpte_smpte_rates[(int)r];
}

//
// MDGetSMPTERateFrequencyLong() convert the SMPTE_RATE enum to an int
// frequency times 100
//

extern const double smpte_smpte_rates_long[];

inline long GetSMPTERateFrequencyLong( SMPTE_RATE r )
{
    return (long)smpte_smpte_rates_long[(int)r];
}

//
// GetSampleRateFrequency() convert the SAMPLE_RATE enum to a double frequency
//

extern const double smpte_sample_rates[];

inline double GetSampleRateFrequency( SAMPLE_RATE r )
{
    return smpte_sample_rates[(int)r];
}

//
// MDGetSampleRateFrequencyLong() converts the SAMPLE_RATE enum to a long
// frequency times 10
//

extern const long smpte_sample_rates_long[];

inline long GetSampleRateFrequencyLong( SAMPLE_RATE r )
{
    // return the sample rate as a long word of the frequency times 10.
    return smpte_sample_rates_long[(int)r];
}

///
/// This class performs conversions between number of samples, milliseconds and smpte format
/// (hours::minutes::seconds::frames::subframes).
/// You can choose between several smpte formats and sample rates
/// (see \ref rates "SMPTE and sample rates").
///

class SMPTE
{
  public:

    /// The constructor sets the SMPTE rate to SMPTE_30 and the sample rate to SAMPLE_48000
    SMPTE( SMPTE_RATE smpte_rate = SMPTE_RATE_30, SAMPLE_RATE sample_rate = SAMPLE_48000 );

    /// The copy constructor
    SMPTE( const SMPTE &s );

    /// Sets the smpte rate. See \ref rates "SMPTE and sample rates" for avalaible smpte rates
    void SetSMPTERate( SMPTE_RATE r )
    {
        smpte_rate = r;
        sample_number_dirty = true;
    }

    /// Returns the smpte rate
    SMPTE_RATE GetSMPTERate()
    {
        return smpte_rate;
    }

    /// Sets the sample rate. See \ref rates "SMPTE and sample rates" for avalaible sample rates
    void SetSampleRate( SAMPLE_RATE r )
    {
        sample_rate = r;
        sample_number_dirty = true;
    }

    /// Returns the sample rate
    SAMPLE_RATE GetSampleRate()
    {
        return sample_rate;
    }

    /// \name To perform a smpte-to-samples conversion.
    /// You must first load the SMPTE with the number of of hours, minutes, seconds, frames and subframes
    /// to convert using SetTime() (or other functions setting individual items); then you can call
    /// GetSampleNumber() to get the corresponding number of samples.
    //@{
    void SetTime( uchar h, uchar m, uchar s, uchar f = 0, uchar sf = 0 )
    {
        hours = h;
        minutes = m;
        seconds = s;
        frames = f;
        sub_frames = sf;
        sample_number_dirty = true;
    }

    void SetHours( uchar h )
    {
        hours = h;
        sample_number_dirty = true;
    }
    void SetMinutes( uchar m )
    {
        minutes = m;
        sample_number_dirty = true;
    }
    void SetSeconds( uchar s )
    {
        seconds = s;
        sample_number_dirty = true;
    }
    void SetFrames( uchar f )
    {
        frames = f;
        sample_number_dirty = true;
    }
    void SetSubFrames( uchar sf )
    {
        sub_frames = sf;
        sample_number_dirty = true;
    }
    ulong GetSampleNumber()
    {
        if ( sample_number_dirty )
            TimeToSample();

        return sample_number;
    }
    //@}

    /// \name To perform a samples-to-smpte conversion.
    /// You must first load the SMPTE with the number of samples to convert using SetSampleNumber();
    /// then you can call other functions to get the corresponding hours, minutes, etc.
    //@{
    void SetSampleNumber( ulong n )
    {
        sample_number = n;
        SampleToTime();
    }
    uchar GetHours()
    {
        return hours;
    }
    uchar GetMinutes()
    {
        return minutes;
    }
    uchar GetSeconds()
    {
        return seconds;
    }
    uchar GetFrames()
    {
        return frames;
    }
    uchar GetSubFrames()
    {
        return sub_frames;
    }
    //@}

    /// To perform a millisecond-to-smpte or millisecond-to-sample conversion.
    /// You must first load the SMPTE with the number of milliseconds to convert using SetMilliSeconds();
    /// then you can call GetSampleNumber or GetHours(), GetMinutes() etc.
    void SetMilliSeconds( ulong msecs )
    {
        sample_number = ( ulong )( msecs * GetSampleRateFrequency( sample_rate ) / 1000 );
        SampleToTime();
    }

    /// To perform a smpte-to-millisecond or sample-to-millisecond conversion.
    /// You must first load the SMPTE with the number of sample or with smpte items to convert using
    /// SetSampleNumber() or SetHoure(), SetMinutes() etc. ; then you can call GetMilliSeconds() to get the
    /// corresponding millieseconds
    ulong GetMilliSeconds()
    {
        if ( sample_number_dirty )
            TimeToSample();

        return ( ulong )( sample_number * 1000 / GetSampleRateFrequency( sample_rate ) );
    }

    /// \name To add, increment and decrement samples.
    /// These functions add, increment or decrement the current sample number./ You can use them
    /// instead of SetSampleNunber() to perform a samples-to-smpte conversion
    //@{
    void AddSamples( long n )
    {
        sample_number = GetSampleNumber() + n;
        SampleToTime();
    }

    void IncSamples()
    {
        AddSamples( 1 );
    }

    void DecSamples()
    {
        AddSamples( -1 );
    }
    //@}

    /// \name To add, increment and decrement smpte
    /// These functions add, increment or decrement smpte time parameters./ You can use them instead of
    /// SetTime() to perform a smpte-to-samples conversion
    //@{
    void AddHours( char h );
    void AddMinutes( char m );
    void AddSeconds( char s );
    void AddFrames( char f );
    void AddSubFrames( char sf );

    void IncHours()
    {
        AddHours( 1 );
    }
    void IncMinutes()
    {
        AddMinutes( 1 );
    }
    void IncSeconds()
    {
        AddSeconds( 1 );
    }
    void IncFrames()
    {
        AddFrames( 1 );
    }
    void IncSubFrames()
    {
        AddSubFrames( 1 );
    }

    void DecHours()
    {
        AddHours( -1 );
    }
    void DecMinutes()
    {
        AddMinutes( -1 );
    }
    void DecSeconds()
    {
        AddSeconds( -1 );
    }
    void DecFrames()
    {
        AddFrames( -1 );
    }
    void DecSubFrames()
    {
        AddSubFrames( -1 );
    }
    //@}

    /// \name The operators (these compare the current time)
    //@{
    const SMPTE &operator=( const SMPTE &s )
    {
        Copy( s );
        return *this;
    }
    bool operator==( SMPTE &s )
    {
        return Compare( s ) == 0;
    }
    bool operator!=( SMPTE &s )
    {
        return Compare( s ) != 0;
    }
    bool operator<( SMPTE &s )
    {
        return Compare( s ) < 0;
    }
    bool operator>( SMPTE &s )
    {
        return Compare( s ) > 0;
    }
    bool operator<=( SMPTE &s )
    {
        return Compare( s ) <= 0;
    }
    bool operator>=( SMPTE &s )
    {
        return Compare( s ) >= 0;
    }

    const SMPTE &operator+=( SMPTE &s )
    {
        Add( s );
        return *this;
    }
    const SMPTE &operator-=( SMPTE &s )
    {
        Subtract( s );
        return *this;
    }
    //@}

  protected:

    /// Performs samples-to-smpte conversion
    void SampleToTime();

    /// Performs smpte-to-samples conversion
    void TimeToSample();

    void Copy( const SMPTE &s );
    int Compare( SMPTE &s );
    void Add( SMPTE &s );
    void Subtract( SMPTE &s );

    long GetSampleRateLong()
    {
        return GetSampleRateFrequencyLong( sample_rate );
    }

    int GetSMPTERateLong()
    {
        return GetSMPTERateFrequencyLong( smpte_rate );
    }

  private:
    SMPTE_RATE smpte_rate;
    SAMPLE_RATE sample_rate;
    ulong sample_number;

    uchar hours;
    uchar minutes;
    uchar seconds;
    uchar frames;
    uchar sub_frames;
    uchar sample_number_dirty;

    friend SMPTE operator+( SMPTE a, SMPTE b );
    friend SMPTE operator-( SMPTE a, SMPTE b );
};

inline SMPTE operator+( SMPTE a, SMPTE b )
{
    SMPTE c( a );
    c += b;
    return c;
}

inline SMPTE operator-( SMPTE a, SMPTE b )
{
    SMPTE c( a );
    c -= b;
    return c;
}
}

#endif
