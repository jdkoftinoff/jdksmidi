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

#include "jdksmidi/world.h"
#include "jdksmidi/driver.h"

namespace jdksmidi
{

std::chrono::steady_clock::time_point MIDIDriver::session_start = std::chrono::steady_clock::now();

MIDIDriver::MIDIDriver ( int queue_size )
    :
    m_pMidiIn ( 0 ),
    m_pMidiOut ( 0 ),
    in_queue ( queue_size ),
    out_queue ( queue_size ),
    in_proc ( 0 ),
    out_proc ( 0 ),
    thru_proc ( 0 ),
    thru_enable ( false ),
    timer_open ( false ),
    tick_proc ( 0 )
{
}

MIDIDriver::~MIDIDriver()
{
}

void MIDIDriver::Reset()
{
    in_queue.Clear();
    out_queue.Clear();
    out_matrix.Clear();
}

void MIDIDriver::OutputMessage( MIDITimedBigMessage &msg )
{
    // don't send meta events and beat markers
    if ( msg.IsServiceMsg() || msg.IsMetaEvent() )
    {
        return;
    }
    if ( ( out_proc && out_proc->Process( &msg ) ) || !out_proc )
    {
        out_matrix.Process( msg );
        out_queue.Put( msg );
    }
}

void MIDIDriver::AllNotesOff( int chan )
{
    if ( m_pMidiOut != NULL )
    {
        MIDITimedBigMessage msg;
        // send a note off for every note on in the out_matrix

        if ( out_matrix.GetChannelCount( chan ) > 0 )
        {
            for ( int note = 0; note < 128; ++note )
            {
                while ( out_matrix.GetNoteCount( chan, note ) > 0 )
                {
                    // make a note off with note on msg, velocity 0
                    msg.SetNoteOn( (unsigned char)chan, (unsigned char)note, 0 );
                    OutputMessage( msg );
                }
            }
        }

        msg.SetControlChange( chan, C_DAMPER, 0 );
        OutputMessage( msg );
        msg.SetAllNotesOff( (unsigned char)chan );
        OutputMessage( msg );
    }
}

void MIDIDriver::AllNotesOff()
{
    for ( int i = 0; i < 16; ++i )
    {
        AllNotesOff( i );
    }
}

bool MIDIDriver::HardwareMsgIn( MIDITimedBigMessage &msg )
{
    // put input midi messages thru the in processor
    if ( in_proc )
    {
        if ( in_proc->Process( &msg ) == false )
        {
            // message was deleted, so ignore it.
            return true;
        }
    }

    // stick input into in queue

    if ( in_queue.CanPut() )
    {
        in_queue.Put( msg );
    }

    else
    {
        return false;
    }

    // now stick it through the THRU processor

    if ( thru_proc )
    {
        if ( thru_proc->Process( &msg ) == false )
        {
            // message was deleted, so ignore it.
            return true;
        }
    }

    if ( thru_enable )
    {
        // stick this message into the out queue so the tick procedure
        // will play it out asap
        if ( out_queue.CanPut() )
        {
            out_queue.Put( msg );
        }

        else
        {
            return false;
        }
    }

    return true;
}

// NEW: THESE WERE MOVED HERE FROM ALSADriver.cpp by GP
bool MIDIDriver::OpenMIDIInPort( int id )
{
    if ( m_pMidiIn != NULL ) // close any open port
    {
        delete m_pMidiIn;
        m_pMidiIn = NULL;
    }
    if ( m_pMidiIn == NULL )
    {
        try
        {
            m_pMidiIn = new RtMidiIn( RtMidi::UNSPECIFIED, m_strClientName );
        }
        catch ( RtMidiError &error )
        {
            error.printMessage();
            return false;
        }
    }
    if ( m_pMidiIn != NULL )
    {
        try
        {
            m_pMidiIn->openPort( id );
        }
        catch ( RtMidiError &error )
        {
            error.printMessage();
            return false;
        }
    }
    return true;
}

bool MIDIDriver::OpenMIDIOutPort( int id )
{
    if ( m_pMidiOut != NULL ) // close any open port
    {
        delete m_pMidiOut;
        m_pMidiOut = NULL;
    }
    if ( m_pMidiOut == NULL )
    {
        try
        {
            m_pMidiOut = new RtMidiOut( RtMidi::UNSPECIFIED, m_strClientName );
        }
        catch ( RtMidiError &error )
        {
            error.printMessage();
            return false;
        }
    }
    if ( m_pMidiOut != NULL )
    {
        try
        {
            m_pMidiOut->openPort( id );
        }
        catch ( RtMidiError &error )
        {
            error.printMessage();
            return false;
        }
    }
    buffer_size = DEFAULT_BUFFER_SIZE;
    buffer = new unsigned char[DEFAULT_BUFFER_SIZE];

    return true;
}

void MIDIDriver::CloseMIDIInPort()
{
    if ( m_pMidiIn != NULL )
    {
        m_pMidiIn->closePort();
        delete m_pMidiIn;
        m_pMidiIn = NULL;
    }
}

void MIDIDriver::CloseMIDIOutPort()
{
    if ( m_pMidiOut != NULL )
    {
        m_pMidiOut->closePort();
        delete m_pMidiOut;
        m_pMidiOut = NULL;
        delete buffer;
        buffer_size = 0;
    }
}

bool MIDIDriver::HardwareMsgOut( const MIDITimedBigMessage &msg )
{
    if ( m_pMidiOut != NULL )
    {
        try
        {
            if ( msg.IsChannelEvent() )
            {
                buffer[0] = msg.GetStatus();
                buffer[1] = msg.GetByte1();
                buffer[2] = msg.GetByte2();
                std::vector<unsigned char> vec( buffer, buffer + 3 );
                m_pMidiOut->sendMessage( &vec );
            }

            else if ( msg.IsSystemExclusive() )
            {
                if ( msg.GetSysEx()->GetLength() + 1 > buffer_size )
                {
                    buffer_size = msg.GetSysEx()->GetLength() + 1;
                    delete buffer;
                    buffer = new unsigned char[buffer_size];
                }
                buffer[0] = msg.GetStatus();
                memcpy( buffer + 1, msg.GetSysEx()->GetBuf(), msg.GetSysEx()->GetLength() );
                std::vector<unsigned char> vec( buffer, buffer + buffer_size );
                m_pMidiOut->sendMessage( &vec );

                // std::cout << "Driver sent sysex message: lenght " << msg.GetSysEx()->GetLength() << "\n";
            }

            else
            {
                // char s[100];
                // std::cout << "Driver skipped message " << msg.MsgToText(s) << std::endl;
            }
            return true;
        }
        catch ( RtMidiError &error )
        {
            error.printMessage();
            return false;
        }
    }

    return false;
}
// END OF EDITING BY NC

void MIDIDriver::TimeTick( unsigned long sys_time )
{
    // run the additional tick procedure if we need to
    if ( tick_proc )
    {
        tick_proc->TimeTick( sys_time );
    }

    // feed as many midi messages from out_queu to the hardware out port
    // as we can

    while ( out_queue.CanGet() )
    {
        // use the Peek() function to avoid allocating memory for
        // a duplicate sysex
        if ( HardwareMsgOut( *( out_queue.Peek() ) ) == true )
        {
            // ok, got and sent a message - update our out_queue now
            out_queue.Next();
        }

        else
        {
            // cant send any more, stop now.
            break;
        }
    }
}

unsigned int MIDIDriver::GetNumMIDIInDevs()
{
    RtMidiIn midiin;
    return midiin.getPortCount();
}

unsigned int MIDIDriver::GetNumMIDIOutDevs()
{
    RtMidiOut midiout;
    return midiout.getPortCount();
}

std::string MIDIDriver::GetMIDIInDevName( unsigned int id )
{
    RtMidiIn midiin;
    return midiin.getPortName( id );
}

std::string MIDIDriver::GetMIDIOutDevName( unsigned int id )
{
    RtMidiOut midiout;
    return midiout.getPortName( id );
}

unsigned long MIDIDriver::GetSystemTime()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::milliseconds dur = std::chrono::duration_cast<std::chrono::milliseconds>( now - session_start );
    unsigned long ret = dur.count();
    return ret;
}
}
