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
#include "jdksmidi/sysex.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/file.h"

namespace jdksmidi
{

const char * MIDIMessage::chan_msg_name[16] =
{
    "CHANNEL MSG  ERROR 0x00",  // 0x00
    "CHANNEL MSG  ERROR 0x10",  // 0x10
    "CHANNEL MSG  ERROR 0x20",  // 0x20
    "CHANNEL MSG  ERROR 0x30",  // 0x30
    "CHANNEL MSG  ERROR 0x40",  // 0x40
    "CHANNEL MSG  ERROR 0x50",  // 0x50
    "CHANNEL MSG  ERROR 0x60",  // 0x60
    "CHANNEL MSG  ERROR 0x70",  // 0x70
    "NOTE OFF    ",  // 0x80
    "NOTE ON     ",  // 0x90
    "POLY PRES.  ",  // 0xA0
    "CTRL CHANGE ",  // 0xB0
    "PROG CHANGE ",  // 0xC0
    "CHAN PRES.  ",  // 0xD0
    "BENDER      ",  // 0xE0
    "CHANNEL MSG  ERROR 0xF0"   // 0xF0
};

const char * MIDIMessage::sys_msg_name[16] =
{
    "SYSTEM MSG   SYSEX-N     ",  // 0xF0
    "SYSTEM MSG   MTC         ",  // 0xF1
    "SYSTEM MSG   SONG POS    ",  // 0xF2
    "SYSTEM MSG   SONG SELECT ",  // 0xF3
    "SYSTEM MSG   ERROR 0xF4  ",  // 0xF4
    "SYSTEM MSG   ERROR 0xF5  ",  // 0xF5
    "SYSTEM MSG   TUNE REQ.   ",  // 0xF6
    "SYSTEM MSG   SYSEX-A     ",  // 0xF7
    "SYSTEM MSG   CLOCK       ",  // 0xF8
    "SYSTEM MSG   MEASURE END ",  // 0xF9
    "SYSTEM MSG   START       ",  // 0xFA
    "SYSTEM MSG   CONTINUE    ",  // 0xFB
    "SYSTEM MSG   STOP        ",  // 0xFC
    "SYSTEM MSG   ERROR 0xFD  ",  // 0xFD
    "SYSTEM MSG   SENSE       ",  // 0xFE
    "SYSTEM MSG   META-EVENT  "   // 0xFF
};

const char * MIDIMessage::service_msg_name[] =
{
    "SERVICE  ERROR NOT SERVICE", // NOT_SERVICE = 0,
    "SERVICE  BEAT MARKER", // SERVICE_BEAT_MARKER = 1,
    "SERVICE  NO OPERATION", // SERVICE_NO_OPERATION = 2,
    "SERVICE  USERAPP MARKER", // SERVICE_USERAPP_MARKER = 3,
    "SERVICE  ERROR INVALID SERVICE" // OUT_OF_RANGE_SERVICE_NUM = 4
};


const char * MIDIMessage::MsgToText ( char *txt ) const
{
    char buf[256];
    int len = GetLengthMSG();
    *txt = 0;

    if ( IsServiceMsg() )
    {
        unsigned int serv = GetServiceNum();
        if ( serv > OUT_OF_RANGE_SERVICE_NUM ) serv = OUT_OF_RANGE_SERVICE_NUM;
        sprintf ( buf, "%s  ", service_msg_name[ serv ] );
        strcat ( txt, buf );
        return txt;
    }

    if ( IsMetaEvent() ) // all Meta Events
    {
        sprintf ( buf, "%s ", sys_msg_name[ status - 0xF0 ] );
        strcat ( txt, buf );

        sprintf ( buf, "Type %02X  ", (int) byte1 ); // type of meta events
        strcat ( txt, buf );

        if ( len > 0 )
        {
            sprintf ( buf, "Data %02X  ", ( int ) byte2 );
            strcat ( txt, buf );
        }

        if ( len > 1 )
        {
            sprintf ( buf, "%02X  ", ( int ) byte3 );
            strcat ( txt, buf );
        }

        if ( len > 2 )
        {
            sprintf ( buf, "%02X  ", ( int ) byte4 );
            strcat ( txt, buf );
        }

        if ( len > 3 )
        {
            sprintf ( buf, "%02X  ", ( int ) byte5 );
            strcat ( txt, buf );
        }

        if ( len > 4 )
        {
            sprintf ( buf, "%02X  ", ( int ) byte6 );
            strcat ( txt, buf );
        }

        return txt;
    }

    if ( IsSystemExclusive() ) // all System Exclusive Events
    {
        sprintf ( buf, "%s  ", sys_msg_name[ status - 0xF0 ] );
        strcat ( txt, buf );
        return txt;
    }

    // else all Channel Events

    if ( IsAllNotesOff() )
    {
        sprintf ( buf, "Ch %2d  All Notes Off  Type %3d  Mode %3d ", (int)GetChannel()+1, (int)byte1, (int)byte2);
        strcat ( txt, buf );
        return txt;
    }

    int type = ( status & 0xF0 ) >> 4;

    sprintf ( buf, "Ch %2d  ", ( int ) GetChannel() + 1 );
    strcat ( txt, buf );

    sprintf ( buf, "%s  ", chan_msg_name[ type ] );
    strcat ( txt, buf );

    char *endtxt = txt + strlen ( txt );

    switch ( status & 0xf0 )
    {
    case NOTE_ON:
        if ( IsNoteOnV0() ) // velocity = 0: Note off
            sprintf ( endtxt, "Note %3d  Vel  %3d    (Note Off)  ", ( int ) byte1, ( int ) byte2 );
        else
            sprintf ( endtxt, "Note %3d  Vel  %3d  ", ( int ) byte1, ( int ) byte2 );
        break;

    case NOTE_OFF:
        sprintf ( endtxt, "Note %3d  Vel  %3d  ", ( int ) byte1, ( int ) byte2 );
        break;

    case POLY_PRESSURE:
        sprintf ( endtxt, "Note %3d  Pres %3d  ", ( int ) byte1, ( int ) byte2 );
        break;

    case CONTROL_CHANGE:
        if ( IsAllNotesOff() )
            sprintf ( endtxt, "Ctrl %3d  Val  %3d  (All Notes Off)  ", ( int ) byte1, ( int ) byte2 );
        else
            sprintf ( endtxt, "Ctrl %3d  Val  %3d  ", ( int ) byte1, ( int ) byte2 );
        break;

    case PROGRAM_CHANGE:
        sprintf ( endtxt, "PG   %3d  ", ( int ) byte1 );
        break;

    case CHANNEL_PRESSURE:
        sprintf ( endtxt, "Pres %3d  ", ( int ) byte1 );
        break;

    case PITCH_BEND:
        sprintf ( endtxt, "Val %5d  ", ( int ) GetBenderValue() );
        break;
    }

    return txt;
}

MIDIMessage::MIDIMessage()
{
    Clear();
}

MIDIMessage::MIDIMessage ( const MIDIMessage &m )
{
    status = m.status;
    byte1 = m.byte1;
    byte2 = m.byte2;
    byte3 = m.byte3;
    byte4 = m.byte4;
    byte5 = m.byte5;
    byte6 = m.byte6;
    data_length = m.data_length;
    service_num = m.service_num;
}

void MIDIMessage::Copy ( const MIDIMessage & m )
{
    *this = m;
}


//
// The equal operator
//

const MIDIMessage & MIDIMessage::operator = ( const MIDIMessage &m )
{
    status = m.status;
    byte1 = m.byte1;
    byte2 = m.byte2;
    byte3 = m.byte3;
    byte4 = m.byte4;
    byte5 = m.byte5;
    byte6 = m.byte6;
    data_length = m.data_length;
    service_num = m.service_num;
    return *this;
}

int MIDIMessage::GetLengthMSG() const
{
    if ( IsMetaEvent() ) // for all Meta Events
    {
        return data_length;
    }

    else if ( IsSystemMessage() ) // for all System Exclusive Events
    {
        return GetSystemMessageLength ( status );
    }

    else // for all Channel Events
    {
        return GetMessageLength ( status );
    }
}

short MIDIMessage::GetBenderValue() const
{
    return ( short ) ( ( ( byte2 << 7 ) | byte1 ) - 8192 );
}

unsigned short MIDIMessage::GetMetaValue() const
{
    return ( unsigned short ) ( ( byte3 << 8 ) | byte2 );
}

unsigned char MIDIMessage::GetTimeSigNumerator() const
{
    return byte2;
}

unsigned char MIDIMessage::GetTimeSigDenominator() const
{
    return byte3;
}

unsigned char MIDIMessage::GetTimeSigDenominatorPower() const
{
    return byte4;
}

signed char MIDIMessage::GetKeySigSharpFlats() const
{
    return ( signed char ) byte2;
}

unsigned char MIDIMessage::GetKeySigMajorMinor() const
{
    return byte3;
}

bool MIDIMessage::IsNoteOn() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == NOTE_ON );
}

bool MIDIMessage::IsNoteOff() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == NOTE_OFF );
}

bool MIDIMessage::IsPolyPressure() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == POLY_PRESSURE );
}

bool MIDIMessage::IsControlChange() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == CONTROL_CHANGE );
}

bool MIDIMessage::IsProgramChange() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == PROGRAM_CHANGE );
}

bool MIDIMessage::IsChannelPressure() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == CHANNEL_PRESSURE );
}

bool MIDIMessage::IsPitchBend() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == PITCH_BEND );
}

bool MIDIMessage::IsSystemMessage() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status & 0xf0 ) == 0xf0;
}

bool MIDIMessage::IsSysExN() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == SYSEX_START_N );
}

bool MIDIMessage::IsSysExURT() const
{
    return IsSysExN() && ( byte1 == 0x7F );
}

int MIDIMessage::GetSysExURTdevID() const
{
    return byte2;
}

int MIDIMessage::GetSysExURTsubID() const
{
    return byte3;
}

bool MIDIMessage::IsSysExA() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == SYSEX_START_A );
}

bool MIDIMessage::IsMTC() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == MTC );
}

bool MIDIMessage::IsSongPosition() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == SONG_POSITION );
}

bool MIDIMessage::IsSongSelect() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == SONG_SELECT );
}

bool  MIDIMessage::IsTuneRequest() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == TUNE_REQUEST );
}

bool MIDIMessage::IsMetaEvent() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT );
}

bool MIDIMessage::IsChannelMsg() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status >= 0x80 ) && ( status < 0xF0 );
}

bool MIDIMessage::IsTextEvent() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT ) &&
           ( byte1 >= 0x01 && byte1 <= 0x0F );
}

bool MIDIMessage::IsAllNotesOff() const
{
    return ( service_num == NOT_SERVICE) &&
           ( ( status & 0xf0 ) == CONTROL_CHANGE )
           && ( byte1 >= C_ALL_NOTES_OFF );
}

bool MIDIMessage::IsChannelPrefix() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT ) &&
           ( byte1 == META_CHANNEL_PREFIX );
}

bool MIDIMessage::IsTempo() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT ) &&
           ( byte1 == META_TEMPO );
}

bool MIDIMessage::IsDataEnd() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT ) &&
           ( byte1 == META_END_OF_TRACK );
}

bool MIDIMessage::IsTimeSig() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT ) &&
           ( byte1 == META_TIMESIG );
}

bool MIDIMessage::IsKeySig() const
{
    return ( service_num == NOT_SERVICE) &&
           ( status == META_EVENT ) &&
           ( byte1 == META_KEYSIG );
}

bool  MIDIMessage::IsUserAppMarker() const
{
    return ( service_num == SERVICE_USERAPP_MARKER );
}

bool MIDIMessage::IsBeatMarker() const
{
    return ( service_num == SERVICE_BEAT_MARKER );
}

unsigned long MIDIMessage::GetTempo() const
{
    return MIDIFile::To32Bit ( 0, byte2, byte3, byte4 );
}

unsigned long MIDIMessage::GetTempo32() const
{
    // tempo is in microseconds per beat
    unsigned long tempo = GetTempo();
    if ( tempo == 0 )
        tempo = 1;
    /*
        // calculate beats per second by
        double beats_per_second = 1e6 / ( double ) tempo;// 1 million microseconds per second
        double beats_per_minute = beats_per_second * 60.;
        unsigned long tempo_bpm_times_32 = (unsigned long) ( 0.5 + beats_per_minute * 32. );
    */
    unsigned long tempo_bpm_times_32 = (unsigned long) ( 0.5 + (32*60*1e6) / ( double ) tempo );
    return tempo_bpm_times_32;
}

unsigned short MIDIMessage::GetLoopNumber() const
{
    return GetMetaValue();
}

void MIDIMessage::SetBenderValue ( short v )
{
    short x = ( short ) ( v + 8192 );
    byte1 = ( unsigned char ) ( x & 0x7f );
    byte2 = ( unsigned char ) ( ( x >> 7 ) & 0x7f );
}

void MIDIMessage::SetMetaType ( unsigned char t )
{
    byte1 = t;
}

void MIDIMessage::SetMetaValue ( unsigned short v )
{
    byte2 = ( unsigned char ) ( v & 0xff );
    byte3 = ( unsigned char ) ( ( v >> 8 ) & 0xff );
}

void MIDIMessage::SetNoteOn ( unsigned char chan, unsigned char note, unsigned char vel )
{
    Clear();
    status = ( unsigned char ) ( chan | NOTE_ON );
    byte1 = note;
    byte2 = vel;
}

void MIDIMessage::SetNoteOff ( unsigned char chan, unsigned char note, unsigned char vel )
{
    Clear();
    status = ( unsigned char ) ( chan | NOTE_OFF );
    byte1 = note;
    byte2 = vel;
}

void MIDIMessage::SetPolyPressure ( unsigned char chan, unsigned char note, unsigned char pres )
{
    Clear();
    status = ( unsigned char ) ( chan | POLY_PRESSURE );
    byte1 = note;
    byte2 = pres;
}

void MIDIMessage::SetControlChange ( unsigned char chan, unsigned char ctrl, unsigned char val )
{
    Clear();
    status = ( unsigned char ) ( chan | CONTROL_CHANGE );
    byte1 = ctrl;
    byte2 = val;
}

double MIDIMessage::GetPan()
{
    int val = GetControllerValue(); // 0 = leftmost, 64 = centre, 127 = rightmost
    if (val == 127) val = 128;
    return (val-64)/64.;
}

void MIDIMessage::SetPan( unsigned char chan, double pan )
{
    //     leftmost  centre   rightmost
    //  pan = -1 ...    0 ...    +1
    // ipan =  0 ... 8192 ... 16384
    int ipan = jdks_float2int( 8192. * (pan + 1.) );
    if ( ipan > 16383 ) ipan = 16383;

    int pan_msb = ipan / 128;
//  int pan_lsb = ipan % 128;

    SetControlChange( chan, C_PAN, pan_msb );
//  Russian (windows 1251):
//  к сожалению любое pan_lsb сбрасывает панораму в центр при проигрывании midi
//  и через MediaPlayer и даже через Timidity, поэтому не делаем установку lsb
//  English:
//  unfortunately any pan_lsb drops panorama to the center with the playback of midi file
//  through MediaPlayer and even through Timidity; therefore we do not make the set of lsb
//  SetControlChange( chan, C_PAN + C_LSB, pan_lsb ); // don't work...
}

void MIDIMessage::SetProgramChange ( unsigned char chan, unsigned char val )
{
    Clear();
    status = ( unsigned char ) ( chan | PROGRAM_CHANGE );
    byte1 = val;
}

void MIDIMessage::SetChannelPressure ( unsigned char chan, unsigned char val )
{
    Clear();
    status = ( unsigned char ) ( chan | CHANNEL_PRESSURE );
    byte1 = val;
}

void MIDIMessage::SetPitchBend ( unsigned char chan, short val )
{
    Clear();
    status = ( unsigned char ) ( chan | PITCH_BEND );
    val += ( short ) 0x2000; // center value
    byte1 = ( unsigned char ) ( val & 0x7f ); // 7 bit bytes
    byte2 = ( unsigned char ) ( ( val >> 7 ) & 0x7f );
}

void MIDIMessage::SetPitchBend ( unsigned char chan, unsigned char low, unsigned char high )
{
    Clear();
    status = ( unsigned char ) ( chan | PITCH_BEND );
    byte1 = ( unsigned char ) ( low );
    byte2 = ( unsigned char ) ( high );
}

void MIDIMessage::SetSysEx( unsigned char type )
{
    Clear();
    status = type; // SYSEX_START or SYSEX_START_A
}

void MIDIMessage::SetMTC ( unsigned char field, unsigned char v )
{
    Clear();
    status = MTC;
    byte1 = ( unsigned char ) ( ( field << 4 ) | v );
}

void MIDIMessage::SetSongPosition ( short pos )
{
    Clear();
    status = SONG_POSITION;
    byte1 = ( unsigned char ) ( pos & 0x7f );
    byte2 = ( unsigned char ) ( ( pos >> 7 ) & 0x7f );
}

void MIDIMessage::SetSongSelect ( unsigned char sng )
{
    Clear();
    status = SONG_SELECT;
    byte1 = sng;
}

void MIDIMessage::SetTuneRequest()
{
    Clear();
    status = TUNE_REQUEST;
}

void MIDIMessage::SetMetaEvent ( unsigned char type, unsigned char v1, unsigned char v2 )
{
    Clear();
    status = META_EVENT;
    byte1 = type;
    byte2 = v1;
    byte3 = v2;
}

void MIDIMessage::SetMetaEvent ( unsigned char type, unsigned short v )
{
    unsigned char v1 = ( unsigned char ) ( v & 0xff );
    unsigned char v2 = ( unsigned char ) ( ( v >> 8 ) & 0xff );
    SetMetaEvent ( type, v1, v2 );
}

void MIDIMessage::SetAllNotesOff (unsigned char chan, unsigned char type, unsigned char mode)
{
    Clear();
    status = ( unsigned char ) ( chan | CONTROL_CHANGE );
    byte1 = type;
    byte2 = mode;
//  byte2 = 0x7f; // was
}

void MIDIMessage::SetLocal ( unsigned char chan, unsigned char v )
{
    Clear();
    status = ( unsigned char ) ( chan | CONTROL_CHANGE );
    byte1 = C_LOCAL;
    byte2 = v;
}

void MIDIMessage::SetTempo ( unsigned long tempo )
{
    int a, b, c;
    c = tempo & 0xFF;
    b = (tempo >> 8) & 0xFF;
    a = (tempo >> 16) & 0xFF;
    SetMetaEvent ( META_TEMPO, a, b );
    SetByte4( c );
}

void MIDIMessage::SetTempo32 ( unsigned long tempo_times_32 )
{
    unsigned long tempo = (unsigned long) ( 0.5 + (32*60*1e6) / ( double ) tempo_times_32 );
    SetTempo ( tempo );
}

void MIDIMessage::SetText ( unsigned short text_num, unsigned char type )
{
    SetMetaEvent ( type, text_num );
}

void MIDIMessage::SetDataEnd()
{
    SetMetaEvent ( META_END_OF_TRACK, 0 );
}

void MIDIMessage::SetTimeSig (
    unsigned char numerator,
    unsigned char denominator_power,
    unsigned char midi_clocks_per_metronome,
    unsigned char num_32nd_per_midi_quarter_note )
{
    int denominator = 1 << denominator_power;
    // forward to msg denominator instead denominator power
    // set numerator in msg byte2, denominator in byte3
    SetMetaEvent ( META_TIMESIG, numerator, denominator );
    SetByte4( denominator_power ); // also add original denominator power in byte4
    SetByte5( midi_clocks_per_metronome );
    SetByte6( num_32nd_per_midi_quarter_note );
}

void MIDIMessage::SetKeySig ( signed char sharp_flats, unsigned char major_minor )
{
    SetMetaEvent ( META_KEYSIG, sharp_flats, major_minor );
}

void MIDIMessage::SetBeatMarker()
{
    Clear();
    service_num = SERVICE_BEAT_MARKER;
}

void MIDIMessage::SetUserAppMarker()
{
    Clear();
    service_num = SERVICE_USERAPP_MARKER;
}


MIDIBigMessage::MIDIBigMessage()
    :
    sysex ( 0 )
{
}

MIDIBigMessage::MIDIBigMessage ( const MIDIBigMessage &m )
    :
    MIDIMessage ( m ),
    sysex ( 0 )
{
    if ( m.sysex )
    {
        sysex = new MIDISystemExclusive ( *m.sysex );
    }
}

MIDIBigMessage::MIDIBigMessage ( const MIDIMessage &m )
    :
    MIDIMessage ( m ),
    sysex ( 0 )
{
}

MIDIBigMessage::MIDIBigMessage ( const MIDIMessage &m, const MIDISystemExclusive *e )
    :
    MIDIMessage ( m ),
    sysex ( 0 )
{
    CopySysEx( e );
}

void MIDIBigMessage::Clear()
{
    ClearSysEx();
    MIDIMessage::Clear();
}

//
// destructors
//

MIDIBigMessage::~MIDIBigMessage()
{
    Clear();
}

//
// operator =
//

const MIDIBigMessage &MIDIBigMessage::operator = ( const MIDIBigMessage &m )
{
    delete sysex;
    sysex = 0;
    if ( m.sysex )
        sysex = new MIDISystemExclusive ( *m.sysex );

    MIDIMessage::operator = ( m );
    return *this;
}

const MIDIBigMessage &MIDIBigMessage::operator = ( const MIDIMessage &m )
{
    delete sysex;
    sysex = 0;
    MIDIMessage::operator = ( m );
    return *this;
}

void MIDIBigMessage::Copy ( const MIDIMessage &m )
{
    *this = m;
}

void MIDIBigMessage::Copy ( const MIDIBigMessage &m )
{
    *this = m;
}

//
// 'Get' methods
//

MIDISystemExclusive *MIDIBigMessage::GetSysEx()
{
    return sysex;
}

const MIDISystemExclusive *MIDIBigMessage::GetSysEx() const
{
    return sysex;
}

//
// 'Set' methods
//

void MIDIBigMessage::CopySysEx ( const MIDISystemExclusive *e )
{
    ClearSysEx();
    if ( e )
    {
        sysex = new MIDISystemExclusive ( *e );
    }
}

#if 0
void MIDIBigMessage::SetSysEx ( MIDISystemExclusive *e )
{
    delete sysex;
    sysex = e;
}
#endif

void MIDIBigMessage::ClearSysEx()
{
    jdks_safe_delete_object( sysex );
}






//
// Constructors
//

MIDITimedMessage::MIDITimedMessage()
    : time ( 0 )
{
}

MIDITimedMessage::MIDITimedMessage ( const MIDITimedMessage &m )
    : MIDIMessage ( m ), time ( m.GetTime() )
{
}

MIDITimedMessage::MIDITimedMessage ( const MIDIMessage &m )
    : MIDIMessage ( m ), time ( 0 )
{
}

void MIDITimedMessage::Clear()
{
    time = 0;
    MIDIMessage::Clear();
}

void MIDITimedMessage::Copy ( const MIDITimedMessage &m )
{
    *this = m;
}

//
// operator =
//

const MIDITimedMessage &MIDITimedMessage::operator = ( const MIDITimedMessage & m )
{
    time = m.GetTime();
    MIDIMessage::operator = ( m );
    return *this;
}

const MIDITimedMessage &MIDITimedMessage::operator = ( const MIDIMessage & m )
{
    time = 0;
    MIDIMessage::operator = ( m );
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDITimedMessage::GetTime() const
{
    return time;
}

//
// 'Set' methods
//

void MIDITimedMessage::SetTime ( MIDIClockTime t )
{
    time = t;
}

int  MIDITimedMessage::CompareEvents (
    const MIDITimedMessage &m1,
    const MIDITimedMessage &m2
)
{
    bool n1 = m1.IsNoOp();
    bool n2 = m2.IsNoOp();
    // NOP's always are larger.

    if ( n1 && n2 )
        return 0; // same, do not care.

    if ( n2 )
        return 2; // m2 is larger

    if ( n1 )
        return 1; // m1 is larger

    if ( m1.GetTime() > m2.GetTime() )
        return 1; // m1 is larger

    if ( m2.GetTime() > m1.GetTime() )
        return 2; // m2 is larger

    // if times are the same, a note off is always larger

    if ( m1.byte1 == m2.byte1
            && m1.GetStatus() == NOTE_ON
            && ( ( m2.GetStatus() == NOTE_ON && m2.byte2 == 0 ) || ( m2.GetStatus() == NOTE_OFF ) )
       )
        return 2; // m2 is larger

    if ( m1.byte1 == m2.byte1
            && m2.GetStatus() == NOTE_ON
            && ( ( m1.GetStatus() == NOTE_ON && m1.byte2 == 0 ) || ( m1.GetStatus() == NOTE_OFF ) )
       )
        return 1; // m1 is larger

    return 0;  // both are equal.
}



MIDIDeltaTimedMessage::MIDIDeltaTimedMessage()
    : dtime ( 0 )
{
}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage ( const MIDIDeltaTimedMessage &m )
    : MIDIMessage ( m ), dtime ( m.GetDeltaTime() )
{
}

MIDIDeltaTimedMessage::MIDIDeltaTimedMessage ( const MIDIMessage &m )
    : MIDIMessage ( m ), dtime ( 0 )
{
}

void MIDIDeltaTimedMessage::Clear()
{
    dtime = 0;
    MIDIMessage::Clear();
}

void MIDIDeltaTimedMessage::Copy ( const MIDIDeltaTimedMessage &m )
{
    dtime = m.GetDeltaTime();
    MIDIMessage::Copy ( m );
}

//
// operator =
//

const MIDIDeltaTimedMessage &MIDIDeltaTimedMessage::operator = ( const MIDIDeltaTimedMessage &m )
{
    dtime = m.GetDeltaTime();
    MIDIMessage::operator = ( m );
    return *this;
}

const MIDIDeltaTimedMessage &MIDIDeltaTimedMessage::operator = ( const MIDIMessage &m )
{
    dtime = 0;
    MIDIMessage::operator = ( m );
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDIDeltaTimedMessage::GetDeltaTime() const
{
    return dtime;
}

//
// 'Set' methods
//

void MIDIDeltaTimedMessage::SetDeltaTime ( MIDIClockTime t )
{
    dtime = t;
}


//
// Constructors
//

MIDITimedBigMessage::MIDITimedBigMessage()
    : time ( 0 )
{
}

MIDITimedBigMessage::MIDITimedBigMessage ( const MIDITimedBigMessage &m )
    : MIDIBigMessage ( m ),
      time ( m.GetTime() )
{
}

MIDITimedBigMessage::MIDITimedBigMessage ( const MIDIBigMessage &m )
    : MIDIBigMessage ( m ),
      time ( 0 )
{
}

MIDITimedBigMessage::MIDITimedBigMessage ( const MIDITimedMessage &m )
    : MIDIBigMessage ( m ),
      time ( m.GetTime() )
{
}

MIDITimedBigMessage::MIDITimedBigMessage ( const MIDIMessage &m )
    : MIDIBigMessage ( m ),
      time ( 0 )
{
}

MIDITimedBigMessage::MIDITimedBigMessage ( const MIDITimedMessage &m, const MIDISystemExclusive *e )
    : MIDIBigMessage ( m, e ),
      time ( m.GetTime() )
{
}

void MIDITimedBigMessage::Clear()
{
    time = 0;
    MIDIBigMessage::Clear();
}

void MIDITimedBigMessage::Copy ( const MIDITimedBigMessage &m )
{
    *this = m;
}

void MIDITimedBigMessage::Copy ( const MIDITimedMessage &m )
{
    *this = m;
}

//
// operator =
//

const MIDITimedBigMessage &MIDITimedBigMessage::operator = ( const MIDITimedBigMessage & m )
{
    time = m.GetTime();
    MIDIBigMessage::operator = ( m );
    return *this;
}

const MIDITimedBigMessage &MIDITimedBigMessage::operator = ( const MIDITimedMessage & m )
{
    time = m.GetTime();
    MIDIBigMessage::operator = ( m );
    return *this;
}

const MIDITimedBigMessage &MIDITimedBigMessage::operator = ( const MIDIMessage & m )
{
    time = 0;
    MIDIBigMessage::operator = ( m );
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDITimedBigMessage::GetTime() const
{
    return time;
}

//
// 'Set' methods
//

void MIDITimedBigMessage::SetTime ( MIDIClockTime t )
{
    time = t;
}



int  MIDITimedBigMessage::CompareEvents (
    const MIDITimedBigMessage &m1,
    const MIDITimedBigMessage &m2
)
{
    bool n1 = m1.IsNoOp();
    bool n2 = m2.IsNoOp();
    // NOP's always are larger.

    if ( n1 && n2 )
        return 0; // same, do not care.

    if ( n2 )
        return 2; // m2 is larger

    if ( n1 )
        return 1; // m1 is larger

    if ( m1.GetTime() > m2.GetTime() )
        return 1; // m1 is larger

    if ( m2.GetTime() > m1.GetTime() )
        return 2; // m2 is larger

    // if times are the same, a note off is always larger

    if ( m1.byte1 == m2.byte1
            && m1.GetStatus() == NOTE_ON
            && ( ( m2.GetStatus() == NOTE_ON && m2.byte2 == 0 ) || ( m2.GetStatus() == NOTE_OFF ) )
       )
        return 2; // m2 is larger

    if ( m1.byte1 == m2.byte1
            && m2.GetStatus() == NOTE_ON
            && ( ( m1.GetStatus() == NOTE_ON && m1.byte2 == 0 ) || ( m1.GetStatus() == NOTE_OFF ) )
       )
        return 1; // m1 is larger

    return 0;  // both are equal.
}

//
// Constructors
//

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage()
    : dtime ( 0 )
{
}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage ( const MIDIDeltaTimedBigMessage &m )
    : MIDIBigMessage ( m ), dtime ( m.GetDeltaTime() )
{
}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage ( const MIDIBigMessage &m )
    : MIDIBigMessage ( m ), dtime ( 0 )
{
}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage ( const MIDIMessage &m )
    : MIDIBigMessage ( m ), dtime ( 0 )
{
}

MIDIDeltaTimedBigMessage::MIDIDeltaTimedBigMessage ( const MIDIDeltaTimedMessage &m )
    : MIDIBigMessage ( m ), dtime ( m.GetDeltaTime() )
{
}

void MIDIDeltaTimedBigMessage::Clear()
{
    dtime = 0;
    MIDIBigMessage::Clear();
}

void MIDIDeltaTimedBigMessage::Copy ( const MIDIDeltaTimedBigMessage &m )
{
    dtime = m.GetDeltaTime();
    MIDIBigMessage::Copy ( m );
}

void MIDIDeltaTimedBigMessage::Copy ( const MIDIDeltaTimedMessage &m )
{
    dtime = m.GetDeltaTime();
    MIDIBigMessage::Copy ( m );
}

//
// operator =
//

const MIDIDeltaTimedBigMessage &MIDIDeltaTimedBigMessage::operator = ( const MIDIDeltaTimedBigMessage &m )
{
    dtime = m.GetDeltaTime();
    MIDIBigMessage::operator = ( m );
    return *this;
}

const MIDIDeltaTimedBigMessage &MIDIDeltaTimedBigMessage::operator = ( const MIDIDeltaTimedMessage &m )
{
    dtime = m.GetDeltaTime();
    MIDIBigMessage::operator = ( m );
    return *this;
}

const MIDIDeltaTimedBigMessage &MIDIDeltaTimedBigMessage::operator = ( const MIDIMessage &m )
{
    dtime = 0;
    MIDIBigMessage::operator = ( m );
    return *this;
}

//
// 'Get' methods
//

MIDIClockTime MIDIDeltaTimedBigMessage::GetDeltaTime() const
{
    return dtime;
}

//
// 'Set' methods
//

void MIDIDeltaTimedBigMessage::SetDeltaTime ( MIDIClockTime t )
{
    dtime = t;
}


// friend operators

bool operator == ( const MIDIMessage &m1, const MIDIMessage &m2 )
{
    if ( m1.GetServiceNum() != m2.GetServiceNum() ) return false;
    // else equal service_num values
    if ( m1.GetServiceNum() != NOT_SERVICE ) return true; // equal services
    // else == NOT_SERVICE, normal messages
    return (  m1.GetStatus() == m2.GetStatus() &&
              m1.GetByte1() == m2.GetByte1() &&
              m1.GetByte2() == m2.GetByte2() &&
              m1.GetByte3() == m2.GetByte3() &&
              m1.GetByte4() == m2.GetByte4() &&
              m1.GetByte5() == m2.GetByte5() &&
              m1.GetByte6() == m2.GetByte6() &&
              m1.GetDataLength() == m2.GetDataLength()
           );
}

bool operator == ( const MIDITimedMessage &m1, const MIDITimedMessage &m2 )
{
    if ( m1.GetTime() != m2.GetTime() )
        return false;

    return ( (MIDIMessage) m1 ) == ( (MIDIMessage) m2 );
}

bool operator == ( const MIDIBigMessage &m1, const MIDIBigMessage &m2 )
{
    const MIDISystemExclusive *e1 = m1.GetSysEx();
    const MIDISystemExclusive *e2 = m2.GetSysEx();

    if ( e1 != 0 )
    {
        if ( e2 == 0 )
            return false;
    }
    else // e1 == 0
    {
        if ( e2 != 0 )
            return false;
    }

    if ( e1 != 0 && e2 != 0 )
    {
        if ( !( *e1 == *e2 ) )
            return false;
    }

    return ( (MIDIMessage) m1 ) == ( (MIDIMessage) m2 );
}

bool operator == ( const MIDITimedBigMessage &m1, const MIDITimedBigMessage &m2 )
{
    if ( m1.GetTime() != m2.GetTime() )
        return false;

    return ( (MIDIBigMessage) m1 ) == ( (MIDIBigMessage) m2 );
}

}
