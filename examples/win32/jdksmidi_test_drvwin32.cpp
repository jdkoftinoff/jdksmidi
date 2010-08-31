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


#include "jdksmidi/world.h"
#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/driver.h"
#include "jdksmidi/driverwin32.h"

using namespace jdksmidi;

int main ( int argc, char **argv )
{
#ifdef WIN32

    if ( argc > 1 )
    {
        MIDIFileReadStreamFile rs ( argv[1] );
        MIDIMultiTrack tracks ( 64 );
        MIDIFileReadMultiTrack track_loader ( &tracks );
        MIDIFileRead reader ( &rs, &track_loader );
        MIDISequencerGUIEventNotifierText gui ( stdout );
        MIDISequencer seq ( &tracks, &gui );
        MIDIDriverWin32 driver ( 128 );
        MIDIManager mgr ( &driver, &gui );
        reader.Parse();
        driver.StartTimer ( 20 );
        driver.OpenMIDIOutPort ( MIDI_MAPPER );
        seq.GoToZero();
        mgr.SetSeq ( &seq );
        mgr.SetTimeOffset ( timeGetTime() );
        mgr.SeqPlay();
        getchar();
        mgr.SeqStop();
    }

    return 0;
#endif
}


