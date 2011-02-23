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
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

#ifndef JDKSMIDI_FILEWRITEMULTITRACK_H
#define JDKSMIDI_FILEWRITEMULTITRACK_H

#include "jdksmidi/filewrite.h"
#include "jdksmidi/multitrack.h"

namespace jdksmidi
{

class MIDIFileWriteMultiTrack
{
public:

    MIDIFileWriteMultiTrack (
        const MIDIMultiTrack *mlt_,
        MIDIFileWriteStream *strm_
    );

    virtual ~MIDIFileWriteMultiTrack();

    bool Write ( int num_tracks, int division );

    bool Write ( int num_tracks )
    {
        return Write ( num_tracks, multitrack->GetClksPerBeat() );
    }
    bool Write()
    {
        return Write ( multitrack->GetNumTracks(), multitrack->GetClksPerBeat() );
    }

    // false argument disable use running status in midi file (true on default)
    void UseRunningStatus( bool use )
    {
        writer.UseRunningStatus( use );
    }

private:
    virtual bool PreWrite();
    virtual bool PostWrite();

    const MIDIMultiTrack *multitrack;
    MIDIFileWrite writer;
};

}


#endif
