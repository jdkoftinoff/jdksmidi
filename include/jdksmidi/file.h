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

#ifndef JDKSMIDI_FILE_H
#define JDKSMIDI_FILE_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/tempo.h"

namespace jdksmidi
{

//
// The MIDIFile class contains definitions and utilities to deal with
// reading and writing midi files.
//

const unsigned long _MThd = OSTYPE ( 'M', 'T', 'h', 'd' );
const unsigned long _MTrk = OSTYPE ( 'M', 'T', 'r', 'k' );

class MIDIFile
{
public:

    MIDIFile();
    virtual ~MIDIFile();

    struct MIDIFileChunk
    {
        unsigned long id;
        unsigned long length;
    };

    struct MIDIFileHeader
    {
        short format;
        short ntrks;
        short division;
    };

    //
    // define all the different meta event message types.
    //

    enum
    {
        MF_META_SEQUENCE_NUMBER = META_SEQUENCE_NUMBER,

        MF_META_GENERIC_TEXT    = META_GENERIC_TEXT,
        MF_META_COPYRIGHT       = META_COPYRIGHT,
        MF_META_TRACK_NAME      = META_TRACK_NAME,
        MF_META_INSTRUMENT_NAME = META_INSTRUMENT_NAME,
        MF_META_LYRIC_TEXT      = META_LYRIC_TEXT,
        MF_META_MARKER_TEXT     = META_MARKER_TEXT,
        MF_META_CUE_POINT       = META_CUE_POINT,
        MF_META_GENERIC_TEXT_8  = META_PROGRAM_NAME,
        MF_META_GENERIC_TEXT_9  = META_DEVICE_NAME,
        MF_META_GENERIC_TEXT_A  = META_GENERIC_TEXT_A,
        MF_META_GENERIC_TEXT_B  = META_GENERIC_TEXT_B,
        MF_META_GENERIC_TEXT_C  = META_GENERIC_TEXT_C,
        MF_META_GENERIC_TEXT_D  = META_GENERIC_TEXT_D,
        MF_META_GENERIC_TEXT_E  = META_GENERIC_TEXT_E,
        MF_META_GENERIC_TEXT_F  = META_GENERIC_TEXT_F,

        MF_META_CHANNEL_PREFIX = META_CHANNEL_PREFIX,
        MF_META_OUTPUT_CABLE   = META_OUTPUT_CABLE,
        MF_META_TRACK_LOOP     = META_TRACK_LOOP,
        MF_META_END_OF_TRACK   = META_END_OF_TRACK,

        MF_META_TEMPO   = META_TEMPO,
        MF_META_SMPTE   = META_SMPTE,
        MF_META_TIMESIG = META_TIMESIG,
        MF_META_KEYSIG  = META_KEYSIG,

        MF_META_SEQUENCER_SPECIFIC = META_SEQUENCER_SPECIFIC
    };


    //
    // ConvertTempoToFreq() returns the frequency of the required
    // tempo clock
    //

    static unsigned long ConvertTempoToFreq ( short division, MIDITempo &tempo );

    //
    // Convert a four byte number to an unsigned long.
    //

    static unsigned long To32Bit ( unsigned char a, unsigned char b, unsigned char c, unsigned char d )
    {
        return ( static_cast<unsigned long> ( a ) << 24 ) |
               ( static_cast<unsigned long> ( b ) << 16 ) |
               ( static_cast<unsigned long> ( c ) <<  8 ) |
               ( static_cast<unsigned long> ( d )       );
    }

    //
    // Convert a two byte number to an unsigned short
    //

    static unsigned short To16Bit ( unsigned char a, unsigned char b )
    {
        return ( static_cast<unsigned short> ( a ) << 8 ) |
               ( static_cast<unsigned short> ( b )      );
    }

    static unsigned long ReadVariableLengthNumber ( unsigned char **in );
    static unsigned char * WriteVariableLengthNumber ( unsigned long num, unsigned char *out );
};

}

#endif

