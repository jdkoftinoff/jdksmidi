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

#include "jdksmidi/world.h"

#include "jdksmidi/midi.h"

namespace jdksmidi
{

const int lut_msglen[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    3, // 0x80=note off, 3 bytes
    3, // 0x90=note on, 3 bytes
    3, // 0xA0=poly pressure, 3 bytes
    3, // 0xB0=control change, 3 bytes
    2, // 0xC0=program change, 2 bytes
    2, // 0xD0=channel pressure, 2 bytes
    3, // 0xE0=pitch bend, 3 bytes
    -1 // 0xF0=other things. may vary.
};

const int lut_sysmsglen[16] =
{
// System Common Messages
    -1, // 0xF0=Normal SysEx Events start. may vary
    2, // 0xF1=MIDI Time Code. 2 bytes
    3, // 0xF2=MIDI Song position. 3 bytes
    2, // 0xF3=MIDI Song Select. 2 bytes.
    0, // 0xF4=undefined. (Reserved)
    0, // 0xF5=undefined. (Reserved)
    1, // 0xF6=TUNE Request. 1 byte
    0, // 0xF7=Normal or Divided SysEx Events end.
// -1, // 0xF7=Divided or Authorization SysEx Events. may vary

// System Real-Time Messages
    1, // 0xF8=timing clock. 1 byte
    1, // 0xF9=proposed measure end? (Reserved)
    1, // 0xFA=start. 1 byte
    1, // 0xFB=continue. 1 byte
    1, // 0xFC=stop. 1 byte
    0, // 0xFD=undefined. (Reserved)
    1, // 0xFE=active sensing. 1 byte
//  1, // 0xFF=reset. 1 byte

    3  // 0xFF=not reset, but a META-EVENT, which is always 3 bytes
    // TODO@VRM // not valid jet? see comment to midi.h function:
    // inline int GetSystemMessageLength ( unsigned char stat )
};


const bool lut_is_white[12] =
{
//  C C#  D D#  E  F F#  G G#  A A#  B
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1
};



}
