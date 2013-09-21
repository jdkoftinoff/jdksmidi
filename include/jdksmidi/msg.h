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

//
// MODIFIED by N. Cassetta
// (added member function GetMarkerText() to MIDIMessage
//

#ifndef JDKSMIDI_MSG_H
#define JDKSMIDI_MSG_H

#include "jdksmidi/midi.h"
#include "jdksmidi/tempo.h"
#include "jdksmidi/sysex.h"

namespace jdksmidi
{

class MIDIMessage;
class MIDIBigMessage;

class MIDITimedMessage;
class MIDIDeltaTimedMessage;

class MIDITimedBigMessage;
class MIDIDeltaTimedBigMessage;

///
/// The MIDIMessage class is a simple, lightweight container which can hold a single
/// MIDI Message that can fit within 7 bytes plus status byte.  It can also hold some
/// non-MIDI messages, known as Meta messages like No-op, Key signature, Time Signature, etc,
/// which are useful for internal processing.
///
/// This class is the base class
/// for a number of MIDIMessage variants, such as the MIDIBigMessage, MIDITimedMessage,
/// MIDITimedBigMessage, and MIDIDeltaTimedBigMessage.  This could be a good candidate
/// for using a mix-in architecture ( see http://en.wikipedia.org/wiki/Mixin ) via
/// multiple inheritance, but at the time this was written in 1990, C++ compilers typically
/// had problems with MI.
///

class MIDIMessage
{
public:

    ///@name The Constructors and Initializing methods
    //@{

    MIDIMessage(); ///< Create a MIDIMessage object which holds no values.

    MIDIMessage ( const MIDIMessage &m ); ///< Copy Constructor.

    const MIDIMessage & operator = ( const MIDIMessage &m ); ///< The assignment operator. Copies the MIDIMessage value.

    void Clear() ///< Set the MIDIMessage object to 0
    {
        status = 0;
        byte1 = byte2 = byte3 = 0;
        byte4 = byte5 = byte6 = 0;
        data_length = 0;
        service_num = NOT_SERVICE;
    }

    void Copy ( const MIDIMessage & m ); ///< Copy the value of the specified MIDIMessage.

    //@}

    const char * MsgToText ( char *txt ) const; ///< Create a human readable ascii string describing the message.  This is potentially unsafe as the 'txt' param must point to a buffer of at least 64 chars long.

    ///@name The Query methods.
    //@{

    int GetLengthMSG() const;

    int GetLength() const ///< Get the length in bytes of the entire message.
    {
        return GetLengthMSG();
    }

    /// Get the status byte of the message.
    unsigned char GetStatus() const
    {
        return ( unsigned char ) status;
    }

    /// If the message is a channel message, this method returns the MIDI channel (0...15) that the message is on.
    unsigned char GetChannel() const
    {
        return ( unsigned char ) ( status & 0x0F );
    }

    /// If the message is a channel message, this method returns the relevant top 4 bits which describe what type of channel message it is.
    unsigned char GetType() const
    {
        return ( unsigned char ) ( status & 0xF0 );
    }

    /// If the message is some sort of meta-message, then GetMetaType returns the type byte.
    unsigned char GetMetaType() const
    {
        return byte1;
    }

    /// Access to the raw byte1 of the message
    unsigned char GetByte1() const
    {
        return byte1;
    }

    /// Access to the raw byte2 of the message
    unsigned char GetByte2() const
    {
        return byte2;
    }

    /// Access to the raw byte3 of the message
    unsigned char GetByte3() const
    {
        return byte3;
    }

    /// Access to the raw byte4 of the message
    unsigned char GetByte4() const
    {
        return byte4;
    }

    /// Access to the raw byte5 of the message
    unsigned char GetByte5() const
    {
        return byte5;
    }

    /// Access to the raw byte6 of the message
    unsigned char GetByte6() const
    {
        return byte6;
    }

    /// Access to the raw data_length byte of the message
    unsigned char GetDataLength() const
    {
        return data_length;
    }

    /// Access to the raw service_num data of the message
    unsigned int GetServiceNum() const
    {
        return service_num;
    }

    /// If the message is a note on, note off, or poly aftertouch message, GetNote() returns the note number
    unsigned char GetNote() const
    {
        return byte1;
    }

    /// If the message is a note on, note off, or poly aftertouch message, GetVelocity() returns the velocity or pressure
    unsigned char GetVelocity() const
    {
        return byte2;
    }

    /// If the message is a channel pressure message, GetChannelPressure() returns the pressure value.
    unsigned char GetChannelPressure() const
    {
        return byte1;
    }

    /// If the message is a 7 bit program change value, GetPGValue() returns the program number.
    unsigned char GetPGValue() const
    {
        return byte1;
    }

    /// If the message is a control change message, GetController() returns the controller number.
    unsigned char GetController() const
    {
        return byte1;
    }

    /// If the message is a control change message, GetControllerValue() returns the 7 bit controller value.
    unsigned char GetControllerValue() const
    {
        return byte2;
    }

    /// If the message is a bender message, GetBenderValue() returns the signed 14 bit bender value.
    short GetBenderValue() const;

    /// If the message is a meta-message, GetMetaValue() returns the unsigned 14 bit value attached.
    unsigned short GetMetaValue() const;

    /// If the message is a time signature meta-message, GetTimeSigNumerator() returns the numerator of the time signature.
    unsigned char GetTimeSigNumerator() const;

    /// If the message is a time signature meta-message, GetTimeSigDenominator() returns the denominator of the time signature.
    unsigned char GetTimeSigDenominator() const;

    /// If the message is a time signature meta-message, GetTimeSigDenominatorPower() returns the denominator power of the time signature.
    unsigned char GetTimeSigDenominatorPower() const;

    /// If the message is a key signature meta-message, GetKeySigSharpFlats() returns to standard midi file form of the key. Negative values means that many flats, positive numbers means that many sharps.
    signed char GetKeySigSharpFlats() const;

    /// If the message is a key signature meta-message, GetKeySigMajorMinor() returns to standard midi file form of the key major/minor flag. 0 means a major key, 1 means a minor key.
    unsigned char GetKeySigMajorMinor() const;

    // not midi message
    bool IsServiceMsg() const
    {
        return service_num != NOT_SERVICE;
    }

    /// If the message is some sort of real time channel message, IsChannelMsg() will return true. You can then call GetChannel() for more information.
    bool IsChannelMsg() const;

    /// If the message is a note on message, IsNoteOn() will return true. You can then call GetChannel(), GetNote() and GetVelocity() for further information.
    bool IsNoteOn() const;

    /// If the message is a note off message, IsNoteOff() will return true. You can then call GetChannel(), GetNote() and GetVelocity() for further information.
    bool IsNoteOff() const;

    /// If the message is a note on message and velocity=0 (i.e. note off), IsNoteOnV0() will return true. You can then call GetChannel(), GetNote() for further information.
    bool IsNoteOnV0() const
    {
        return IsNoteOn() && GetVelocity() == 0;
    }

    bool IsNote() const
    {
        return IsNoteOn() || IsNoteOff();
    }

    bool ImplicitIsNoteOn() const
    {
        return IsNoteOn() && GetVelocity() != 0;
    }

    bool ImplicitIsNoteOff() const
    {
        return IsNoteOff() || IsNoteOnV0();
    }

    /// If the message is a polyphonic pressure chanel message, IsPolyPressure() will return true. You can then call GetChannel(), GetNote() and GetVelocity() for further informtion.
    bool IsPolyPressure() const;

    /// If the message is a control change message, IsControlChange() will return true. You can then call GetChannel(), GetController() and GetControllerValue() for further information.
    bool IsControlChange() const;

    // panorama msg
    bool IsPanChange() const
    {
        return IsControlChange() && GetController() == C_PAN;
    }

    /// If the message is a program change message, IsProgramChange() will return true.  You can then call GetChannel() and GetPGValue() for further information.
    bool IsProgramChange() const;

    /// If the message is a channel pressure change message, IsChannelPressure() will return true. You can then call GetChannel() and GetChannelPressure() for further information.
    bool IsChannelPressure() const;

    /// If the message is a bender message, IsPitchBend() will return true. You can then call GetChannel() and GetBenderValue() for further information
    bool IsPitchBend() const;

    /// If the message is a system message (the status byte is 0xf0 or higher), IsSystemMessage() will return true.
    bool IsSystemMessage() const;

    /// If the message is a normal system exclusive marker, IsSysExN() will return true.
    /// \note Sysex messages are not stored in the MIDIMessage object. \see MIDIBigMessage
    bool IsSysExN() const; // Normal SysEx Event

    bool IsSysExURT() const; // Universal Real Time System Exclusive message, URT sysex

    int GetSysExURTdevID() const; // return Device ID code for URT sysex

    int GetSysExURTsubID() const; // return Sub ID code for URT sysex

    bool IsSysExA() const; // Authorization SysEx Event

    // TODO@VRM note to Jeff:
    // code with old fun IsSysEx() need to rewrite manually, because now it's two func: IsSysExN() and IsSysExA()
    bool IsSystemExclusive() const
    {
        return ( IsSysExN() || IsSysExA() );
    }

    bool IsMTC() const;

    bool IsSongPosition() const;

    bool IsSongSelect() const;

    bool IsTuneRequest() const;

    bool IsMetaEvent() const;

    bool IsChannelEvent() const
    {
        return IsChannelMsg();
    }

    bool IsTextEvent() const;

    bool IsLyricText() const
    {
        return ( IsTextEvent() && GetMetaType() == META_LYRIC_TEXT );
    }

    bool IsTrackName() const
    {
        return ( IsTextEvent() && GetMetaType() == META_TRACK_NAME );
    }

    bool IsMarkerText() const                       // new by NC //
    {
        return ( IsTextEvent() && GetMetaType() == META_MARKER_TEXT );
    }

    bool IsAllNotesOff() const;

    bool IsNoOp() const
    {
        return ( service_num == SERVICE_NO_OPERATION );
    }

    bool IsChannelPrefix() const;

    bool IsTempo() const;

    bool IsDataEnd() const;
    bool IsEndOfTrack() const
    {
        return IsDataEnd();
    }

    bool IsTimeSig() const;

    bool IsKeySig() const;

    bool IsBeatMarker() const;

    bool IsUserAppMarker() const;

    ///
    /// GetTempo32() returns the tempo value in 1/32 bpm
    ///
    unsigned long GetTempo32() const;

    // GetTempo() returns the original midifile tempo value (microseconds per beat)
    unsigned long GetTempo() const;

    unsigned short GetLoopNumber() const;

    //@}

    ///@name The 'Set' methods
    //@{

    /// Set all bits of the status byte
    void SetStatus ( unsigned char s )
    {
        status = s;
    }

    /// set just the lower 4 bits of the status byte without changing the upper 4 bits
    void SetChannel ( unsigned char s )
    {
        status = ( unsigned char ) ( ( status & 0xf0 ) | s );
    }

    /// set just the upper 4 bits of the status byte without changing the lower 4 bits
    void SetType ( unsigned char s )
    {
        status = ( unsigned char ) ( ( status & 0x0f ) | s );
    }

    /// Set the value of the data byte 1
    void SetByte1 ( unsigned char b )
    {
        byte1 = b;
    }

    /// Set the value of the data byte 2
    void SetByte2 ( unsigned char b )
    {
        byte2 = b;
    }

    /// Set the value of the data byte 3
    void SetByte3 ( unsigned char b )
    {
        byte3 = b;
    }

    /// Set the value of the data byte 4
    void SetByte4 ( unsigned char b )
    {
        byte4 = b;
    }

    /// Set the value of the data byte 5
    void SetByte5 ( unsigned char b )
    {
        byte5 = b;
    }

    /// Set the value of the data byte 6
    void SetByte6 ( unsigned char b )
    {
        byte6 = b;
    }

    /// Set the value of the data byte data_length
    void SetDataLength ( unsigned char b )
    {
        data_length = b;
    }

    /// Set the note number for note on, note off, and polyphonic aftertouch messages
    void SetNote ( unsigned char n )
    {
        byte1 = n;
    }

    /// Set the velocity of a note on or note off message
    void SetVelocity ( unsigned char v )
    {
        byte2 = v;
    }

    /// Set the program number of a program change message
    void SetPGValue ( unsigned char v )
    {
        byte1 = v;
    }

    /// Set the controller number of a control change message
    void SetController ( unsigned char c )
    {
        byte1 = c;
    }

    /// Set the 7 bit controller value of a control change message
    void SetControllerValue ( unsigned char v )
    {
        byte2 = v;
    }

    /// Set the signed 14 bit bender value of a pitch bend message
    void SetBenderValue ( short v );

    void SetMetaType ( unsigned char t ) ;

    void SetMetaValue ( unsigned short v );

    void SetNoteOn ( unsigned char chan, unsigned char note, unsigned char vel );

    void SetNoteOff ( unsigned char chan, unsigned char note, unsigned char vel );

    void SetPolyPressure ( unsigned char chan, unsigned char note, unsigned char pres );

    void SetControlChange ( unsigned char chan, unsigned char ctrl, unsigned char val );

    // set panorama control in chan: pan = -1. for lefmost, 0. for centre, +1. for rightmost
    void SetPan( unsigned char chan, double pan );

    double GetPan();

    void SetProgramChange ( unsigned char chan, unsigned char val );

    void SetChannelPressure ( unsigned char chan, unsigned char val );

    void SetPitchBend ( unsigned char chan, short val );

    void SetPitchBend ( unsigned char chan, unsigned char low, unsigned char high );

    void SetSysEx( unsigned char type ); // type = SYSEX_START or SYSEX_START_A

    void SetMTC ( unsigned char field, unsigned char v );

    void SetSongPosition ( short pos );

    void SetSongSelect ( unsigned char sng );

    void SetTuneRequest();

    void SetMetaEvent ( unsigned char type, unsigned char v1, unsigned char v2 );

    void SetMetaEvent ( unsigned char type, unsigned short v );

    void SetAllNotesOff ( unsigned char chan, unsigned char type = C_ALL_NOTES_OFF, unsigned char mode = 0);

    void SetLocal ( unsigned char chan, unsigned char v );

    void SetNoOp()
    {
        Clear();
        service_num = SERVICE_NO_OPERATION;
    }

    void SetTempo ( unsigned long tempo ); // If no tempo is define, 120 beats per minute is assumed.

    void SetTempo32 ( unsigned long tempo_times_32 );

    void SetText ( unsigned short text_num, unsigned char type = META_GENERIC_TEXT );

    void SetDataEnd();
    void SetEndOfTrack()
    {
        SetDataEnd();
    }

    void SetTimeSig (
        unsigned char numerator = 4,
        unsigned char denominator_power = 2,
        unsigned char midi_clocks_per_metronome = 24,
        unsigned char num_32nd_per_midi_quarter_note = 8 );

    void SetKeySig ( signed char sharp_flats, unsigned char major_minor );

    void SetBeatMarker();

    void SetUserAppMarker();

    friend bool operator == ( const MIDIMessage &m1, const MIDIMessage &m2 );

    //@}

protected:

    static const char * chan_msg_name[16]; ///< Simple ascii text strings describing each channel message type (0x8X to 0xeX)
    static const char * sys_msg_name[16]; ///< Simple ascii text strings describing each system message type (0xf0 to 0xff)
    static const char * service_msg_name[];

    unsigned int service_num; // if service_num != NOT_SERVICE than event used for internal service

    unsigned char status; // type of events and channel for Channel events, type of SysEx events
    unsigned char byte1; // type of Meta events, not used for SysExURT events
    unsigned char byte2; // Meta events or SysExURT events first data byte (#1)
    unsigned char byte3;
    unsigned char byte4;
    unsigned char byte5;
    unsigned char byte6; // Meta events or SysExURT events last data byte (#5)
    unsigned char data_length; // number of data bytes in Meta events or SysExURT events (0...5)
};


///
/// The MIDIBigMessage inherits from a MIDIMessage and adds the capability of storing
/// a dynamically allocated MIDISystemExclusive message inside in case the the message needs to
/// store a sysex.  If it does not need to store a sysex, typically the MIDISysexExclusive is not
/// allocated
///

class MIDIBigMessage : public MIDIMessage
{
public:

    ///@name Constructors/assignment operators/Copiers
    //@{

    MIDIBigMessage();

    MIDIBigMessage ( const MIDIBigMessage &m );

    MIDIBigMessage ( const MIDIMessage &m );

    MIDIBigMessage ( const MIDIMessage &m, const MIDISystemExclusive *e );

    const MIDIBigMessage &operator = ( const MIDIBigMessage &m );

    const MIDIBigMessage &operator = ( const MIDIMessage &m );


    void Copy ( const MIDIBigMessage &m );

    void Copy ( const MIDIMessage &m );

    void CopySysEx ( const MIDISystemExclusive *e );

    //@}


    void Clear();

    void ClearSysEx();


    ///
    /// destructor
    ///

    ~MIDIBigMessage();

    void SetNoOp()
    {
        Clear();
        MIDIMessage::SetNoOp();
    }

    MIDISystemExclusive *GetSysEx();

    const MIDISystemExclusive *GetSysEx() const;

    std::string GetSysExString() const
    {
        const unsigned char *buf = GetSysEx()->GetBuf();
        int len = GetSysEx()->GetLengthSE();
        std::string str;
        for (int i = 0; i < len; ++i)
            str.push_back( (char) buf[i] );
        return str;
    }

    friend bool operator == ( const MIDIBigMessage &m1, const MIDIBigMessage &m2 );

protected:

    MIDISystemExclusive *sysex;
};



class MIDITimedMessage : public MIDIMessage
{
public:

    //
    // Constructors
    //

    MIDITimedMessage();

    MIDITimedMessage ( const MIDITimedMessage &m );

    MIDITimedMessage ( const MIDIMessage &m );

    void Clear();

    void Copy ( const MIDITimedMessage &m );

    //
    // operator =
    //

    const MIDITimedMessage &operator = ( const MIDITimedMessage & m );

    const MIDITimedMessage &operator = ( const MIDIMessage & m );

    //
    // 'Get' methods
    //

    MIDIClockTime GetTime() const;

    //
    // 'Set' methods
    //

    void SetTime ( MIDIClockTime t );


    //
    // Compare method for sorting. Not just comparing time.
    //

    static int  CompareEvents (
        const MIDITimedMessage &a,
        const MIDITimedMessage &b
    );

    friend bool operator == ( const MIDITimedMessage &m1, const MIDITimedMessage &m2 );

protected:


    MIDIClockTime time;
};



class MIDIDeltaTimedMessage : public MIDIMessage
{
public:

    //
    // Constructors
    //

    MIDIDeltaTimedMessage();

    MIDIDeltaTimedMessage ( const MIDIDeltaTimedMessage &m );

    MIDIDeltaTimedMessage ( const MIDIMessage &m );

    void Clear();

    void Copy ( const MIDIDeltaTimedMessage &m );

    //
    // operator =
    //

    const MIDIDeltaTimedMessage &operator = ( const MIDIDeltaTimedMessage &m );

    const MIDIDeltaTimedMessage &operator = ( const MIDIMessage &m );

    //
    // 'Get' methods
    //

    MIDIClockTime GetDeltaTime() const;

    //
    // 'Set' methods
    //

    void SetDeltaTime ( MIDIClockTime t );


protected:
    MIDIClockTime dtime;
};



class MIDITimedBigMessage : public MIDIBigMessage
{
public:

    //
    // Constructors
    //

    MIDITimedBigMessage();

    MIDITimedBigMessage ( const MIDITimedBigMessage &m );

    MIDITimedBigMessage ( const MIDIBigMessage &m );

    MIDITimedBigMessage ( const MIDITimedMessage &m );

    MIDITimedBigMessage ( const MIDIMessage &m );

    MIDITimedBigMessage ( const MIDITimedMessage &m, const MIDISystemExclusive *e );

    void Clear();

    void Copy ( const MIDITimedBigMessage &m );

    void Copy ( const MIDITimedMessage &m );

    //
    // operator =
    //

    const MIDITimedBigMessage &operator = ( const MIDITimedBigMessage & m );

    const MIDITimedBigMessage &operator = ( const MIDITimedMessage & m );

    const MIDITimedBigMessage &operator = ( const MIDIMessage & m );

    //
    // 'Get' methods
    //

    MIDIClockTime GetTime() const;

    //
    // 'Set' methods
    //

    void SetTime ( MIDIClockTime t );

    //
    // Compare method, for sorting. Not just comparing time.
    //

    static int CompareEvents (
        const MIDITimedBigMessage &a,
        const MIDITimedBigMessage &b
    );

    friend bool operator == ( const MIDITimedBigMessage &m1, const MIDITimedBigMessage &m2 );

protected:
    MIDIClockTime time;
};



class MIDIDeltaTimedBigMessage : public MIDIBigMessage
{
public:
    //
    // Constructors
    //

    MIDIDeltaTimedBigMessage();

    MIDIDeltaTimedBigMessage ( const MIDIDeltaTimedBigMessage &m );

    MIDIDeltaTimedBigMessage ( const MIDIBigMessage &m );

    MIDIDeltaTimedBigMessage ( const MIDIMessage &m );

    MIDIDeltaTimedBigMessage ( const MIDIDeltaTimedMessage &m );

    void Clear();

    void Copy ( const MIDIDeltaTimedBigMessage &m );

    void Copy ( const MIDIDeltaTimedMessage &m );

    //
    // operator =
    //

    const MIDIDeltaTimedBigMessage &operator = ( const MIDIDeltaTimedBigMessage &m );

    const MIDIDeltaTimedBigMessage &operator = ( const MIDIDeltaTimedMessage &m );

    const MIDIDeltaTimedBigMessage &operator = ( const MIDIMessage &m );

    //
    // 'Get' methods
    //

    MIDIClockTime GetDeltaTime() const;

    //
    // 'Set' methods
    //

    void SetDeltaTime ( MIDIClockTime t );


protected:
    MIDIClockTime dtime;
};

}


#endif

