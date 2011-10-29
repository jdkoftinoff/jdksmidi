/*

  libjdksmidi C++ Class Library for MIDI addendum

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program;
  if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com
// vrm@vmgames.com
//

#include "jdksmidi/world.h"
#include "jdksmidi/utils.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/filewritemultitrack.h"

namespace jdksmidi
{

void SoloMelodyConverter( const MIDIMultiTrack &src, MIDIMultiTrack &dst, int ignore_channel )
{
    // this simple code works better for src MultiTrack with 1 track,
    // if not, we can make before the call of CollapseMultiTrack()

    dst.ClearAndResize( src.GetNumTracks() );
    dst.SetClksPerBeat( src.GetClksPerBeat() );

    MIDIClockTime ev_time = 0;
    MIDISequencer seq( &src );
    seq.GoToTime( 0 );
    if ( !seq.GetNextEventTime ( &ev_time ) )
        return; // empty src multitrack

    MIDITimedBigMessage ev;
    int ev_track;

    int solo_note = -1; // highest midi note number in current time, valid values 0...127
    bool solo_note_on = false;
    MIDITimedBigMessage solo_note_on_ev; // last solo note on event
    solo_note_on_ev.SetNoOp();

    while ( seq.GetNextEvent( &ev_track, &ev ) )
    {
        if ( ev.IsServiceMsg() || ev.IsNoOp() )
            continue;

        if ( ev.IsChannelEvent() )
        {
            if ( ev.GetChannel() == ignore_channel )
                continue;

//          if ( ev.IsAllNotesOff() ) ... ; // for future work...

            if ( ev.IsNote() )
            {
                int new_note = ev.GetNote();

                // skip all note events if new note lower than solo note
                if ( new_note < solo_note )
                    continue;
                // else ( new_note >= solo_note )

                if ( ev.ImplicitIsNoteOn() ) // new note on event
                {
                    if ( solo_note_on ) // new note on after previous solo note on
                    {
                        // make noteoff message for previous solo note
                        solo_note_on_ev.SetTime( ev.GetTime() );
                        solo_note_on_ev.SetVelocity( 0 ); // note off
                        dst.GetTrack(ev_track)->PutEvent( solo_note_on_ev );

                        // make new solo note
                        solo_note_on_ev = ev;
                        solo_note = new_note;
                    }
                    else // ( solo_note_on == false ) - new note on after previous silence
                    {
                        // make new solo note
                        solo_note_on = true;
                        solo_note_on_ev = ev;
                        solo_note = new_note;
                    }
                }
                else // new note off event ( new_note >= solo_note )
                {
                    if ( solo_note_on ) // new note off after previous solo note on
                    {
                        if ( new_note == solo_note ) // solo note off event
                        {
                            // test channels of the events
                            if ( ev.GetChannel() == solo_note_on_ev.GetChannel() )
                            {
                                solo_note_on = false;
                                solo_note = -1; // erase solo_note
                            }
                            else
                                continue; // skip other note off stuff
                        }
                        else // ( new_note > solo_note ) any other note off event
                            continue; // skip other note off stuff
                    }
                    else // ( solo_note_on == false ) - new note off after previous silence
                        continue; // skip other note off stuff
                }
            }
        }
        dst.GetTrack(ev_track)->PutEvent(ev);
    }
}

void CopyWithoutChannel( const MIDIMultiTrack &src, MIDIMultiTrack &dst, int ignore_channel )
{
    dst.ClearAndResize( src.GetNumTracks() );
    dst.SetClksPerBeat( src.GetClksPerBeat() );

    MIDIClockTime ev_time = 0;
    MIDISequencer seq( &src );
    seq.GoToTime( 0 );
    if ( !seq.GetNextEventTime ( &ev_time ) )
        return; // empty src multitrack

    MIDITimedBigMessage ev;
    int ev_track;

    while ( seq.GetNextEvent( &ev_track, &ev ) )
    {
        if ( ev.IsServiceMsg() || ev.IsNoOp() )
            continue;

        if ( ev.IsChannelEvent() && ev.GetChannel() == ignore_channel )
            continue;

        dst.GetTrack(ev_track)->PutEvent(ev);
    }
}

void CompressStartPause( const MIDIMultiTrack &src, MIDIMultiTrack &dst, int ignore_channel )
{
    dst.ClearAndResize( src.GetNumTracks() );
    dst.SetClksPerBeat( src.GetClksPerBeat() );

    MIDIClockTime ev_time = 0;
    MIDISequencer seq( &src );
    seq.GoToTime( 0 );
    if ( !seq.GetNextEventTime ( &ev_time ) )
        return; // empty src multitrack

    MIDITimedBigMessage ev;
    int ev_track;
    bool compress = true;
    MIDIClockTime old_ev_time = 0, delta_ev_time = 0, ev_time0 = 0;

    while ( seq.GetNextEvent( &ev_track, &ev ) )
    {
        if ( ev.IsServiceMsg() || ev.IsNoOp() )
            continue;

        if ( ev.IsChannelEvent() && ev.GetChannel() == ignore_channel )
            continue;

        ev_time = ev.GetTime();
        if ( compress )
        {
            // compress time intervals between adjacent messages to 1 tick
            if (ev_time > old_ev_time)
                ++delta_ev_time;

            old_ev_time = ev_time;

            ev.SetTime( delta_ev_time );

            if ( ev.ImplicitIsNoteOn() )
            {
                compress = false;
                ev_time0 = ev_time - delta_ev_time;
            }
        }
        else
        {
            ev.SetTime( ev_time - ev_time0 );
        }

        dst.GetTrack(ev_track)->PutEvent(ev);
    }
}

void ClipMultiTrack( const MIDIMultiTrack &src, MIDIMultiTrack &dst, double max_time_sec )
{
    dst.ClearAndResize( src.GetNumTracks() );
    dst.SetClksPerBeat( src.GetClksPerBeat() );

    double max_event_time = 1000.*max_time_sec; // msec
    double event_time = 0.; // msec

    MIDISequencer seq( &src );
    seq.GoToTimeMs( 0.f );
    if ( !seq.GetNextEventTimeMs ( &event_time ) )
        return; // empty src multitrack

    MIDITimedBigMessage ev;
    int ev_track;
    while ( seq.GetNextEvent( &ev_track, &ev ) )
    {
        // ignore NoOp, BeatMarker and other Service messages
        if ( ev.IsServiceMsg() || ev.IsNoOp() )
            continue;

        dst.GetTrack(ev_track)->PutEvent(ev);

        if ( event_time >= max_event_time )
            break; // end of max_time_sec

        if ( !seq.GetNextEventTimeMs( &event_time ) )
            break; // end of src multitrack
    }
}

void CollapseMultiTrack( const MIDIMultiTrack &src, MIDIMultiTrack &dst )
{
    dst.ClearAndResize( 1 );
    dst.SetClksPerBeat( src.GetClksPerBeat() );

    MIDISequencer seq( &src );
    seq.GoToZero();

    MIDITimedBigMessage ev;
    int ev_track;
    while ( seq.GetNextEvent( &ev_track, &ev ) )
    {
        // ignore all src EndOfTrack messages!!
        if ( ev.IsDataEnd() )
            continue;

        // ignore NoOp, BeatMarker and other Service messages
        if ( ev.IsServiceMsg() || ev.IsNoOp() )
            continue;

        dst.GetTrack(0)->PutEvent(ev);
    }

    // set (single!) dst EndOfTrack message
    MIDITimedBigMessage end(ev); // copy time of last src event
    end.SetDataEnd();
    dst.GetTrack(0)->PutEvent(end);
}

void CollapseAndExpandMultiTrack( const MIDIMultiTrack &src, MIDIMultiTrack &dst )
{
    CollapseMultiTrack(src, dst);
    dst.AssignEventsToTracks(0);
}

bool ReadMidiFile(const char *file, MIDIMultiTrack &dst)
{
    MIDIFileReadStreamFile rs( file );
    MIDIFileReadMultiTrack track_loader( &dst );
    MIDIFileRead reader( &rs, &track_loader );
    // set amount of dst tracks equal to midifile
    dst.ClearAndResize( reader.ReadNumTracks() );
    // load the midifile into the multitrack object
    return reader.Parse();
}

bool WriteMidiFile(const MIDIMultiTrack &src, const char *file, bool use_running_status)
{
    MIDIFileWriteStreamFileName out_stream( file );
    if ( !out_stream.IsValid() )
        return false;

    MIDIFileWriteMultiTrack writer( &src, &out_stream );

    // write midifile with or without running status usage
    writer.UseRunningStatus( use_running_status );

    int tracks_number = src.GetNumTracksWithEvents();
    return writer.Write( tracks_number );
}

double GetMisicDurationInSeconds(const MIDIMultiTrack &mt)
{
    MIDISequencer seq( &mt );
    return seq.GetMisicDurationInSeconds();
}

std::string MultiTrackAsText(const MIDIMultiTrack &mt)
{
    MIDISequencer seq( &mt );
    seq.GoToZero();

    int track;
    MIDITimedBigMessage ev;

    std::ostringstream ostr;
    ostr << "Clocks per beat  "  << mt.GetClksPerBeat() << std::endl << std::endl;
    while ( seq.GetNextEvent( &track, &ev ) )
    {
        if ( ev.IsBeatMarker() ) continue;

        MIDIClockTime midi_time = seq.GetCurrentMIDIClockTime();
        double msec_time = seq.GetCurrentTimeInMs();

        char buf[256];
        ev.MsgToText( buf );

        ostr << "Track " << track;
        ostr << "  Midi tick " << midi_time;
        ostr << "  Time msec " << msec_time;
        ostr << "  MSG " << buf << std::endl;
    }

    ostr << std::endl;
    return ostr.str();
}

std::string EventAsText(const MIDITimedBigMessage &ev)
{
    char buf[256];
    ev.MsgToText( buf );
    MIDIClockTime time = ev.GetTime();

    std::ostringstream ostr;
    ostr << " Midi tick " << time;
    ostr << "  MSG " << buf << " ";
    return ostr.str();
}

void LastEventsProlongation( MIDIMultiTrack &tracks, int track_num, MIDIClockTime add_ticks )
{
    MIDITrack *track = tracks.GetTrack( track_num );
    int index = track->GetNumEvents() - 1;
    if ( add_ticks == 0 || index < 0 )
        return;

    MIDITimedBigMessage *msg = track->GetEvent( index );
    MIDIClockTime tmax = msg->GetTime();

    while ( msg->GetTime() == tmax )
    {
        msg->SetTime( tmax + add_ticks );
        if ( --index < 0 )
            break;
        msg = track->GetEvent( index );
    }
}

bool AddEndingPause( MIDIMultiTrack &tracks, int track_num, MIDIClockTime pause_ticks )
{
    MIDIClockTime t = tracks.GetTrack( track_num )->GetLastEventTime();
    MIDITimedBigMessage msg;
    msg.SetTime( t + pause_ticks );
    // add lowest "note on" in channel 0 with velocity 0 (i.e. "note off")
    msg.SetNoteOn( 0, 0, 0 );
    return tracks.GetTrack( track_num )->PutEvent( msg );
}


}

