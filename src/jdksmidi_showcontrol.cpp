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

#include "jdksmidi/showcontrol.h"
#include "jdksmidi/world.h"

#include <cinttypes>

namespace jdksmidi {

MIDIShowControlPacket::MIDIShowControlPacket()
    : QNumber(0)
    , QList(0)
    , QPath(0)
{
    DeviceId = 0;
    CommandFmt = 0;
    Command = MIDI_SC_GO;
    HasTime = false;
    HasQNumber = false;
    HasQList = false;
    HasQPath = false;
    Hours = 0;
    Minutes = 0;
    Seconds = 0;
    Frames = 0;
    FractFrames = 0;
    Val1 = 0;
    Val2 = 0;
}

void MIDIShowControlPacket::ClearVariableStuff()
{
    QNumber.Clear();
    QList.Clear();
    QPath.Clear();
    Command = MIDI_SC_GO;
    HasTime = false;
    HasQNumber = false;
    HasQList = false;
    HasQPath = false;
    Hours = 0;
    Minutes = 0;
    Seconds = 0;
    Frames = 0;
    FractFrames = 0;
    Val1 = 0;
    Val2 = 0;
}

bool MIDIShowControlPacket::ParseEntireSysEx(MIDISystemExclusive const* e)
{
    bool f = true;
    int pos = 0;

    if (e->GetData(pos++) != 0xf0)
        return false;

    if (e->GetData(pos++) != 0x7f)
        return false;

    ClearVariableStuff();
    DeviceId = e->GetData(pos++);

    if (e->GetData(pos++) != 0x02)
        return false;

    CommandFmt = e->GetData(pos++);
    Command = (MIDIShowCommand)e->GetData(pos++);

    switch (Command) {
        case MIDI_SC_GO:
        case MIDI_SC_STOP:
        case MIDI_SC_RESUME:
        case MIDI_SC_LOAD:
        case MIDI_SC_GO_OFF:
        case MIDI_SC_GO_JAM: {
            f &= Parse3Param(e, &pos);
        } break;
        case MIDI_SC_TIMED_GO: {
            f &= ParseTime(e, &pos);
            f &= Parse3Param(e, &pos);
        } break;
        case MIDI_SC_SET: {
            f &= ParseSet(e, &pos);
        } break;
        case MIDI_SC_FIRE: {
            f &= ParseFire(e, &pos);
        } break;
        case MIDI_SC_ALL_OFF:
        case MIDI_SC_RESTORE:
        case MIDI_SC_RESET:
            // no additional params.
            break;
        case MIDI_SC_STANDBY_PLUS:
        case MIDI_SC_STANDBY_MINUS:
        case MIDI_SC_SEQUENCE_PLUS:
        case MIDI_SC_SEQUENCE_MINUS:
        case MIDI_SC_START_CLOCK:
        case MIDI_SC_STOP_CLOCK:
        case MIDI_SC_ZERO_CLOCK:
        case MIDI_SC_MTC_CHASE_ON:
        case MIDI_SC_MTC_CHASE_OFF: {
            if (e->GetData(pos) != 0xf7 && e->GetData(pos) != 0)
                f &= ParseQList(e, &pos);
        } break;
        case MIDI_SC_SET_CLOCK: {
            f &= ParseTime(e, &pos);

            if (e->GetData(pos) != 0xf7 && e->GetData(pos) != 0)
                f &= ParseQList(e, &pos);
        } break;
        case MIDI_SC_OPEN_Q_LIST:
        case MIDI_SC_CLOSE_Q_LIST: {
            f &= ParseQList(e, &pos);
        } break;
        case MIDI_SC_OPEN_Q_PATH:
        case MIDI_SC_CLOSE_Q_PATH: {
            f &= ParseQPath(e, &pos);
        } break;
        default:
            f = false;  // unrecognized command
            break;
    }

    if (e->GetLength() < pos) {
        return false;
    }

    return f;
}

bool MIDIShowControlPacket::StoreToSysEx(MIDISystemExclusive* e) const
{
    bool f = true;
    e->Clear();
    e->PutEXC();
    e->PutByte(0x7f);
    e->PutByte(DeviceId);
    e->PutByte(0x02);
    e->PutByte(CommandFmt);
    e->PutByte(Command);

    switch (Command) {
        case MIDI_SC_GO:
        case MIDI_SC_STOP:
        case MIDI_SC_RESUME:
        case MIDI_SC_LOAD:
        case MIDI_SC_GO_OFF:
        case MIDI_SC_GO_JAM: {
            f &= Store3Param(e);
        } break;
        case MIDI_SC_TIMED_GO: {
            f &= StoreTime(e);
            f &= Store3Param(e);
        } break;
        case MIDI_SC_SET: {
            f &= StoreSet(e);
        } break;
        case MIDI_SC_FIRE: {
            f &= StoreFire(e);
        } break;
        case MIDI_SC_ALL_OFF:
        case MIDI_SC_RESTORE:
        case MIDI_SC_RESET:
            // no additional params.
            break;
        case MIDI_SC_STANDBY_PLUS:
        case MIDI_SC_STANDBY_MINUS:
        case MIDI_SC_SEQUENCE_PLUS:
        case MIDI_SC_SEQUENCE_MINUS:
        case MIDI_SC_START_CLOCK:
        case MIDI_SC_STOP_CLOCK:
        case MIDI_SC_ZERO_CLOCK:
        case MIDI_SC_MTC_CHASE_ON:
        case MIDI_SC_MTC_CHASE_OFF: {
            if (HasQList)
                f &= StoreQList(e);
        } break;
        case MIDI_SC_SET_CLOCK: {
            f &= StoreTime(e);

            if (HasQList)
                f &= StoreQList(e);
        } break;
        case MIDI_SC_OPEN_Q_LIST:
        case MIDI_SC_CLOSE_Q_LIST: {
            f &= StoreQList(e);
        } break;
        case MIDI_SC_OPEN_Q_PATH:
        case MIDI_SC_CLOSE_Q_PATH: {
            f &= StoreQPath(e);
        } break;
        default:
            f = false;  // unrecognized command
            break;
    }

    e->PutEOX();

    if (e->IsFull())
        return false;

    return f;
}

bool MIDIShowControlPacket::StoreTime(MIDISystemExclusive* e) const
{
    if (HasTime) {
        e->PutByte(Hours);
        e->PutByte(Minutes);
        e->PutByte(Seconds);
        e->PutByte(Frames);
        e->PutByte(FractFrames);
        return true;
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::ParseTime(MIDISystemExclusive const* e, int* pos)
{
    Hours = e->GetData((*pos)++);
    Minutes = e->GetData((*pos)++);
    Seconds = e->GetData((*pos)++);
    Frames = e->GetData((*pos)++);
    FractFrames = e->GetData((*pos)++);
    return true;
}

bool MIDIShowControlPacket::Store3Param(MIDISystemExclusive* e) const
{
    bool f = true;

    if (HasQNumber) {
        f &= StoreAsciiNum(e, GetQNumber());

        if (HasQList) {
            e->PutByte(0);
            f &= StoreAsciiNum(e, GetQList());

            if (HasQPath) {
                e->PutByte(0);
                f &= StoreAsciiNum(e, GetQPath());
            }
        }
    }

    return f;
}

bool MIDIShowControlPacket::Parse3Param(MIDISystemExclusive const* e, int* pos)
{
    bool f = true;
    MIDICue v;

    if (e->GetData(*pos) != 0xf7) {
        //
        // Read the Q number
        //
        f = ParseAsciiNum(e, pos, &v);

        if (f) {
            HasQNumber = true;
            SetQNumber(v);

            if (*pos < e->GetLength()) {
                //
                // read the q list
                //
                f = ParseAsciiNum(e, pos, &v);

                if (f) {
                    HasQList = true;
                    SetQList(v);

                    if (*pos < e->GetLength()) {
                        //
                        // read the q path
                        //
                        f = ParseAsciiNum(e, pos, &v);

                        if (f) {
                            HasQPath = true;
                            SetQPath(v);
                        }
                    }
                }
            }
        }
    }

    return f;
}

bool MIDIShowControlPacket::StoreSet(MIDISystemExclusive* e) const
{
    bool f = true;
    e->PutByte(static_cast<std::uint8_t>(GetControlNum() & 0x7f));
    e->PutByte(static_cast<std::uint8_t>((GetControlNum() >> 7) & 0x7f));
    e->PutByte(static_cast<std::uint8_t>(GetControlVal() & 0x7f));
    e->PutByte(static_cast<std::uint8_t>((GetControlVal() >> 7) & 0x7f));

    if (HasTime) {
        f = StoreTime(e);
    }

    return f;
}

bool MIDIShowControlPacket::ParseSet(MIDISystemExclusive const* e, int* pos)
{
    std::uint32_t v;
    v = e->GetData((*pos)++);
    v += (e->GetData((*pos)++) << 7);
    SetControlNum(v);
    v = e->GetData((*pos)++);
    v += (e->GetData((*pos)++) << 7);
    SetControlVal(v);

    if (e->GetData(*pos) != 0xf7) {
        return ParseTime(e, pos);
    }

    else {
        return true;
    }
}

bool MIDIShowControlPacket::StoreFire(MIDISystemExclusive* e) const
{
    e->PutByte(static_cast<std::uint8_t>(GetMacroNum()));
    return true;
}

bool MIDIShowControlPacket::ParseFire(MIDISystemExclusive const* e, int* pos)
{
    int v;
    v = e->GetData((*pos)++);
    SetMacroNum(v);
    return true;
}

bool MIDIShowControlPacket::StoreQPath(MIDISystemExclusive* e) const
{
    if (HasQPath) {
        return StoreAsciiNum(e, GetQPath());
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::ParseQPath(MIDISystemExclusive const* e, int* pos)
{
    bool f = true;
    MIDICue v;

    if (e->GetData(*pos) != 0xf7) {
        //
        // Read the Q Path
        //
        f = ParseAsciiNum(e, pos, &v);
        SetQPath(v);
        HasQPath = true;
        return f;
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::StoreQList(MIDISystemExclusive* e) const
{
    if (HasQList) {
        return StoreAsciiNum(e, GetQList());
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::ParseQList(MIDISystemExclusive const* e, int* pos)
{
    bool f = true;
    MIDICue v;

    if (e->GetData(*pos) != 0xf7) {
        //
        // Read the Q list
        //
        f = ParseAsciiNum(e, pos, &v);
        SetQList(v);
        HasQList = true;
        return f;
    }

    else {
        return false;
    }
}

bool MIDIShowControlPacket::StoreAscii(MIDISystemExclusive* e, char const* str) const
{
    while (*str) {
        e->PutByte(*str++);
    }

    return true;
}

bool MIDIShowControlPacket::StoreAsciiNum(MIDISystemExclusive* e, MIDICue const& num) const
{
    char buf[32];
    bool f = false;
    *buf = '\0';

    switch (num.GetNumValues()) {
        default:
        case 0:
            break;
        case 1:
            snprintf(buf, sizeof(buf), "%" PRIu32, num.GetV1());
            break;
        case 2:
            snprintf(buf, sizeof(buf), "%" PRIu32 ".%" PRIu32, num.GetV1(), num.GetV2());
            break;
        case 3:
            snprintf(
                buf,
                sizeof(buf),
                "%" PRIu32 ".%" PRIu32 ".%" PRIu32,
                num.GetV1(),
                num.GetV2(),
                num.GetV3());
            break;
    }

    f = StoreAscii(e, buf);
    return f;
}

bool MIDIShowControlPacket::ParseAsciiNum(MIDISystemExclusive const* e, int* pos, MIDICue* num)
{
    std::uint32_t v;
    std::uint8_t c;
    bool f;
    // check if there is a field
    c = e->GetData(*pos);

    if (c == 0xf7) {
        // no field
        return false;
    }

    f = ParseAsciiNum(e, pos, &v);

    if (f) {
        num->SetNumValues(1);
        num->SetV1(v);
    }

    //
    // Is there another field?
    //
    c = e->GetData(*pos);

    if (c == '.') {
        //
        // yes read it
        //
        (*pos)++;
        f = ParseAsciiNum(e, pos, &v);

        if (f) {
            num->SetNumValues(2);
            num->SetV2(v);
        }

        //
        // Is there another field?
        //
        c = e->GetData(*pos);

        if (c == '.') {
            //
            // yes read it
            //
            (*pos)++;
            f = ParseAsciiNum(e, pos, &v);

            if (f) {
                num->SetNumValues(2);
                num->SetV3(v);
            }

            //
            // Is there more fields?
            //
            c = e->GetData(*pos);

            if (c == '.') {
                //
                // Yes, skip them
                //
                while (*pos < e->GetLength()) {
                    c = e->GetData((*pos));

                    if (c == 0xf7 || c == 0x00) {
                        break;
                    }

                    (*pos)++;
                }
            }
        }
    }

    while (++(*pos) < e->GetLength()) {
        if (e->GetData(*pos) != 0)
            break;
    }

    return f;
}

bool MIDIShowControlPacket::ParseAsciiNum(
    MIDISystemExclusive const* e, int* pos, std::uint32_t* num)
{
    bool f = true;
    std::uint32_t v = 0;
    std::uint8_t c = 0;
    //
    // Read ascii decimal digits until '.' or 0x00 or 0xf7
    //

    while (*pos < e->GetLength()) {
        c = e->GetData((*pos));

        if (c >= '0' && c <= '9') {
            //
            // If the character is ascii number then shift our current value and
            // add the digit
            //
            v *= 10;
            v += c - '0';
        }

        else if (c == 0x00 || c == 0xf7 || c == '.') {
            //
            // If the character is 0 or EOX or . then this field is over
            //
            *num = v;
            return f;
        }

        (*pos)++;
    }

    *num = v;
    return false;
}

}  // namespace jdksmidi
