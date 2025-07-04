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
// This file previously used JDKSMIDI_ATTRIBUTE family macros to generate
// getter/setter methods. These have been replaced with explicit method 
// definitions for better code clarity and maintainability.
//


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

    void clear()
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

    bool operator==(std::uint32_t v) { return v1 == v; }

    MIDICue const& operator==(MIDICue const& c);
    bool operator!=(std::uint32_t v) { return v1 != v; }
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

    void put_go() { put_simple0(MIDI_SC_GO); }

    void put_go(MIDICue const& q_number) { put_simple1(MIDI_SC_GO, q_number); }

    void put_go(MIDICue const& q_number, MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_GO, q_number, q_list);
    }

    void put_go(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_GO, q_number, q_list, q_path);
    }

    void put_stop() { put_simple0(MIDI_SC_STOP); }

    void put_stop(MIDICue const& q_number) { put_simple1(MIDI_SC_STOP, q_number); }

    void put_stop(MIDICue const& q_number, MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_STOP, q_number, q_list);
    }

    void put_stop(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_STOP, q_number, q_list, q_path);
    }

    void put_resume() { put_simple0(MIDI_SC_RESUME); }

    void put_resume(MIDICue const& q_number) { put_simple1(MIDI_SC_RESUME, q_number); }

    void put_resume(MIDICue const& q_number, MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_RESUME, q_number, q_list);
    }

    void put_resume(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_RESUME, q_number, q_list, q_path);
    }

    void put_timed_go(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
    {
        put_simple0(MIDI_SC_TIMED_GO);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void put_timed_go(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number)
    {
        put_simple1(MIDI_SC_TIMED_GO, q_number);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void put_timed_go(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_TIMED_GO, q_number, q_list);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void put_timed_go(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_number,
        MIDICue const& q_list,
        MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_TIMED_GO, q_number, q_list, q_path);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void put_load(MIDICue const& q_number) { put_simple1(MIDI_SC_LOAD, q_number); }

    void put_load(MIDICue const& q_number, MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_LOAD, q_number, q_list);
    }

    void put_load(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_LOAD, q_number, q_list, q_path);
    }

    void put_set(std::uint32_t ctrl_num, std::uint32_t ctrl_val)
    {
        put_simple0(MIDI_SC_SET);
        SetControlNum(ctrl_num);
        SetControlVal(ctrl_val);
    }

    void put_set(
        std::uint32_t ctrl_num,
        std::uint32_t ctrl_val,
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff)
    {
        put_simple0(MIDI_SC_SET);
        SetControlNum(ctrl_num);
        SetControlVal(ctrl_val);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void put_fire(std::uint8_t macro_num)
    {
        put_simple0(MIDI_SC_ALL_OFF);
        SetMacroNum(macro_num);
    }

    void put_all_off() { put_simple0(MIDI_SC_ALL_OFF); }

    void put_restore() { put_simple0(MIDI_SC_RESTORE); }

    void put_reset() { put_simple0(MIDI_SC_RESET); }

    void put_go_off() { put_simple0(MIDI_SC_GO_OFF); }

    void put_go_off(MIDICue const& q_number) { put_simple1(MIDI_SC_GO_OFF, q_number); }

    void put_go_off(MIDICue const& q_number, MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_GO_OFF, q_number, q_list);
    }

    void put_go_off(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_GO_OFF, q_number, q_list, q_path);
    }

    void put_go_jam() { put_simple0(MIDI_SC_GO_JAM); }

    void put_go_jam(MIDICue const& q_number) { put_simple1(MIDI_SC_GO_JAM, q_number); }

    void put_go_jam(MIDICue const& q_number, MIDICue const& q_list)
    {
        put_simple2(MIDI_SC_GO_JAM, q_number, q_list);
    }

    void put_go_jam(MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        put_simple3(MIDI_SC_GO_JAM, q_number, q_list, q_path);
    }

    void put_standby_plus() { put_simple0(MIDI_SC_STANDBY_PLUS); }

    void put_standby_plus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_STANDBY_PLUS);
        SetQList(q_list);
        SetHasQList(true);
    }
    void put_standby_minus() { put_simple0(MIDI_SC_STANDBY_MINUS); }

    void put_standby_minus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_STANDBY_MINUS);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_sequence_plus() { put_simple0(MIDI_SC_SEQUENCE_PLUS); }

    void put_sequence_plus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_SEQUENCE_PLUS);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_sequence_minus() { put_simple0(MIDI_SC_SEQUENCE_MINUS); }

    void put_sequence_minus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_SEQUENCE_MINUS);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_start_clock() { put_simple0(MIDI_SC_START_CLOCK); }

    void put_start_clock(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_START_CLOCK);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_stop_clock() { put_simple0(MIDI_SC_STOP_CLOCK); }

    void put_stop_clock(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_STOP_CLOCK);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_zero_clock() { put_simple0(MIDI_SC_ZERO_CLOCK); }

    void put_zero_clock(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_ZERO_CLOCK);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_set_clock(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
    {
        put_simple0(MIDI_SC_SET_CLOCK);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
    }

    void put_set_clock(
        std::uint8_t hr,
        std::uint8_t mn,
        std::uint8_t sc,
        std::uint8_t fr,
        std::uint8_t ff,
        MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_SET_CLOCK);
        SetHours(hr);
        SetMinutes(mn);
        SetSeconds(sc);
        SetFrames(fr);
        SetFractFrames(ff);
        SetHasTime(true);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_mtc_chase_on() { put_simple0(MIDI_SC_MTC_CHASE_ON); }

    void put_mtc_chase_on(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_MTC_CHASE_ON);
        SetQList(q_list);
        SetHasQList(true);
    }
    void put_mtc_chase_off() { put_simple0(MIDI_SC_MTC_CHASE_OFF); }

    void put_mtc_chase_off(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_MTC_CHASE_OFF);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_open_q_list(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_OPEN_Q_LIST);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_close_q_list(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_CLOSE_Q_LIST);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_open_q_path(MIDICue const& q_path)
    {
        put_simple0(MIDI_SC_OPEN_Q_PATH);
        SetQPath(q_path);
        SetHasQPath(true);
    }

    void put_close_q_path(MIDICue const& q_path)
    {
        put_simple0(MIDI_SC_CLOSE_Q_PATH);
        SetQPath(q_path);
        SetHasQPath(true);
    }

    bool parse_entire_sys_ex(MIDISystemExclusive const* e);
    bool store_to_sys_ex(MIDISystemExclusive* e) const;

  protected:
    bool store_time(MIDISystemExclusive* e) const;
    bool parse_time(MIDISystemExclusive const* e, int* pos);
    bool store_3_param(MIDISystemExclusive* e) const;
    bool parse_3_param(MIDISystemExclusive const* e, int* pos);
    bool store_set(MIDISystemExclusive* e) const;
    bool parse_set(MIDISystemExclusive const* e, int* pos);
    bool store_fire(MIDISystemExclusive* e) const;
    bool parse_fire(MIDISystemExclusive const* e, int* pos);
    bool store_q_path(MIDISystemExclusive* e) const;
    bool parse_q_path(MIDISystemExclusive const* e, int* pos);
    bool store_q_list(MIDISystemExclusive* e) const;
    bool parse_q_list(MIDISystemExclusive const* e, int* pos);
    bool store_ascii(MIDISystemExclusive* e, char const* str) const;

    bool store_ascii_num(MIDISystemExclusive* e, MIDICue const& num) const;
    bool parse_ascii_num(MIDISystemExclusive const* e, int* pos, MIDICue* num);
    bool parse_ascii_num(MIDISystemExclusive const* e, int* pos, std::uint32_t* num);

    void clear_variable_stuff();

    void put_simple0(MIDIShowCommand cmd)
    {
        clear_variable_stuff();
        SetCommand(cmd);
    }

    void put_simple1(MIDIShowCommand cmd, MIDICue const& q_number)
    {
        clear_variable_stuff();
        SetCommand(cmd);
        SetQNumber(q_number);
        SetHasQNumber(true);
    }

    void put_simple2(MIDIShowCommand cmd, MIDICue const& q_number, MIDICue const& q_list)
    {
        clear_variable_stuff();
        SetCommand(cmd);
        SetQNumber(q_number);
        SetHasQNumber(true);
        SetQList(q_list);
        SetHasQList(true);
    }

    void put_simple3(
        MIDIShowCommand cmd, MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        clear_variable_stuff();
        SetCommand(cmd);
        SetQNumber(q_number);
        SetHasQNumber(true);
        SetQList(q_list);
        SetHasQList(true);
        SetQPath(q_path);
        SetHasQPath(true);
    }

  public:
    // DeviceId attribute
    std::uint8_t GetDeviceId() const { return DeviceId; }
    void SetDeviceId(std::uint8_t a) { DeviceId = a; }

    // CommandFmt attribute  
    std::uint8_t GetCommandFmt() const { return CommandFmt; }
    void SetCommandFmt(std::uint8_t a) { CommandFmt = a; }

    // Command attribute
    MIDIShowCommand GetCommand() const { return Command; }
    void SetCommand(MIDIShowCommand a) { Command = a; }

    // HasTime attribute
    bool GetHasTime() const { return HasTime; }
    void SetHasTime(bool a) { HasTime = a; }

    // HasQNumber attribute
    bool GetHasQNumber() const { return HasQNumber; }
    void SetHasQNumber(bool a) { HasQNumber = a; }

    // HasQList attribute
    bool GetHasQList() const { return HasQList; }
    void SetHasQList(bool a) { HasQList = a; }

    // HasQPath attribute
    bool GetHasQPath() const { return HasQPath; }
    void SetHasQPath(bool a) { HasQPath = a; }

    // Hours attribute
    std::uint8_t GetHours() const { return Hours; }
    void SetHours(std::uint8_t a) { Hours = a; }

    // Minutes attribute
    std::uint8_t GetMinutes() const { return Minutes; }
    void SetMinutes(std::uint8_t a) { Minutes = a; }

    // Seconds attribute
    std::uint8_t GetSeconds() const { return Seconds; }
    void SetSeconds(std::uint8_t a) { Seconds = a; }

    // Frames attribute
    std::uint8_t GetFrames() const { return Frames; }
    void SetFrames(std::uint8_t a) { Frames = a; }

    // FractFrames attribute
    std::uint8_t GetFractFrames() const { return FractFrames; }
    void SetFractFrames(std::uint8_t a) { FractFrames = a; }

    // QNumber attribute (reference)
    const MIDICue& GetQNumber() const { return QNumber; }
    void SetQNumber(const MIDICue& a) { QNumber = a; }

    // QList attribute (reference)
    const MIDICue& GetQList() const { return QList; }
    void SetQList(const MIDICue& a) { QList = a; }

    // QPath attribute (reference)
    const MIDICue& GetQPath() const { return QPath; }
    void SetQPath(const MIDICue& a) { QPath = a; }

    // Val1 attribute
    std::uint32_t GetVal1() const { return Val1; }
    void SetVal1(std::uint32_t a) { Val1 = a; }

    // Val2 attribute
    std::uint32_t GetVal2() const { return Val2; }
    void SetVal2(std::uint32_t a) { Val2 = a; }

    // MacroNum access (aliases Val1)
    std::uint32_t GetMacroNum() const { return Val1; }
    void SetMacroNum(std::uint32_t a) { Val1 = a; }

    // ControlNum access (aliases Val1)
    std::uint32_t GetControlNum() const { return Val1; }
    void SetControlNum(std::uint32_t a) { Val1 = a; }

    // ControlVal access (aliases Val2)
    std::uint32_t GetControlVal() const { return Val2; }
    void SetControlVal(std::uint32_t a) { Val2 = a; }

  private:
    std::uint8_t DeviceId;
    std::uint8_t CommandFmt;
    MIDIShowCommand Command;
    bool HasTime;
    bool HasQNumber;
    bool HasQList;
    bool HasQPath;
    std::uint8_t Hours;
    std::uint8_t Minutes;
    std::uint8_t Seconds;
    std::uint8_t Frames;
    std::uint8_t FractFrames;
    MIDICue QNumber;
    MIDICue QList;
    MIDICue QPath;
    std::uint32_t Val1;
    std::uint32_t Val2;
};

// Macros have been removed and replaced with explicit method definitions

}  // namespace jdksmidi

#endif
