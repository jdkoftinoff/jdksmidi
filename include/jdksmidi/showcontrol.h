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
#ifndef JDKSMIDI_SHOWCONTROL_H
#define JDKSMIDI_SHOWCONTROL_H

#include "jdksmidi/sysex.h"

#include <cstdint>

namespace jdksmidi {
//
// Hacked defines that really should just go away:
//

// Simple macros to make Setter and Getter methods easier
// in an object.
//
// examples:
//
// _ATTRIBUTE(long,Position)
//
// would generate a private 'long Position;'
// a 'const long GetPosition() const;' method
// and a 'void SetPosition( long a );' method
//
//
//
// _ATTRIBUTE_REF( long, Position )
//
// would generate a private 'long Position;'
// a 'const long &GetPosition() const;' method
// and a 'void SetPosition( const long &a );' method
//
//
//
// _ACCESS( double, DPosition, Position )
//
// Is used to access an attribute with a different type.
// It would generate just the methods:
// 'double GetDPosition() const { return Position; }
// 'void SetDPosition( double a ) { Position=a; }
//
//
// _ACCESS_REF( double, DPosition, Position )
//
// Is used to access an attribute with a different type.
// It would generate just the methods:
// 'const double &GetDPosition() const { return Position; }
// 'void SetDPosition( const double &a ) { Position=a; }
//

#define _ATTRIBUTE(TYPE, NAME)                                                                     \
  public:                                                                                          \
    const TYPE Get##NAME() const                                                                   \
    {                                                                                              \
        return NAME;                                                                               \
    }                                                                                              \
    void Set##NAME(TYPE a)                                                                         \
    {                                                                                              \
        NAME = a;                                                                                  \
    }                                                                                              \
                                                                                                   \
  private:                                                                                         \
    TYPE NAME

#define _ATTRIBUTE_REF(TYPE, NAME)                                                                 \
  public:                                                                                          \
    const TYPE& Get##NAME() const                                                                  \
    {                                                                                              \
        return NAME;                                                                               \
    }                                                                                              \
    void Set##NAME(const TYPE& a)                                                                  \
    {                                                                                              \
        NAME = a;                                                                                  \
    }                                                                                              \
                                                                                                   \
  private:                                                                                         \
    TYPE NAME

#define _ACCESS(TYPE, NAME1, NAME2)                                                                \
  public:                                                                                          \
    const TYPE Get##NAME1() const                                                                  \
    {                                                                                              \
        return NAME2;                                                                              \
    }                                                                                              \
    void Set##NAME1(TYPE a)                                                                        \
    {                                                                                              \
        NAME2 = a;                                                                                 \
    }

#define _ACCESS_REF(TYPE, NAME1, NAME2)                                                            \
  public:                                                                                          \
    const TYPE& Get##NAME1() const                                                                 \
    {                                                                                              \
        return NAME2;                                                                              \
    }                                                                                              \
    void Set##NAME1(const TYPE& a)                                                                 \
    {                                                                                              \
        NAME2 = a;                                                                                 \
    }

#define _PATTRIBUTE(TYPE, NAME)                                                                    \
  protected:                                                                                       \
    const TYPE* Get##NAME() const                                                                  \
    {                                                                                              \
        return NAME;                                                                               \
    }                                                                                              \
    void Set##NAME(TYPE* a)                                                                        \
    {                                                                                              \
        NAME = a;                                                                                  \
    }                                                                                              \
                                                                                                   \
  private:                                                                                         \
    TYPE NAME

#define _PATTRIBUTE_REF(TYPE, NAME)                                                                \
  protected:                                                                                       \
    const TYPE& Get##NAME() const                                                                  \
    {                                                                                              \
        return NAME;                                                                               \
    }                                                                                              \
    void Set##NAME(const TYPE& a)                                                                  \
    {                                                                                              \
        NAME = a;                                                                                  \
    }                                                                                              \
                                                                                                   \
  private:                                                                                         \
    TYPE NAME

#define _PACCESS(TYPE, NAME1, NAME2)                                                               \
  protected:                                                                                       \
    const TYPE Get##NAME1() const                                                                  \
    {                                                                                              \
        return NAME2;                                                                              \
    }                                                                                              \
    void Set##NAME1(const TYPE a)                                                                  \
    {                                                                                              \
        NAME2 = a;                                                                                 \
    }

#define _PACCESS_REF(TYPE, NAME1, NAME2)                                                           \
  protected:                                                                                       \
    const TYPE& Get##NAME1() const                                                                 \
    {                                                                                              \
        return NAME2;                                                                              \
    }                                                                                              \
    void Set##NAME1(const TYPE& a)                                                                 \
    {                                                                                              \
        NAME2 = a;                                                                                 \
    }

class MIDICue
{
  public:
    MIDICue(MIDICue const& c)
        : v1(c.v1)
        , v2(c.v2)
        , v3(c.v3)
        , num_values(c.num_values)
    {}
    MIDICue()
        : v1(0)
        , num_values(1)
    {}
    MIDICue(std::uint32_t v1_)
        : v1(v1_)
        , num_values(1)
    {}
    MIDICue(std::uint32_t v1_, std::uint32_t v2_)
        : v1(v1_)
        , v2(v2_)
        , num_values(2)
    {}
    MIDICue(std::uint32_t v1_, std::uint32_t v2_, std::uint32_t v3_)
        : v1(v1_)
        , v2(v2_)
        , v3(v3_)
        , num_values(3)
    {}

    void Clear()
    {
        v1 = 0;
        num_values = 1;
    }
    operator std::uint32_t() const { return v1; }

    MIDICue const& operator=(MIDICue const& c)
    {
        v1 = c.v1;
        v2 = c.v2;
        v3 = c.v3;
        num_values = c.num_values;
        return *this;
    }

    MIDICue const& operator=(std::uint32_t v)
    {
        v1 = v;
        num_values = 1;
        return *this;
    }

    bool const operator==(std::uint32_t v) { return v1 == v; }

    MIDICue const& operator==(MIDICue const& c);
    bool const operator!=(std::uint32_t v) { return v1 != v; }
    MIDICue const& operator!=(MIDICue const& c);

    bool operator<=(std::uint32_t v) { return v1 <= v; }

    bool operator>=(std::uint32_t v) { return v1 >= v; }

    bool operator<(std::uint32_t v) { return v1 < v; }

    bool operator>(std::uint32_t v) { return v1 > v; }

    friend MIDICue operator-(MIDICue const& c, MIDICue const& d);
    friend MIDICue operator-(MIDICue const& c, std::uint32_t v);
    friend MIDICue operator-(std::uint32_t v, MIDICue const& c);

    friend MIDICue operator+(MIDICue const& c, MIDICue const& d);
    friend MIDICue operator+(MIDICue const& c, std::uint32_t v);
    friend MIDICue operator+(std::uint32_t v, MIDICue const& c);

    std::uint32_t GetNumValues() const { return num_values; }
    std::uint32_t GetV1() const { return v1; }
    std::uint32_t GetV2() const { return v2; }
    std::uint32_t GetV3() const { return v3; }

    void SetNumValues(int a) { num_values = a; }
    void SetV1(std::uint32_t a) { v1 = a; }
    void SetV2(std::uint32_t a) { v2 = a; }
    void SetV3(std::uint32_t a) { v3 = a; }

  protected:
    std::uint32_t v1, v2, v3;
    int num_values;
};

inline MIDICue operator-(MIDICue const& c, MIDICue const& d)
{
    MIDICue result = c;
    result.v1 -= d.v1;
    result.v2 -= d.v2;
    result.v3 -= d.v3;
    return result;
}

inline MIDICue operator-(MIDICue const& c, std::uint32_t v)
{
    MIDICue result(c);
    result.v1 -= v;
    return result;
}

inline MIDICue operator-(std::uint32_t v, MIDICue const& c)
{
    MIDICue result(v - c.v1);
    return result;
}

inline MIDICue operator+(MIDICue const& c, MIDICue const& d)
{
    MIDICue result = c;
    result.v1 += d.v1;
    result.v2 += d.v2;
    result.v3 += d.v3;
    return result;
}

inline MIDICue operator+(MIDICue const& c, std::uint32_t v)
{
    MIDICue result(c);
    result.v1 += v;
    return result;
}

inline MIDICue operator+(std::uint32_t v, MIDICue const& c)
{
    MIDICue result(v + c.v1);
    return result;
}

enum MIDIShowCommand
{
    MIDI_SC_GO = 0x01,
    MIDI_SC_STOP = 0x02,
    MIDI_SC_RESUME = 0x03,
    MIDI_SC_TIMED_GO = 0x04,
    MIDI_SC_LOAD = 0x05,
    MIDI_SC_SET = 0x06,
    MIDI_SC_FIRE = 0x07,
    MIDI_SC_ALL_OFF = 0x08,
    MIDI_SC_RESTORE = 0x09,
    MIDI_SC_RESET = 0x0a,
    MIDI_SC_GO_OFF = 0x0b,
    MIDI_SC_GO_JAM = 0x10,
    MIDI_SC_STANDBY_PLUS = 0x11,
    MIDI_SC_STANDBY_MINUS = 0x12,
    MIDI_SC_SEQUENCE_PLUS = 0x13,
    MIDI_SC_SEQUENCE_MINUS = 0x14,
    MIDI_SC_START_CLOCK = 0x15,
    MIDI_SC_STOP_CLOCK = 0x16,
    MIDI_SC_ZERO_CLOCK = 0x17,
    MIDI_SC_SET_CLOCK = 0x18,
    MIDI_SC_MTC_CHASE_ON = 0x19,
    MIDI_SC_MTC_CHASE_OFF = 0x1a,
    MIDI_SC_OPEN_Q_LIST = 0x1b,
    MIDI_SC_CLOSE_Q_LIST = 0x1c,
    MIDI_SC_OPEN_Q_PATH = 0x1d,
    MIDI_SC_CLOSE_Q_PATH = 0x1e
};

class MIDIShowControlPacket
{
  public:
    MIDIShowControlPacket();

    void Put_Go() { Put_Simple0(MIDI_SC_GO); }

    void Put_Go(MIDICue const& q_number) { Put_Simple1(MIDI_SC_GO, q_number); }

    void Put_Go(MIDICue const& q_number, MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_GO, q_number, q_list);
    }

    void Put_Go(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_GO, q_number, q_list, q_path);
    }

    void Put_Stop() { Put_Simple0(MIDI_SC_STOP); }

    void Put_Stop(MIDICue const& q_number) { Put_Simple1(MIDI_SC_STOP, q_number); }

    void Put_Stop(MIDICue const& q_number, MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_STOP, q_number, q_list);
    }

    void Put_Stop(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_STOP, q_number, q_list, q_path);
    }

    void Put_Resume() { Put_Simple0(MIDI_SC_RESUME); }

    void Put_Resume(MIDICue const& q_number) { Put_Simple1(MIDI_SC_RESUME, q_number); }

    void Put_Resume(MIDICue const& q_number, MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_RESUME, q_number, q_list);
    }

    void Put_Resume(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_RESUME, q_number, q_list, q_path);
    }

    void Put_TimedGo(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
    {
        Put_Simple0(MIDI_SC_TIMED_GO);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void Put_TimedGo(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number)
    {
        Put_Simple1(MIDI_SC_TIMED_GO, q_number);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void Put_TimedGo(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_TIMED_GO, q_number, q_list);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void Put_TimedGo(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list,
        MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_TIMED_GO, q_number, q_list, q_path);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void Put_Load(MIDICue const& q_number) { Put_Simple1(MIDI_SC_LOAD, q_number); }

    void Put_Load(MIDICue const& q_number, MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_LOAD, q_number, q_list);
    }

    void Put_Load(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_LOAD, q_number, q_list, q_path);
    }

    void Put_Set(std::uint32_t ctrl_num, std::uint32_t ctrl_val)
    {
        Put_Simple0(MIDI_SC_SET);
        SetControlNum(ctrl_num);
        SetControlVal(ctrl_val);
    }

    void Put_Set(
        std::uint32_t ctrl_num,
        std::uint32_t ctrl_val,
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff)
    {
        Put_Simple0(MIDI_SC_SET);
        SetControlNum(ctrl_num);
        SetControlVal(ctrl_val);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void Put_Fire(std::uint8_t macro_num)
    {
        Put_Simple0(MIDI_SC_ALL_OFF);
        SetMacroNum(macro_num);
    }

    void Put_AllOff() { Put_Simple0(MIDI_SC_ALL_OFF); }

    void Put_Restore() { Put_Simple0(MIDI_SC_RESTORE); }

    void Put_Reset() { Put_Simple0(MIDI_SC_RESET); }

    void Put_GoOff() { Put_Simple0(MIDI_SC_GO_OFF); }

    void Put_GoOff(MIDICue const& q_number) { Put_Simple1(MIDI_SC_GO_OFF, q_number); }

    void Put_GoOff(MIDICue const& q_number, MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_GO_OFF, q_number, q_list);
    }

    void Put_GoOff(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_GO_OFF, q_number, q_list, q_path);
    }

    void Put_GoJam() { Put_Simple0(MIDI_SC_GO_JAM); }

    void Put_GoJam(MIDICue const& q_number) { Put_Simple1(MIDI_SC_GO_JAM, q_number); }

    void Put_GoJam(MIDICue const& q_number, MIDICue const& q_list)
    {
        Put_Simple2(MIDI_SC_GO_JAM, q_number, q_list);
    }

    void Put_GoJam(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        Put_Simple3(MIDI_SC_GO_JAM, q_number, q_list, q_path);
    }

    void Put_StandbyPlus() { Put_Simple0(MIDI_SC_STANDBY_PLUS); }

    void Put_StandbyPlus(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_STANDBY_PLUS);
        SetQList(q_list);
        SetHasQList(true);
    }
    void Put_StandbyMinus() { Put_Simple0(MIDI_SC_STANDBY_MINUS); }

    void Put_StandbyMinus(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_STANDBY_MINUS);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_SequencePlus() { Put_Simple0(MIDI_SC_SEQUENCE_PLUS); }

    void Put_SequencePlus(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_SEQUENCE_PLUS);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_SequenceMinus() { Put_Simple0(MIDI_SC_SEQUENCE_MINUS); }

    void Put_SequenceMinus(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_SEQUENCE_MINUS);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_StartClock() { Put_Simple0(MIDI_SC_START_CLOCK); }

    void Put_StartClock(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_START_CLOCK);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_StopClock() { Put_Simple0(MIDI_SC_STOP_CLOCK); }

    void Put_StopClock(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_STOP_CLOCK);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_ZeroClock() { Put_Simple0(MIDI_SC_ZERO_CLOCK); }

    void Put_ZeroClock(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_ZERO_CLOCK);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_SetClock(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
    {
        Put_Simple0(MIDI_SC_SET_CLOCK);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void Put_SetClock(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_SET_CLOCK);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_MTCChaseOn() { Put_Simple0(MIDI_SC_MTC_CHASE_ON); }

    void Put_MTCChaseOn(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_MTC_CHASE_ON);
        SetQList(q_list);
        SetHasQList(true);
    }
    void Put_MTCChaseOff() { Put_Simple0(MIDI_SC_MTC_CHASE_OFF); }

    void Put_MTCChaseOff(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_MTC_CHASE_OFF);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_OpenQList(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_OPEN_Q_LIST);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_CloseQList(MIDICue const& q_list)
    {
        Put_Simple0(MIDI_SC_CLOSE_Q_LIST);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_OpenQPath(MIDICue const& q_path)
    {
        Put_Simple0(MIDI_SC_OPEN_Q_PATH);
        SetQPath(q_path);
        SetHasQPath(true);
    }

    void Put_CloseQPath(MIDICue const& q_path)
    {
        Put_Simple0(MIDI_SC_CLOSE_Q_PATH);
        SetQPath(q_path);
        SetHasQPath(true);
    }

    bool ParseEntireSysEx(MIDISystemExclusive const* e);
    bool StoreToSysEx(MIDISystemExclusive* e) const;

  protected:
    bool StoreTime(MIDISystemExclusive* e) const;
    bool ParseTime(MIDISystemExclusive const* e, int* pos);
    bool Store3Param(MIDISystemExclusive* e) const;
    bool Parse3Param(MIDISystemExclusive const* e, int* pos);
    bool StoreSet(MIDISystemExclusive* e) const;
    bool ParseSet(MIDISystemExclusive const* e, int* pos);
    bool StoreFire(MIDISystemExclusive* e) const;
    bool ParseFire(MIDISystemExclusive const* e, int* pos);
    bool StoreQPath(MIDISystemExclusive* e) const;
    bool ParseQPath(MIDISystemExclusive const* e, int* pos);
    bool StoreQList(MIDISystemExclusive* e) const;
    bool ParseQList(MIDISystemExclusive const* e, int* pos);
    bool StoreAscii(MIDISystemExclusive* e, char const* str) const;

    bool StoreAsciiNum(MIDISystemExclusive* e, MIDICue const& num) const;
    bool ParseAsciiNum(MIDISystemExclusive const* e, int* pos, MIDICue* num);
    bool ParseAsciiNum(MIDISystemExclusive const* e, int* pos, std::uint32_t* num);

    void ClearVariableStuff();

    void Put_Simple0(MIDIShowCommand cmd)
    {
        ClearVariableStuff();
        SetCommand(cmd);
    }

    void Put_Simple1(MIDIShowCommand cmd, MIDICue const& q_number)
    {
        ClearVariableStuff();
        SetCommand(cmd);
        SetQNumber(q_number);
        SetHasQNumber(true);
    }

    void Put_Simple2(MIDIShowCommand cmd, MIDICue const& q_number, MIDICue const& q_list)
    {
        ClearVariableStuff();
        SetCommand(cmd);
        SetQNumber(q_number);
        SetHasQNumber(true);
        SetQList(q_list);
        SetHasQList(true);
    }

    void Put_Simple3(
        MIDIShowCommand cmd, MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        ClearVariableStuff();
        SetCommand(cmd);
        SetQNumber(q_number);
        SetHasQNumber(true);
        SetQList(q_list);
        SetHasQList(true);
        SetQPath(q_path);
        SetHasQPath(true);
    }

    _ATTRIBUTE(std::uint8_t, DeviceId);
    _ATTRIBUTE(std::uint8_t, CommandFmt);
    _ATTRIBUTE(MIDIShowCommand, Command);
    _ATTRIBUTE(bool, HasTime);
    _ATTRIBUTE(bool, HasQNumber);
    _ATTRIBUTE(bool, HasQList);
    _ATTRIBUTE(bool, HasQPath);
    _ATTRIBUTE(std::uint8_t, Hours);
    _ATTRIBUTE(std::uint8_t, Minutes);
    _ATTRIBUTE(std::uint8_t, Seconds);
    _ATTRIBUTE(std::uint8_t, Frames);
    _ATTRIBUTE(std::uint8_t, FractFrames);

    _ATTRIBUTE_REF(MIDICue, QNumber);
    _ATTRIBUTE_REF(MIDICue, QList);
    _ATTRIBUTE_REF(MIDICue, QPath);

    _ATTRIBUTE(std::uint32_t, Val1);
    _ATTRIBUTE(std::uint32_t, Val2);

    _ACCESS(std::uint32_t, MacroNum, Val1);
    _ACCESS(std::uint32_t, ControlNum, Val1);
    _ACCESS(std::uint32_t, ControlVal, Val2);
};

// unlearn the brain damage
#undef _ATTRIBUTE
#undef _ATTRIBUTE_REF
#undef _ACCESS
#undef _ACCESS_REF
#undef _PATTRIBUTE
#undef _PATTRIBUTE_REF
#undef _PACCESS
#undef _PACCESS_REF

}  // namespace jdksmidi

#endif
