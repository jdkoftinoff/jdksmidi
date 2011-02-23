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

#ifndef JDKSMIDI_FILEREADMULTITRACK_H
#define JDKSMIDI_FILEREADMULTITRACK_H

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/file.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/multitrack.h"

namespace jdksmidi
{

class MIDIFileReadMultiTrack : public MIDIFileEvents
{
public:
    MIDIFileReadMultiTrack ( MIDIMultiTrack *mlttrk );

    virtual ~MIDIFileReadMultiTrack();


//
// The possible events in a MIDI Files
//

    virtual bool mf_metamisc ( MIDIClockTime time, int type, int len, unsigned char *data );
    virtual bool mf_timesig ( MIDIClockTime time, int, int, int, int );
    virtual bool mf_tempo ( MIDIClockTime time, unsigned char a, unsigned char b, unsigned char c );
    virtual bool mf_keysig ( MIDIClockTime time, int, int );
    virtual bool mf_sqspecific ( MIDIClockTime time, int, unsigned char * );
    virtual bool mf_text ( MIDIClockTime time, int, int, unsigned char * );
    virtual bool mf_eot ( MIDIClockTime time );
    virtual bool mf_sysex ( MIDIClockTime time, int type, int len, unsigned char *s );

//
// the following methods are to be overridden for your specific purpose
//

    virtual void mf_error ( const char * );
    virtual void mf_starttrack ( int trk );
    virtual void mf_endtrack ( int trk );
    virtual void mf_header ( int, int, int );

//
// Higher level dispatch functions
//

    virtual bool ChanMessage ( const MIDITimedMessage &msg );
    // test and sort events temporal order in all tracks
    virtual void SortEventsOrder();

protected:

    // return false if dest_track absent or no space for event
    bool AddEventToMultiTrack ( const MIDITimedMessage &msg, MIDISystemExclusive *sysex, int dest_track );

    MIDIMultiTrack *multitrack;
    int cur_track;

    int the_format;
    int num_tracks;
    int division;

};

}


#endif
