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

    std::uint32_t get_num_values() const { return num_values; }
    std::uint32_t get_v1() const { return v1; }
    std::uint32_t get_v2() const { return v2; }
    std::uint32_t get_v3() const { return v3; }

    void set_num_values(int a) { num_values = a; }
    void set_v1(std::uint32_t a) { v1 = a; }
    void set_v2(std::uint32_t a) { v2 = a; }
    void set_v3(std::uint32_t a) { v3 = a; }

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
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
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
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
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
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
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
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
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
        set_control_num(ctrl_num);
        set_control_val(ctrl_val);
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
        set_control_num(ctrl_num);
        set_control_val(ctrl_val);
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
    }

    void put_fire(std::uint8_t macro_num)
    {
        put_simple0(MIDI_SC_ALL_OFF);
        set_macro_num(macro_num);
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
        set_q_list(q_list);
        set_has_q_list(true);
    }
    void put_standby_minus() { put_simple0(MIDI_SC_STANDBY_MINUS); }

    void put_standby_minus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_STANDBY_MINUS);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_sequence_plus() { put_simple0(MIDI_SC_SEQUENCE_PLUS); }

    void put_sequence_plus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_SEQUENCE_PLUS);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_sequence_minus() { put_simple0(MIDI_SC_SEQUENCE_MINUS); }

    void put_sequence_minus(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_SEQUENCE_MINUS);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_start_clock() { put_simple0(MIDI_SC_START_CLOCK); }

    void put_start_clock(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_START_CLOCK);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_stop_clock() { put_simple0(MIDI_SC_STOP_CLOCK); }

    void put_stop_clock(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_STOP_CLOCK);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_zero_clock() { put_simple0(MIDI_SC_ZERO_CLOCK); }

    void put_zero_clock(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_ZERO_CLOCK);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_set_clock(
        std::uint8_t hr, std::uint8_t mn, std::uint8_t sc, std::uint8_t fr, std::uint8_t ff)
    {
        put_simple0(MIDI_SC_SET_CLOCK);
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
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
        set_hours(hr);
        set_minutes(mn);
        set_seconds(sc);
        set_frames(fr);
        set_fract_frames(ff);
        set_has_time(true);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_mtc_chase_on() { put_simple0(MIDI_SC_MTC_CHASE_ON); }

    void put_mtc_chase_on(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_MTC_CHASE_ON);
        set_q_list(q_list);
        set_has_q_list(true);
    }
    void put_mtc_chase_off() { put_simple0(MIDI_SC_MTC_CHASE_OFF); }

    void put_mtc_chase_off(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_MTC_CHASE_OFF);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_open_q_list(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_OPEN_Q_LIST);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_close_q_list(MIDICue const& q_list)
    {
        put_simple0(MIDI_SC_CLOSE_Q_LIST);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_open_q_path(MIDICue const& q_path)
    {
        put_simple0(MIDI_SC_OPEN_Q_PATH);
        set_q_path(q_path);
        set_has_q_path(true);
    }

    void put_close_q_path(MIDICue const& q_path)
    {
        put_simple0(MIDI_SC_CLOSE_Q_PATH);
        set_q_path(q_path);
        set_has_q_path(true);
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
        set_command(cmd);
    }

    void put_simple1(MIDIShowCommand cmd, MIDICue const& q_number)
    {
        clear_variable_stuff();
        set_command(cmd);
        set_q_number(q_number);
        set_has_q_number(true);
    }

    void put_simple2(MIDIShowCommand cmd, MIDICue const& q_number, MIDICue const& q_list)
    {
        clear_variable_stuff();
        set_command(cmd);
        set_q_number(q_number);
        set_has_q_number(true);
        set_q_list(q_list);
        set_has_q_list(true);
    }

    void put_simple3(
        MIDIShowCommand cmd, MIDICue const& q_number, MIDICue const& q_list, MIDICue const& q_path)
    {
        clear_variable_stuff();
        set_command(cmd);
        set_q_number(q_number);
        set_has_q_number(true);
        set_q_list(q_list);
        set_has_q_list(true);
        set_q_path(q_path);
        set_has_q_path(true);
    }

  public:
    // device_id attribute
    std::uint8_t get_device_id() const { return device_id; }
    void set_device_id(std::uint8_t a) { device_id = a; }

    // command_fmt attribute  
    std::uint8_t get_command_fmt() const { return command_fmt; }
    void set_command_fmt(std::uint8_t a) { command_fmt = a; }

    // command attribute
    MIDIShowCommand get_command() const { return command; }
    void set_command(MIDIShowCommand a) { command = a; }

    // HasTime attribute
    bool get_has_time() const { return has_time; }
    void set_has_time(bool a) { has_time = a; }

    // HasQNumber attribute
    bool get_has_q_number() const { return has_q_number; }
    void set_has_q_number(bool a) { has_q_number = a; }

    // HasQList attribute
    bool get_has_q_list() const { return has_q_list; }
    void set_has_q_list(bool a) { has_q_list = a; }

    // HasQPath attribute
    bool get_has_q_path() const { return has_q_path; }
    void set_has_q_path(bool a) { has_q_path = a; }

    // hours attribute
    std::uint8_t get_hours() const { return hours; }
    void set_hours(std::uint8_t a) { hours = a; }

    // minutes attribute
    std::uint8_t get_minutes() const { return minutes; }
    void set_minutes(std::uint8_t a) { minutes = a; }

    // seconds attribute
    std::uint8_t get_seconds() const { return seconds; }
    void set_seconds(std::uint8_t a) { seconds = a; }

    // frames attribute
    std::uint8_t get_frames() const { return frames; }
    void set_frames(std::uint8_t a) { frames = a; }

    // fract_frames attribute
    std::uint8_t get_fract_frames() const { return fract_frames; }
    void set_fract_frames(std::uint8_t a) { fract_frames = a; }

    // q_number attribute (reference)
    const MIDICue& get_q_number() const { return q_number; }
    void set_q_number(const MIDICue& a) { q_number = a; }

    // q_list attribute (reference)
    const MIDICue& get_q_list() const { return q_list; }
    void set_q_list(const MIDICue& a) { q_list = a; }

    // q_path attribute (reference)
    const MIDICue& get_q_path() const { return q_path; }
    void set_q_path(const MIDICue& a) { q_path = a; }

    // val1 attribute
    std::uint32_t get_val1() const { return val1; }
    void set_val1(std::uint32_t a) { val1 = a; }

    // val2 attribute
    std::uint32_t get_val2() const { return val2; }
    void set_val2(std::uint32_t a) { val2 = a; }

    // MacroNum access (aliases val1)
    std::uint32_t get_macro_num() const { return val1; }
    void set_macro_num(std::uint32_t a) { val1 = a; }

    // ControlNum access (aliases val1)
    std::uint32_t get_control_num() const { return val1; }
    void set_control_num(std::uint32_t a) { val1 = a; }

    // ControlVal access (aliases val2)
    std::uint32_t get_control_val() const { return val2; }
    void set_control_val(std::uint32_t a) { val2 = a; }

  private:
    std::uint8_t device_id;
    std::uint8_t command_fmt;
    MIDIShowCommand command;
    bool has_time;
    bool has_q_number;
    bool has_q_list;
    bool has_q_path;
    std::uint8_t hours;
    std::uint8_t minutes;
    std::uint8_t seconds;
    std::uint8_t frames;
    std::uint8_t fract_frames;
    MIDICue q_number;
    MIDICue q_list;
    MIDICue q_path;
    std::uint32_t val1;
    std::uint32_t val2;
};

// Macros have been removed and replaced with explicit method definitions

}  // namespace jdksmidi

#endif
