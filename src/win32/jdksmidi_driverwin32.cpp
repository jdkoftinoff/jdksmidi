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

//
// Modified by N. Cassetta ncassetta@tiscali.it
//

#include "jdksmidi/world.h"

#ifdef WIN32
#include "jdksmidi/driverwin32.h"

namespace jdksmidi
{

unsigned int jdks_get_safe_system_msg_id()
{
    static UINT base = WM_APP;
    return base++;
}



MIDISequencerGUIEventNotifierWin32::MIDISequencerGUIEventNotifierWin32 (
    HWND w,
    DWORD msg,
    WPARAM wparam_value_
)
    :
    dest_window ( w ),
    window_msg ( msg ),
    wparam_value ( wparam_value_ ),
    en ( true )
{
}

/* NEW BY NC: auto sets window_msg and wparam_value */
MIDISequencerGUIEventNotifierWin32::MIDISequencerGUIEventNotifierWin32 ( HWND w )
    :
    dest_window ( w ),
    window_msg ( jdks_get_safe_system_msg_id() ),
    wparam_value ( 0 ),
    en ( true )
{
}


void MIDISequencerGUIEventNotifierWin32::Notify (
    const MIDISequencer *seq,
    MIDISequencerGUIEvent e
)
{
    if ( en )
    {
        PostMessage (
            dest_window,
            window_msg,
            wparam_value,
            ( unsigned long ) e
        );
    }
}


//
///////////////////// MIDIDriverWin32
//

char **MIDIDriverWin32::in_dev_names = 0;
char **MIDIDriverWin32::out_dev_names = 0;
unsigned int MIDIDriverWin32::num_in_devs = MIDIDriverWin32::FillMIDIInDevices();
unsigned int MIDIDriverWin32::num_out_devs = MIDIDriverWin32::FillMIDIOutDevices();


MIDIDriverWin32::MIDIDriverWin32 ( int queue_size )
    :
    MIDIDriver ( queue_size ),
    in_handle ( 0 ),
    out_handle ( 0 ),
    in_open ( false ),
    out_open ( false ),
    timer_open ( false )
{
}

MIDIDriverWin32::~MIDIDriverWin32()
{
    StopTimer();
    CloseMIDIInPort();
    CloseMIDIOutPort();
}

bool MIDIDriverWin32::OpenMIDIInPort ( int id )
{
    if ( !in_open )
    {
        if ( midiInOpen (
                    &in_handle,
                    id,
                    ( DWORD ) win32_midi_in,
                    ( DWORD ) this,
                    CALLBACK_FUNCTION ) != 0
           )
        {
            return false;
        }

        midiInStart ( in_handle );
        in_open = true;
    }

    return true;
}

bool MIDIDriverWin32::OpenMIDIOutPort ( int id )
{
    if ( !out_open )
    {
        int e = midiOutOpen (
                    &out_handle,
                    id,
                    0,
                    0,
                    CALLBACK_NULL
                );

        if ( e != 0 )
        {
            return false;
        }

        out_open = true;
    }

    return true;
}

void MIDIDriverWin32::CloseMIDIInPort()
{
    if ( in_open )
    {
        midiInStop ( in_handle );
        midiInClose ( in_handle );
        in_open = false;
    }
}

void MIDIDriverWin32::CloseMIDIOutPort()
{
    if ( out_open )
    {
        midiOutClose ( out_handle );
        out_open = false;
        Reset();
    }
}

void MIDIDriverWin32::ResetMIDIOut()
{
    if ( out_open )
    {
        midiOutReset ( out_handle );
    }
}

bool MIDIDriverWin32::StartTimer ( int res )
{
    if ( !timer_open )
    {
        TIMECAPS tc;

        if ( timeGetDevCaps ( &tc, sizeof ( TIMECAPS ) ) != TIMERR_NOERROR )
        {
            return false;
        }

        timer_res = res;

        if ( timer_res < ( int ) tc.wPeriodMin )
            timer_res = ( int ) tc.wPeriodMin;

        if ( timer_res > ( int ) tc.wPeriodMax )
            timer_res = ( int ) tc.wPeriodMax;

        timeBeginPeriod ( timer_res );
        timer_id = timeSetEvent (
                       res,
                       res,
                       win32_timer,
                       ( DWORD ) this,
                       TIME_PERIODIC
                   );

        if ( timer_id )
        {
            timer_open = true;
        }
    }

    return true;
}

void MIDIDriverWin32::StopTimer()
{
    if ( timer_open )
    {
        timeKillEvent ( timer_id );
        timeEndPeriod ( timer_res );
        timer_open = false;
    }
}

bool MIDIDriverWin32::HardwareMsgOut ( const MIDITimedBigMessage &msg )
{
    if ( out_open )
    {
        // dont send sysex or meta-events
        // if ( msg.GetStatus() < 0xff && !msg.IsSysEx() )
        if ( msg.IsChannelEvent() )
        {
            DWORD winmsg;
            winmsg =
                ( ( ( DWORD ) msg.GetStatus() & 0xFF )       )
                | ( ( ( DWORD ) msg.GetByte1()  & 0xFF ) <<  8 )
                | ( ( ( DWORD ) msg.GetByte2()  & 0xFF ) << 16 );

            if ( midiOutShortMsg ( out_handle, winmsg ) != 0 )
            {
                return false;
            }
        }
        else
        {
            char s[100];
            std::cout << "Driver skipped message " << msg.MsgToText(s) << std::endl;
        }

        return true;
    }

    return false;
}


// protected functions

void CALLBACK MIDIDriverWin32::win32_timer (
    UINT wTimerID,
    UINT msg,
    DWORD dwUser,
    DWORD dw1,
    DWORD dw2
)
{
    MIDIDriverWin32 *self = ( MIDIDriverWin32 * ) dwUser;
    self->TimeTick ( timeGetTime() );
}

void CALLBACK MIDIDriverWin32::win32_midi_in (
    HMIDIIN hMidiIn,
    UINT wMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
)
{
    MIDIDriverWin32 *self = ( MIDIDriverWin32 * ) dwInstance;

    if ( wMsg == MIM_DATA )
    {
        MIDITimedBigMessage msg;
        msg.SetStatus ( ( unsigned char ) ( dwParam1 & 0xff ) );
        msg.SetByte1 ( ( unsigned char ) ( ( dwParam1 >> 8 ) & 0xff ) );
        msg.SetByte2 ( ( unsigned char ) ( ( dwParam1 >> 16 ) & 0xff ) );
        msg.SetTime ( timeGetTime() );
        self->HardwareMsgIn ( msg );
    }
}


unsigned int MIDIDriverWin32::FillMIDIInDevices()
{
    MIDIINCAPS InCaps;
    UINT n_devs = midiInGetNumDevs();
    in_dev_names = new char* [n_devs];
    for(UINT i = 0; i < n_devs; i++)
    {
        if ( midiInGetDevCaps( i, &InCaps, sizeof(InCaps) ) == MMSYSERR_NOERROR )
        {
            in_dev_names[i] = new char[DEVICENAME_LEN];
            strncpy( in_dev_names[i], InCaps.szPname, DEVICENAME_LEN-1 );
            in_dev_names[i][DEVICENAME_LEN-1] = 0;
        }
    }
    return n_devs;
}


unsigned int MIDIDriverWin32::FillMIDIOutDevices()
{
    MIDIOUTCAPS OutCaps;
    UINT n_devs = midiOutGetNumDevs();
    out_dev_names= new char* [n_devs];
    for(UINT i = 0; i < n_devs; i++)
    {
        if ( midiOutGetDevCaps( i, &OutCaps, sizeof(OutCaps) ) == MMSYSERR_NOERROR )
        {
            out_dev_names[i] = new char[DEVICENAME_LEN];
            strncpy( out_dev_names[i], OutCaps.szPname, DEVICENAME_LEN-1 );
            out_dev_names[i][DEVICENAME_LEN-1] = 0;
        }
    }
    return n_devs;
}

}
#endif

