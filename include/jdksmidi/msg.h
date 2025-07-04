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
/// MIDI Message that can fit within 3 bytes plus _status byte.  It can also hold some
/// non-MIDI messages, known as Meta messages like No-op, Key signature, Time Signature, etc,
/// which are useful for internal processing.
///
/// This class is the base class
/// for a number of MIDIMessage variants, such as the MIDIBigMessage, MIDITimedMessage,
/// MIDITimedBigMessage, and MIDIDeltaTimedBigMessage.  This could be a good candidate
/// for using a mix-in architecture ( see http://en.wikipedia.org/wiki/Mixin ) via
/// multiple inheritance, but at the _time this was written in 1990, C++ compilers typically
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

    void clear();  ///< Set the MIDIMessage object to 0,0,0,0.

    void copy(MIDIMessage const& m);  ///< Copy the value of the specified MIDIMessage.

    //@}

    char const* msg_to_text(
        char* txt) const;  ///< Create a human readable ascii string describing the message.  This
                           ///< is potentially unsafe as the 'txt' param must point to a buffer of
                           ///< at least 64 chars long.

    ///@name The Query methods.
    //@{

    char get_length() const;  ///< Get the length in bytes of the entire message.

    /// Get the _status byte of the message.
    std::uint8_t get_status() const { return (std::uint8_t)_status; }

    /// If the message is a channel message, this method returns the MIDI channel that the message
    /// is on.
    std::uint8_t get_channel() const { return (std::uint8_t)(_status & 0x0f); }

    /// If the message is a channel message, this method returns the relevant top 4 bits which
    /// describe what type of channel message it is.
    std::uint8_t get_type() const { return (std::uint8_t)(_status & 0xf0); }

    /// If the message is some sort of meta-message, then GetMetaType returns the type byte.
    std::uint8_t get_meta_type() const { return _byte1; }

    /// Access to the raw _byte1 of the message
    std::uint8_t get_byte1() const { return _byte1; }

    /// Access to the raw _byte2 of the message
    std::uint8_t get_byte2() const { return _byte2; }

    /// Access to the raw _byte3 of the message
    std::uint8_t get_byte3() const { return _byte3; }

    /// If the message is a note on, note off, or poly aftertouch message, get_note() returns the
    /// note number
    std::uint8_t get_note() const { return _byte1; }

    /// If the message is a note on, note off, or poly aftertouch message, get_velocity() returns the
    /// velocity or pressure
    std::uint8_t get_velocity() const { return _byte2; }

    /// If the message is a channel pressure message, get_channel_pressure() returns the pressure
    /// value.
    std::uint8_t get_channel_pressure() const { return _byte1; }

    /// If the message is a 7 bit program change value, get_pg_value() returns the program number.
    std::uint8_t get_pg_value() const { return _byte1; }

    /// If the message is a control change message, get_controller() returns the controller number.
    std::uint8_t get_controller() const { return _byte1; }

    /// If the message is a control change message, get_controller_value() returns the 7 bit
    /// controller value.
    std::uint8_t get_controller_value() const { return _byte2; }

    /// If the message is a bender message, get_bender_value() returns the signed 14 bit bender value.
    short get_bender_value() const;

    /// If the message is a meta-message, get_meta_value() returns the unsigned 14 bit value attached.
    unsigned short get_meta_value() const;

    /// If the message is a _time signature meta-message, get_time_sig_numerator() returns the numerator
    /// of the _time signature.
    std::uint8_t get_time_sig_numerator() const;

    /// If the message is a _time signature meta-message, get_time_sig_denominator() returns the
    /// denominator of the _time signature.
    std::uint8_t get_time_sig_denominator() const;

    /// If the message is a key signature meta-message, get_key_sig_sharp_flats() returns to standard
    /// midi file form of the key. Negative values means that many flats, positive numbers means
    /// that many sharps.
    signed char get_key_sig_sharp_flats() const;

    /// If the message is a key signature meta-message, get_key_sig_major_minor() returns to standard
    /// midi file form of the key major/minor flag. 0 means a major key, 1 means a minor key.
    std::uint8_t get_key_sig_major_minor() const;

    /// If the message is some sort of real _time channel message, is_channel_msg() will return true.
    /// You can then call get_channel() for more information.
    bool is_channel_msg() const;

    /// If the message is a note on message (but not a note on message with velocity>0), is_note_on()
    /// will return true. You can then call get_channel(), get_note() and get_velocity() for further
    /// information.
    bool is_note_on() const;

    /// If the message is a note off message or a note on message with velocity == 0, is_note_off()
    /// will return true. You can then call get_channel(), get_note() and get_velocity() for further
    /// information.
    bool is_note_off() const;

    /// If the message is a polyphonic pressure chanel message, is_poly_pressure() will return true.
    /// You can then call get_channel(), get_note() and get_velocity() for further informtion.
    bool is_poly_pressure() const;

    /// If the message is a control change message, is_control_change() will return true. You can then
    /// call get_channel(), get_controller() and get_controller_value() for further information.
    bool is_control_change() const;

    /// If the message is a program change message, is_program_change() will return true.  You can
    /// then call get_channel() and get_pg_value() for further information.
    bool is_program_change() const;

    /// If the message is a channel pressure change message, is_channel_pressure() will return true.
    /// You can then call get_channel() and get_channel_pressure() for further information.
    bool is_channel_pressure() const;

    /// If the message is a bender message, is_pitch_bend() will return true. You can then call
    /// get_channel() and get_bender_value() for further information
    bool is_pitch_bend() const;

    /// If the message is a system message (the _status byte is 0xf0 or higher), is_system_message()
    /// will return true.
    bool is_system_message() const;

    /// If the message is a system exclusive marker, is_sys_ex() will return true. You can then call
    /// get_sys_ex_num() to extract a sysex id code which must be managed separately.
    /// \note Sysex messages are not stored in the MIDIMessage object. \see MIDIBigMessage
    bool is_sys_ex() const;

    short get_sys_ex_num() const;

    bool is_mtc() const;

    bool is_song_position() const;

    bool is_song_select() const;

    bool is_tune_request() const;

    bool is_meta_event() const;

    bool is_text_event() const;

    bool is_all_notes_off() const;

    bool is_no_op() const;

    bool is_tempo() const;

    bool is_data_end() const;

    bool is_time_sig() const;

    bool is_key_sig() const;

    bool is_beat_marker() const;

    ///
    /// GetTempo() returns the tempo value in 1/32 bpm
    ///
    unsigned short get_tempo32() const;

    unsigned short get_loop_number() const;

    //@}

    ///@name The 'Set' methods
    //@{

    /// Set all bits of the _status byte
    void set_status(std::uint8_t s) { _status = s; }

    /// set just the lower 4 bits of the _status byte without changing the upper 4 bits
    void set_channel(std::uint8_t s) { _status = (std::uint8_t)((_status & 0xf0) | s); }

    /// set just the upper 4 bits of the _status byte without changing the lower 4 bits
    void set_type(std::uint8_t s) { _status = (std::uint8_t)((_status & 0x0f) | s); }

    /// Set the value of the data byte 1
    void set_byte1(std::uint8_t b) { _byte1 = b; }

    /// Set the value of the data byte 2
    void set_byte2(std::uint8_t b) { _byte2 = b; }

    /// Set the value of the data byte 3
    void set_byte3(std::uint8_t b) { _byte3 = b; }

    /// Set the note number for note on, note off, and polyphonic aftertouch messages
    void set_note(std::uint8_t n) { _byte1 = n; }

    /// Set the velocity of a note on or note off message
    void set_velocity(std::uint8_t v) { _byte2 = v; }

    /// Set the program number of a program change message
    void set_pg_value(std::uint8_t v) { _byte1 = v; }

    /// Set the controller number of a control change message
    void set_controller(std::uint8_t c) { _byte1 = c; }

    /// Set the 7 bit controller value of a control change message
    void set_controller_value(std::uint8_t v) { _byte2 = v; }

    /// Set the signed 14 bit bender value of a pitch bend message
    void set_bender_value(short v);

    void set_meta_type(std::uint8_t t);

    void set_meta_value(unsigned short v);

    void set_note_on(std::uint8_t chan, std::uint8_t note, std::uint8_t vel);

    void set_note_off(std::uint8_t chan, std::uint8_t note, std::uint8_t vel);

    void set_poly_pressure(std::uint8_t chan, std::uint8_t note, std::uint8_t pres);

    void set_control_change(std::uint8_t chan, std::uint8_t ctrl, std::uint8_t val);

    void set_program_change(std::uint8_t chan, std::uint8_t val);

    void set_channel_pressure(std::uint8_t chan, std::uint8_t val);

    void set_pitch_bend(std::uint8_t chan, short val);

    void set_pitch_bend(std::uint8_t chan, std::uint8_t low, std::uint8_t high);

    void set_sys_ex();

    void set_mtc(std::uint8_t field, std::uint8_t v);

    void set_song_position(short pos);

    void set_song_select(std::uint8_t sng);

    void set_tune_request();

    void set_meta_event(std::uint8_t type, std::uint8_t v1, std::uint8_t v2);

    void set_meta_event(std::uint8_t type, unsigned short v);

    void set_all_notes_off(std::uint8_t chan, std::uint8_t type = C_ALL_NOTES_OFF);

    void set_local(std::uint8_t chan, std::uint8_t v);

    void set_no_op();

    void set_tempo32(unsigned short tempo_times_32);

    void set_text(unsigned short text_num, std::uint8_t type = META_GENERIC_TEXT);

    void set_data_end();

    void set_time_sig(std::uint8_t numerator, std::uint8_t denominator);

    void set_key_sig(signed char sharp_flats, std::uint8_t major_minor);

    void set_beat_marker();

    //@}

  protected:
    static char const* chan_msg_name[16];  ///< Simple ascii text strings describing each channel
                                           ///< message type (0x8X to 0xeX)
    static char const* sys_msg_name[16];   ///< Simple ascii text strings describing each system
                                           ///< message type (0xf0 to 0xff)

    std::uint8_t _status;
    std::uint8_t _byte1;
    std::uint8_t _byte2;
    std::uint8_t _byte3;  ///< byte 3 is only used for meta-events and to round out the structure
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

    void copy(MIDIBigMessage const& m);

    void copy(MIDIMessage const& m);

    void copy_sys_ex(MIDISystemExclusive const* e);

    //@}

    void clear();

    void clear_sys_ex();

    ///
    /// destructor
    ///

    ~MIDIBigMessage();

    MIDISystemExclusive* get_sys_ex();

    MIDISystemExclusive const* get_sys_ex() const;

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

    void clear();

    void copy(MIDITimedMessage const& m);

    //
    // operator =
    //

    MIDITimedMessage const& operator=(MIDITimedMessage const& m);

    MIDITimedMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime get_time() const;

    //
    // 'Set' methods
    //

    void set_time(MIDIClockTime t);

    //
    // Compare method for sorting. Not just comparing _time.
    //

    static int compare_events(MIDITimedMessage const& a, MIDITimedMessage const& b);

  protected:
    MIDIClockTime _time;
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

    void clear();

    void copy(MIDIDeltaTimedMessage const& m);

    //
    // operator =
    //

    MIDIDeltaTimedMessage const& operator=(MIDIDeltaTimedMessage const& m);

    MIDIDeltaTimedMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime get_delta_time() const;

    //
    // 'Set' methods
    //

    void set_delta_time(MIDIClockTime t);

  protected:
    MIDIClockTime _d_time;
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

    void clear();

    void copy(MIDITimedBigMessage const& m);

    void copy(MIDITimedMessage const& m);

    //
    // operator =
    //

    MIDITimedBigMessage const& operator=(MIDITimedBigMessage const& m);

    MIDITimedBigMessage const& operator=(MIDITimedMessage const& m);

    MIDITimedBigMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime get_time() const;

    //
    // 'Set' methods
    //

    void set_time(MIDIClockTime t);

    //
    // Compare method, for sorting. Not just comparing _time.
    //

    static int compare_events(MIDITimedBigMessage const& a, MIDITimedBigMessage const& b);

  protected:
    MIDIClockTime _time;
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

    void clear();

    void copy(MIDIDeltaTimedBigMessage const& m);

    void copy(MIDIDeltaTimedMessage const& m);

    //
    // operator =
    //

    MIDIDeltaTimedBigMessage const& operator=(MIDIDeltaTimedBigMessage const& m);

    MIDIDeltaTimedBigMessage const& operator=(MIDIDeltaTimedMessage const& m);

    MIDIDeltaTimedBigMessage const& operator=(MIDIMessage const& m);

    //
    // 'Get' methods
    //

    MIDIClockTime get_delta_time() const;

    //
    // 'Set' methods
    //

    void set_delta_time(MIDIClockTime t);

  protected:
    MIDIClockTime _d_time;
};

}  // namespace jdksmidi

#endif
