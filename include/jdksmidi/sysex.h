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


#ifndef JDKSMIDI_SYSEX_H
#define JDKSMIDI_SYSEX_H

#include "jdksmidi/midi.h"

#include <cstdint>
#include <vector>

namespace jdksmidi {

class MIDISystemExclusive
{
  public:
    MIDISystemExclusive(int size = 384);

    MIDISystemExclusive(MIDISystemExclusive const& e);

    MIDISystemExclusive(std::uint8_t* buf_, int max_len_, int cur_len_, bool deletable_)
        : _max_len(max_len_)
        , _chk_sum(0)
    {
        if (buf_ && cur_len_ > 0) {
            _buffer.assign(buf_, buf_ + cur_len_);
        }
    }

    virtual ~MIDISystemExclusive();

    void clear()
    {
        _buffer.clear();
        _chk_sum = 0;
    }
    void clear_checksum() { _chk_sum = 0; }

    void put_sys_byte(std::uint8_t b)  // does not add to chksum
    {
        _buffer.push_back(b);
    }

    void put_byte(std::uint8_t b)
    {
        put_sys_byte(b);
        _chk_sum += b;
    }

    void put_exc() { put_sys_byte(SYSEX_START); }
    void put_eox() { put_sys_byte(SYSEX_END); }

    // low nibble first
    void put_nibblized_byte(std::uint8_t b)
    {
        put_byte((std::uint8_t)(b & 0xf));
        put_byte((std::uint8_t)(b >> 4));
    }

    // high nibble first
    void put_nibblized_byte2(std::uint8_t b)
    {
        put_byte((std::uint8_t)(b >> 4));
        put_byte((std::uint8_t)(b & 0xf));
    }

    void put_checksum() { put_byte((std::uint8_t)(_chk_sum & 0x7f)); }

    std::uint8_t get_checksum() const { return (std::uint8_t)(_chk_sum & 0x7f); }

    int get_length() const { return static_cast<int>(_buffer.size()); }

    std::uint8_t get_data(int i) const
    {
        return (i >= 0 && i < static_cast<int>(_buffer.size())) ? _buffer[i] : 0;
    }

    bool is_full() const { return false; }

    std::uint8_t* get_buf() { return _buffer.empty() ? nullptr : _buffer.data(); }

    std::uint8_t const* get_buf() const { return _buffer.empty() ? nullptr : _buffer.data(); }

  private:
    std::vector<std::uint8_t> _buffer;
    int _max_len;
    std::uint8_t _chk_sum;
};
}  // namespace jdksmidi

#endif
