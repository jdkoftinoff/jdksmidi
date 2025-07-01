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

#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/parser.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/world.h"

#include <cinttypes>

using namespace jdksmidi;

void PrintSysEx(FILE* f, MIDISystemExclusive* ex)
{
    int l = ex->GetLength();
    fprintf(f, "Sysex Len=%d", l);

    for (int i = 0; i < l; ++i) {
        if (((i) % 20) == 0) {
            fprintf(f, "\n");
        }

        fprintf(f, "%02" PRIx8 " ", ex->GetData(i));
    }

    fprintf(f, "\n");
    fflush(f);
}

void PrintMsg(FILE* f, MIDIMessage* m)
{
    int l = m->GetLength();
    fprintf(f, "Msg : ");

    if (l == 1) {
        fprintf(f, " %02x \t=", m->GetStatus());
    }

    else if (l == 2) {
        fprintf(f, " %02x %02x \t=", m->GetStatus(), m->GetByte1());
    }

    else if (l == 3) {
        fprintf(f, " %02x %02x %02x \t=", m->GetStatus(), m->GetByte1(), m->GetByte2());
    }

    char buf[129];
    m->MsgToText(buf);
    fprintf(f, "%s\n", buf);
    fflush(f);
}

int main(int argc, char** argv)
{
    fprintf(stdout, "mdparse:\n");
    MIDIParser p(32 * 1024);
    MIDIMessage m;
    FILE* f = stdin;

    while (!feof(f)) {
        int c = fgetc(f);

        if (c == EOF)
            break;

        if (p.Parse(static_cast<std::uint8_t>(c), &m)) {
            if (m.IsSysEx()) {
                PrintSysEx(stdout, p.GetSystemExclusive());
            }

            else {
                PrintMsg(stdout, &m);
            }
        }
    }

    return 0;
}
