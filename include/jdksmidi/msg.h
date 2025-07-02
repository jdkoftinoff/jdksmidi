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

#ifndef JDKSMIDI_MSG_H
#define JDKSMIDI_MSG_H

#include "jdksmidi/midi.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/tempo.h"

#include <cstdint>

namespace jdksmidi {

class MIDIMessage;
class MIDIBigMessage;

class MIDITimedMessage;
class MIDIDeltaTimedMessage;

class MIDITimedBigMessage;
class MIDIDeltaTimedBigMessage;

///
/// The MIDIMessage class is a simple, lightweight container which can hold a single
/// MIDI Message that can fit within 3 bytes plus status byte.  It can also hold some
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

    MIDIMessage();  ///< Create a MIDIMessage object which holds no values.

    MIDIMessage(MIDIMessage const& m);  ///< Copy Constructor.

    MIDIMessage const& operator=(
        MIDIMessage const& m);  ///< The assignment operator. Copies the MIDIMessage value.

    void Clear();  ///< Set the MIDIMessage object to 0,0,0,0.

    void Copy(MIDIMessage const& m);  ///< Copy the value of the specified MIDIMessage.

    //@}

    char const* MsgToText(
        char* txt) const;  ///< Create a human readable ascii string describing the message.  This
                           ///< is potentially unsafe as the 'txt' param must point to a buffer of
                           ///< at least 64 chars long.

    ///@name The Query methods.
    //@{

    char GetLength() const;  ///< Get the length in bytes of the entire message.

    /// Get the status byte of the message.
    std::uint8_t GetStatus() const { return (std::uint8_t)status; }

    /// If the message is a channel message, this method returns the MIDI channel that the message
    /// is on.
    std::uint8_t GetChannel() const { return (std::uint8_t)(status & 0x0f); }

    /// If the message is a channel message, this method returns the relevant top 4 bits which
    /// describe what type of channel message it is.
    std::uint8_t GetType() const { return (std::uint8_t)(status & 0xf0); }

    /// If the message is some sort of meta-message, then GetMetaType returns the type byte.
    std::uint8_t GetMetaType() const { return byte1; }

    /// Access to the raw byte1 of the message
    std::uint8_t GetByte1() const { return byte1; }

    /// Access to the raw byte2 of the message
    std::uint8_t GetByte2() const { return byte2; }

    /// Access to the raw byte3 of the message
    std::uint8_t GetByte3() const { return byte3; }

    /// If the message is a note on, note off, or poly aftertouch message, GetNote() returns the
    /// note number
    std::uint8_t GetNote() const { return byte1; }

    /// If the message is a note on, note off, or poly aftertouch message, GetVelocity() returns the
    /// velocity or pressure
    std::uint8_t GetVelocity() const { return byte2; }

    /// If the message is a channel pressure message, GetChannelPressure() returns the pressure
    /// value.
    std::uint8_t GetChannelPressure() const { return byte1; }

    /// If the message is a 7 bit program change value, GetPGValue() returns the program number.
    std::uint8_t GetPGValue() const { return byte1; }

    /// If the message is a control change message, GetController() returns the controller number.
    std::uint8_t GetController() const { return byte1; }

    /// If the message is a control change message, GetControllerValue() returns the 7 bit
    /// controller value.
    std::uint8_t GetControllerValue() const { return byte2; }

    /// If the message is a bender message, GetBenderValue() returns the signed 14 bit bender value.
    short GetBenderValue() const;

    /// If the message is a meta-message, GetMetaValue() returns the unsigned 14 bit value attached.
    unsigned short GetMetaValue() const;

    /// If the message is a time signature meta-message, GetTimeSigNumerator() returns the numerator
    /// of the time signature.
    std::uint8_t GetTimeSigNumerator() const;

    /// If the message is a time signature meta-message, GetTimeSigDenominator() returns the
    /// denominator of the time signature.
    std::uint8_t GetTimeSigDenominator() const;

    /// If the message is a key signature meta-message, GetKeySigSharpFlats() returns to standard
    /// midi file form of the key. Negative values means that many flats, positive numbers means
    /// that many sharps.
    signed char GetKeySigSharpFlats() const;

    /// If the message is a key signature meta-message, GetKeySigMajorMinor() returns to standard
    /// midi file form of the key major/minor flag. 0 means a major key, 1 means a minor key.
    std::uint8_t GetKeySigMajorMinor() const;

    /// If the message is some sort of real time channel message, IsChannelMsg() will return true.
    /// You can then call GetChannel() for more information.
    bool IsChannelMsg() const;

    /// If the message is a note on message (but not a note on message with velocity>0), IsNoteOn()
    /// will return true. You can then call GetChannel(), GetNote() and GetVelocity() for further
    /// information.
    bool IsNoteOn() const;

    /// If the message is a note off message or a note on message with velocity == 0, IsNoteOff()
    /// will return true. You can then call GetChannel(), GetNote() and GetVelocity() for further
    /// information.
    bool IsNoteOff() const;

    /// If the message is a polyphonic pressure chanel message, IsPolyPressure() will return true.
    /// You can then call GetChannel(), GetNote() and GetVelocity() for further informtion.
    bool IsPolyPressure() const;

    /// If the message is a control change message, IsControlChange() will return true. You can then
    /// call GetChannel(), GetController() and GetControllerValue() for further information.
    bool IsControlChange() const;

    /// If the message is a program change message, IsProgramChange() will return true.  You can
    /// then call GetChannel() and GetPGValue() for further information.
    bool IsProgramChange() const;

    /// If the message is a channel pressure change message, IsChannelPressure() will return true.
    /// You can then call GetChannel() and GetChannelPressure() for further information.
    bool IsChannelPressure() const;

    /// If the message is a bender message, IsPitchBend() will return true. You can then call
    /// GetChannel() and GetBenderValue() for further information
    bool IsPitchBend() const;

    /// If the message is a system message (the status byte is 0xf0 or higher), IsSystemMessage()
    /// will return true.
    bool IsSystemMessage() const;

    /// If the message is a system exclusive marker, IsSysEx() will return true. You can then call
    /// GetSysExNum() to extract a sysex id code which must be managed separately.
    /// \note Sysex messages are not stored in the MIDIMessage object. \see MIDIBigMessage
    bool IsSysEx() const;

    short GetSysExNum() const;

    bool IsMTC() const;

    bool IsSongPosition() const;

    bool IsSongSelect() const;

    bool IsTuneRequest() const;

    bool IsMetaEvent() const;

    bool IsTextEvent() const;

    bool IsAllNotesOff() const;

    bool IsNoOp() const;

    bool IsTempo() const;

    bool IsDataEnd() const;

    bool IsTimeSig() const;

    bool IsKeySig() const;

    bool IsBeatMarker() const;

    ///
    /// GetTempo() returns the tempo value in 1/32 bpm
    ///
    unsigned short GetTempo32() const;

    unsigned short GetLoopNumber() const;

    //@}

    ///@name The 'Set' methods
    //@{

    /// Set all bits of the status byte
    void SetStatus(std::uint8_t s) { status = s; }

    /// set just the lower 4 bits of the status byte without changing the upper 4 bits
    void SetChannel(std::uint8_t s) { status = (std::uint8_t)((status & 0xf0) | s); }

    /// set just the upper 4 bits of the status byte without changing the lower 4 bits
    void SetType(std::uint8_t s) { status = (std::uint8_t)((status & 0x0f) | s); }

    /// Set the value of the data byte 1
    void SetByte1(std::uint8_t b) { byte1 = b; }

    /// Set the value of the data byte 2
    void SetByte2(std::uint8_t b) { byte2 = b; }

    /// Set the value of the data byte 3
    void SetByte3(std::uint8_t b) { byte3 = b; }

    /// Set the note number for note on, note off, and polyphonic aftertouch messages
    void SetNote(std::uint8_t n) { byte1 = n; }

    /// Set the velocity of a note on or note off message
    void SetVelocity(std::uint8_t v) { byte2 = v; }

    /// Set the program number of a program change message
    void SetPGValue(std::uint8_t v) { byte1 = v; }

    /// Set the controller number of a control change message
    void SetController(std::uint8_t c) { byte1 = c; }

    /// Set the 7 bit controller value of a control change message
    void SetControllerValue(std::uint8_t v) { byte2 = v; }

    /// Set the signed 14 bit bender value of a pitch bend message
    void SetBenderValue(short v);

    void SetMetaType(std::uint8_t t);

    void SetMetaValue(unsigned short v);

    void SetNoteOn(std::uint8_t chan, std::uint8_t note, std::uint8_t vel);

    void SetNoteOff(std::uint8_t chan, std::uint8_t note, std::uint8_t vel);

    void SetPolyPressure(std::uint8_t chan, std::uint8_t note, std::uint8_t pres);

    void SetControlChange(std::uint8_t chan, std::uint8_t ctrl, std::uint8_t val);

    void SetProgramChange(std::uint8_t chan, std::uint8_t val);

    void SetChannelPressure(std::uint8_t chan, std::uint8_t val);

    void SetPitchBend(std::uint8_t chan, short val);

    void SetPitchBend(std::uint8_t chan, std::uint8_t low, std::uint8_t high);

    void SetSysEx();

    void SetMTC(std::uint8_t field, std::uint8_t v);

    void SetSongPosition(short pos);

    void SetSongSelect(std::uint8_t sng);

    void SetTuneRequest();

    void SetMetaEvent(std::uint8_t type, std::uint8_t v1, std::uint8_t v2);

    void SetMetaEvent(std::uint8_t type, unsigned short v);

    void SetAllNotesOff(std::uint8_t chan, std::uint8_t type = C_ALL_NOTES_OFF);

    void SetLocal(std::uint8_t chan, std::uint8_t v);

    void SetNoOp();

    void SetTempo32(unsigned short tempo_times_32);

    void SetText(unsigned short text_num, std::uint8_t type = META_GENERIC_TEXT);

    void SetDataEnd();

    void SetTimeSig(std::uint8_t numerator, std::uint8_t denominator);

    void SetKeySig(signed char sharp_flats, std::uint8_t major_minor);

    void SetBeatMarker();

    //@}

  protected:
    static char const* chan_msg_name[16];  ///< Simple ascii text strings describing each channel
                                           ///< message type (0x8X to 0xeX)
    static char const* sys_msg_name[16];   ///< Simple ascii text strings describing each system
                                           ///< message type (0xf0 to 0xff)

    std::uint8_t status;
    std::uint8_t byte1;
    std::uint8_t byte2;
    std::uint8_t byte3;  ///< byte 3 is only used for meta-events and to round out the structure
                         ///< size to 32 bits
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

    MIDIBigMessage(MIDIBigMessage const& m);

    MIDIBigMessage(MIDIMessage const& m);

    MIDIBigMessage const& operator=(MIDIBigMessage const& m);

    MIDIBigMessage const& operator=(MIDIMessage const& m);

    void Copy(MIDIBigMessage const& m);

    void Copy(MIDIMessage const& m);

    void CopySysEx(MIDISystemExclusive const* e);

    //@}

    void Clear();

    void ClearSysEx();

    ///
    /// destructor
    ///

    ~MIDIBigMessage();

    MIDISystemExclusive* GetSysEx();

    MIDISystemExclusive const* GetSysEx() const;

    MIDISystemExclusive* sysex;
};

class MIDITimedMessage : public MIDIMessage
{
  public:
    //
    // Constructors
    //

    MIDITimedMessage();

    MIDITimedMessage(MIDITimedMessage const& m);

    MIDITimedMessage(MIDIMessage const& m);

    void Clear();

    void Copy(MIDITimedMessage const& m);

    //
    // operator =
    //

    MIDITimedMessage const& operator=(MIDITimedMessage const& m);

    MIDITimedMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime GetTime() const;

    //
    // 'Set' methods
    //

    void SetTime(MIDIClockTime t);

    //
    // Compare method for sorting. Not just comparing time.
    //

    static int CompareEvents(MIDITimedMessage const& a, MIDITimedMessage const& b);

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

    MIDIDeltaTimedMessage(MIDIDeltaTimedMessage const& m);

    MIDIDeltaTimedMessage(MIDIMessage const& m);

    void Clear();

    void Copy(MIDIDeltaTimedMessage const& m);

    //
    // operator =
    //

    MIDIDeltaTimedMessage const& operator=(MIDIDeltaTimedMessage const& m);

    MIDIDeltaTimedMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime GetDeltaTime() const;

    //
    // 'Set' methods
    //

    void SetDeltaTime(MIDIClockTime t);

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

    MIDITimedBigMessage(MIDITimedBigMessage const& m);

    MIDITimedBigMessage(MIDIBigMessage const& m);

    MIDITimedBigMessage(MIDITimedMessage const& m);

    MIDITimedBigMessage(MIDIMessage const& m);

    void Clear();

    void Copy(MIDITimedBigMessage const& m);

    void Copy(MIDITimedMessage const& m);

    //
    // operator =
    //

    MIDITimedBigMessage const& operator=(MIDITimedBigMessage const& m);

    MIDITimedBigMessage const& operator=(MIDITimedMessage const& m);

    MIDITimedBigMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime GetTime() const;

    //
    // 'Set' methods
    //

    void SetTime(MIDIClockTime t);

    //
    // Compare method, for sorting. Not just comparing time.
    //

    static int CompareEvents(MIDITimedBigMessage const& a, MIDITimedBigMessage const& b);

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

    MIDIDeltaTimedBigMessage(MIDIDeltaTimedBigMessage const& m);

    MIDIDeltaTimedBigMessage(MIDIBigMessage const& m);

    MIDIDeltaTimedBigMessage(MIDIMessage const& m);

    MIDIDeltaTimedBigMessage(MIDIDeltaTimedMessage const& m);

    void Clear();

    void Copy(MIDIDeltaTimedBigMessage const& m);

    void Copy(MIDIDeltaTimedMessage const& m);

    //
    // operator =
    //

    MIDIDeltaTimedBigMessage const& operator=(MIDIDeltaTimedBigMessage const& m);

    MIDIDeltaTimedBigMessage const& operator=(MIDIDeltaTimedMessage const& m);

    MIDIDeltaTimedBigMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime GetDeltaTime() const;

    //
    // 'Set' methods
    //

    void SetDeltaTime(MIDIClockTime t);

  protected:
    MIDIClockTime dtime;
};

}  // namespace jdksmidi

#endif
