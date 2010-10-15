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

#ifdef WIN32
#include <windows.h>
#endif

#include "jdksmidi/world.h"
#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/sequencer.h"

using namespace jdksmidi;


void DumpMIDIBigMessage ( MIDITimedBigMessage *msg )
{
    if ( msg )
    {
        char msgbuf[1024];
        fprintf ( stdout, "%s\n", msg->MsgToText ( msgbuf ) );

        if ( msg->IsSysEx() )
        {
            fprintf ( stdout, "\tSYSEX length: %d\n", msg->GetSysEx()->GetLength() );
        }
    }
}

void DumpMIDITimedBigMessage ( MIDITimedBigMessage *msg )
{
    if ( msg )
    {
        char msgbuf[1024];
        fprintf ( stdout, "%8ld : %s\n", msg->GetTime(), msg->MsgToText ( msgbuf ) );

        if ( msg->IsSysEx() )
        {
            fprintf ( stdout, "\tSYSEX length: %d\n", msg->GetSysEx()->GetLength() );
        }
    }
}

void DumpMIDITrack ( MIDITrack *t )
{
    MIDITimedBigMessage *msg;

    for ( int i = 0; i < t->GetNumEvents(); ++i )
    {
        msg = t->GetEventAddress ( i );
        DumpMIDITimedBigMessage ( msg );
    }
}

void DumpAllTracks ( MIDIMultiTrack *mlt )
{
    fprintf ( stdout , "Clocks per beat: %d\n\n", mlt->GetClksPerBeat() );

    for ( int i = 0; i < mlt->GetNumTracks(); ++i )
    {
        if ( mlt->GetTrack ( i )->GetNumEvents() > 0 )
        {
            fprintf ( stdout, "DUMP OF TRACK #%2d:\n", i );
            DumpMIDITrack ( mlt->GetTrack ( i ) );
            fprintf ( stdout, "\n" );
        }
    }
}

void DumpMIDIMultiTrack ( MIDIMultiTrack *mlt )
{
    MIDIMultiTrackIterator i ( mlt );
    MIDITimedBigMessage *msg;
    fprintf ( stdout , "Clocks per beat: %d\n\n", mlt->GetClksPerBeat() );
    i.GoToTime ( 0 );

    do
    {
        int trk_num;

        if ( i.GetCurEvent ( &trk_num, &msg ) )
        {
            fprintf ( stdout, "#%2d - ", trk_num );
            DumpMIDITimedBigMessage ( msg );
        }
    }
    while ( i.GoToNextEvent() );
}

void PlayDumpSequencer ( MIDISequencer *seq )
{
    float pretend_clock_time = 0.0;
    float next_event_time = 0.0;
    MIDITimedBigMessage ev;
    int ev_track;
    seq->GoToTimeMs ( pretend_clock_time );

    if ( !seq->GetNextEventTimeMs ( &next_event_time ) )
    {
        return;
    }

    // simulate a clock going forward with 10 ms resolution for 1 hour
    float max_time = 3600. * 1000.; // VRM@
    for ( ; pretend_clock_time < max_time; pretend_clock_time += 10. )
    {
        // find all events that came before or a the current time
        while ( next_event_time <= pretend_clock_time )
        {
            if ( seq->GetNextEvent ( &ev_track, &ev ) )
            {
                // found the event!
                // show it to stdout
                fprintf ( stdout, "tm=%06.0f : evtm=%06.0f :trk%02d : ",
                          pretend_clock_time, next_event_time, ev_track );
                DumpMIDITimedBigMessage ( &ev );
                // now find the next message

                if ( !seq->GetNextEventTimeMs ( &next_event_time ) )
                {
                    // no events left so end
                    fprintf ( stdout, "End\n" );
                    return;
                }
            }
        }
    }
}

// TO DO: fix problems (see below) and add this function to class MIDISequencer
// recommended values for  time_precision_sec >= 0.001  and for  max_duration_hours = 1 ... 24
double GetMisicDurationInSeconds(MIDISequencer &seq, float time_precision_sec = 0.1f, int max_duration_hours = 2) // func by VRM@
{
  double dur = 0.;
  double clock_time;
  float  next_event_time; // in milliseconds
  const double tp_msec = 1000. * time_precision_sec;

  MIDITimedBigMessage ev;
  int ev_track;

  seq.GoToTimeMs ( 0.f );
  if ( !seq.GetNextEventTimeMs ( &next_event_time ) ) return dur;

  // simulate a clock going forward with tp_msec resolution for hours
  const double hours = max_duration_hours * 3600. * 1000.;
  for ( clock_time = next_event_time = 0.f; clock_time < hours; clock_time += tp_msec )
  {
    // find all events that came before or a the current time
    while ( double(next_event_time) <= clock_time )
    {
      if ( seq.GetNextEvent( &ev_track, &ev ) )
      {
        if ( !seq.GetNextEventTimeMs( &next_event_time ) ) // no events left so end
        {
          dur = 0.001f * clock_time;
          return dur;
        }
      }
    }
  }
  dur = 0.001f * clock_time;
  return dur;
}


int main ( int argc, char **argv )
{
    if ( argc > 1 )
    {
        MIDIFileReadStreamFile rs ( argv[1] );
        MIDIMultiTrack tracks ( 64 );
        MIDIFileReadMultiTrack track_loader ( &tracks );
        MIDIFileRead reader ( &rs, &track_loader );
//    MIDISequencerGUIEventNotifierText notifier( stdout );
//    MIDISequencer seq( &tracks, &notifier );
        MIDISequencer seq ( &tracks );

/*
  TO DO:
  some midi files (for ex. bwv599.mid and bwv604.mid) with parameter (optimize_tracks = 1)
  generate bad PlayDumpSequencer() and GetMisicDurationInSeconds() results!
  may be bug contain in *.mid or in MIDISequencer::GetNextEvent() and GetNextEventTimeMs()
*/
        int optimize_tracks = 0;
        if ( argc > 2 ) optimize_tracks = atoi ( argv[2] );
        reader.SetOptimizeTracks( optimize_tracks != 0 ); // VRM@
        reader.Parse();

        // DumpMIDIMultiTrack( &tracks );
        // note that Sequencer generate "META-EVENT 7e,00" (META_BEAT_MARKER) in files dump,
        // but files themselves not contain this meta event!
        if ( argc > 3 ) PlayDumpSequencer ( &seq );

        float time_precision_sec = 0.01f;
        double dt = GetMisicDurationInSeconds( seq, time_precision_sec );
        fprintf ( stdout, "\nMisic duration = %f seconds +- %f\n", dt, time_precision_sec );
    }
    else
      fprintf ( stderr, "usage:\n\tjdkmidi_test_sequencer FILE.mid [optimize_tracks(0,1) [any arg for Dump]]\n" );

    return 0;
}

