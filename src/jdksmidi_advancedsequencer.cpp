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
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

//
// MODIFIED by N. Cassetta
//



#include "jdksmidi/world.h"
#include "jdksmidi/advancedsequencer.h"

/* NOTE BY NC: ISSUE: if you include "midi.h" before "world.h" the compiler gives errors. This is because
 * this #include chain: midi.h -> world.h -> utils.h -> multitrack.h -> track.h -> msg.h -> midi.h
 * I suggest to separate MIDIMultiTrack declarations in utils.h from general utility functions
 */

namespace jdksmidi
{

static void FixQuotes ( char *s_ )
{
    unsigned char *s = ( unsigned char * ) s_;

    while ( *s )
    {
        if ( *s == 0xd2 || *s == 0xd3 )
        {
            *s = '"';
        }

        else if ( *s == 0xd5 )
        {
            *s = '\'';
        }

        else if ( *s >= 0x80 )
        {
            *s = ' ';
        }

        s++;
    }
}


/* OLD
AdvancedSequencer::AdvancedSequencer()
    :
    thru_processor ( 2 ),
    thru_transposer(),
    thru_rechannelizer(),
    driver ( 256, stdout ),
    tracks ( 17 ),
    notifier ( stdout ),
    seq ( &tracks, &notifier ),
    mgr ( &driver, &notifier, &seq ),
    repeat_start_measure ( 0 ),
    repeat_end_measure ( 0 ),
    repeat_play_mode ( false ),
    num_warp_positions ( 0 ),
    file_loaded ( false ),
    chain_mode ( false )
{
}


AdvancedSequencer::~AdvancedSequencer()
{
    Stop();
    CloseMIDI();

    for ( int i = 0; i < num_warp_positions; ++i )
    {
        jdks_safe_delete_object ( warp_positions[i] );
    }
}
*/
/* NEW BY NC */

AdvancedSequencer::AdvancedSequencer(MIDISequencerGUIEventNotifier *n)
    :

#ifdef WIN32
    // driver( new MIDIDriverWin32( 256 ) ),  // OLD
    driver( new MIDIDriverWin32() ),  /* NEW BY NC: queue_size given as default parameter */
#else
    std:cout << "Currentky only supported WIN32\n";
    exit(EXIT_FAILURE);
#endif // WIN32

    notifier( n ),
    tracks ( 17 ),                      // TODO: allow more tracks!
    seq ( &tracks, notifier ),
    mgr ( driver, notifier, &seq ),

    thru_processor ( 2 ),
    thru_transposer(),
    thru_rechannelizer(),

    num_measures ( 0 ),
    repeat_start_measure ( 0 ),
    repeat_end_measure ( 0 ),
    repeat_play_mode ( false ),
    file_loaded ( false ),
#ifdef WIN32
    in_port ( MIDI_MAPPER ),
    out_port ( MIDI_MAPPER )
#else
    in_port ( 0 ),
    out_port ( 0 )
#endif // WIN32

    // chain_mode ( false ) OLD (see header)

{
/* NOTE BY NC: currently we open midi (and start timer) in the ctor and close it in the dtor:
 * is this right? perhaps we should open/close only in Play/Stop (but what if thru is enabled?)
 * What is better?
 */
    OpenMIDI(in_port, out_port);
}


AdvancedSequencer::~AdvancedSequencer()
{
    Stop();
    CloseMIDI();
    delete driver;
}


void AdvancedSequencer::SetOutputPort( int p)
{
    Stop();
    CloseMIDI();
    out_port = p;
    OpenMIDI(in_port, out_port);
}

int AdvancedSequencer::GetOutputPort() const
{
    return out_port;
}


void AdvancedSequencer::SetInputPort( int p)
{
    Stop();
    CloseMIDI();
    in_port = p;
    OpenMIDI(in_port, out_port);
}

int AdvancedSequencer::GetInputPort() const
{
    return in_port;
}



void AdvancedSequencer::SetMIDIThruEnable ( bool f )
{
    driver->SetThruEnable ( f );
}

bool AdvancedSequencer::GetMIDIThruEnable() const
{
    return driver->GetThruEnable();
}

void AdvancedSequencer::SetMIDIThruChannel ( int chan )
{
    thru_rechannelizer.SetAllRechan ( chan );
    driver->AllNotesOff();
}


int AdvancedSequencer::GetMIDIThruChannel() const
{
    return thru_rechannelizer.GetRechanMap ( 0 );
}



void AdvancedSequencer::SetMIDIThruTranspose ( int val )
{
    thru_transposer.SetAllTranspose ( val );
    driver->AllNotesOff();
}


int AdvancedSequencer::GetMIDIThruTranspose() const
{
    return thru_transposer.GetTransposeChannel ( 0 );
}



bool AdvancedSequencer::Load ( const char *fname )
{
    char realname[1024];
    strcpy ( realname, fname );
    int orignamelen = ( int ) strlen ( fname );
    // chain_mode = false; OLD (see header)

    if ( orignamelen > 0 )
    {
        if ( realname[orignamelen-1] == '+' )
        {
            realname[orignamelen-1] = 0;
            // chain_mode = true; OLD (see header)
        }
    }

    MIDIFileReadStreamFile mfreader_stream ( realname );
    MIDIFileReadMultiTrack track_loader ( &tracks );
    MIDIFileRead reader ( &mfreader_stream, &track_loader );
    Stop();
    driver->AllNotesOff();
    tracks.Clear();
    seq.ResetAllTracks();

    if ( reader.Parse() )
    {
        file_loaded = true;
        Reset();
        // GoToMeasure ( 0 ); OLD: it used warp_positions, not even initialized!!! However,
        // this is already done by Reset();
        ExtractWarpPositions();
    }

    else
    {
        file_loaded = false;
    }

    return file_loaded;
}


void AdvancedSequencer::UnLoad()    /* NEW BY NC */
{
    Reset();
    tracks.Clear();
    warp_positions.clear();
    num_measures = 0;
    file_loaded = false;
}


void AdvancedSequencer::Reset()
{
    Stop();
    // driver->AllNotesOff(); already done in Stop() !
    UnmuteAllTracks();
    UnSoloTrack();
    SetTempoScale ( 1.00 );
    SetRepeatPlay(false, 0, 0 ); /* ADDED BY NC */
    seq.ResetAllTracks();
    seq.GoToZero();
    driver->Reset();    // needed? I don't know
}

/* NEW BY NC */

MIDIMultiTrack& AdvancedSequencer::GetMultiTrack()
{
    return tracks;
}

const MIDIMultiTrack& AdvancedSequencer::GetMultiTrack() const
{
    return tracks;
}

MIDIMultiTrack* AdvancedSequencer::GetMultiTrackAddress()
{
    return &tracks;
}

const MIDIMultiTrack* AdvancedSequencer::GetMUltiTrackAddress() const
{
    return &tracks;
}

void AdvancedSequencer::GoToZero() {
    Stop();
    seq.GoToMeasure(0); // NC: not GoToZero() ! this sets correct values in the MIDISequencerState and notify them
}
/* END OF NEW */

/* NC TODO: these throws some warnings: revise it */
void AdvancedSequencer::GoToTime (MIDIClockTime t) {
    if ( !file_loaded )
    {
        return;
    }

    // figure out which warp item we use
    // try warp to the last warp point BEFORE the
    // requested measure

    unsigned int warp_to_item = 0;
    for ( ; warp_to_item < warp_positions.size(); warp_to_item++ )
    {
        if ( warp_positions[warp_to_item].cur_clock > t )
        {
            break;
        }
    }
    if ( warp_to_item == warp_positions.size() && warp_to_item != 0 )
    {
        warp_to_item--;
    }

    if (mgr.IsSeqPlay())
    {
        Stop();
        seq.SetState (&warp_positions[warp_to_item]);
        seq.GoToTime (t);
        Play();
    }
    else
    {
        seq.SetState (&warp_positions[warp_to_item]);
        seq.GoToTime (t);
        for (int i = 0; i < seq.GetNumTracks(); ++i)
        {
            seq.GetTrackState ( i )->note_matrix.Clear();
        }
    }
}

/* OLD
void AdvancedSequencer::GoToTime ( MIDIClockTime t )
{
    if ( mgr.IsSeqPlay() )
    {
        Stop();
        seq.GoToTime ( t + 1 );
        Play();
    }

    else
    {
        seq.GoToTime ( t + 1 );
    }
}
*/

void AdvancedSequencer::GoToMeasure ( int measure, int beat )
{
    if ( !file_loaded )
    {
        return;
    }

    // figure out which warp item we use
    // try warp to the last warp point BEFORE the
    // requested measure
    int warp_to_item = ( measure - 1 ) / MEASURES_PER_WARP;

    if ( warp_to_item >= warp_positions.size() )
        warp_to_item = warp_positions.size() - 1;

    if ( warp_to_item < 0 )
        warp_to_item = 0;

    if ( mgr.IsSeqPlay() )
    {
        Stop();

/*
        if ( warp_positions[warp_to_item] )
        {
            seq.SetState ( warp_positions[warp_to_item] );
        }
*/      seq.SetState ( &warp_positions[warp_to_item] );
        seq.GoToMeasure ( measure, beat );
        Play();
    }

    else
    {
/*
        if ( warp_positions[warp_to_item] )
        {
            seq.SetState ( warp_positions[warp_to_item] );
        }
*/
        seq.SetState ( &warp_positions[warp_to_item] );
        seq.GoToMeasure ( measure, beat );

        for ( int i = 0; i < seq.GetNumTracks(); ++i )
        {
            seq.GetTrackState ( i )->note_matrix.Clear();
        }
    }
}


void AdvancedSequencer::Play ( /* int clock_offset OLD */ )
{
    if ( !file_loaded )
    {
        return;
    }

    Stop();

/* this is already done by Stop
    for ( int i = 0; i < seq.GetNumTracks(); ++i )
    {
        seq.GetTrackState ( i )->note_matrix.Clear();
    }
*/
    if ( repeat_play_mode )
    {
        // seq.GoToMeasure ( repeat_start_measure ); OLD
        GoToMeasure ( repeat_start_measure );
    }

/* OLD
    MIDIClockTime cur_time = seq.GetCurrentMIDIClockTime();

    if ( ( long ) cur_time > -clock_offset )
        cur_time += clock_offset;

    seq.GoToTime ( cur_time );
*/

    CatchEventsBefore();
    // this intercepts any CC, SYSEX and TEMPO messages and send them to the out port
    // allowing to start with correct values; we could incorporate this in the
    // sequencer state, but it would track even CC (not difficult) and SYSEX messages

    mgr.SetSeqOffset ( ( unsigned long ) seq.GetCurrentTimeInMs() );
    // mgr.SetTimeOffset ( timeGetTime() ); OLD
    mgr.SetTimeOffset ( jdks_get_system_time_ms() ); /* NC: eliminated every WIN32 call not in a #ifdef WIN32 contest */
    mgr.SeqPlay();
}

/*
void AdvancedSequencer::Pause()
{
    if ( !file_loaded )
    {
        return;
    }

    Stop();
}
*/


void AdvancedSequencer::Stop()
{
    if ( !file_loaded )
    {
        return;
    }

    if ( !mgr.IsSeqStop() )
    {
        mgr.SeqStop();
        driver->AllNotesOff();
        GoToMeasure(seq.GetState()->cur_measure, seq.GetState()->cur_beat);
        // stops on a beat (and clear midi matrix)
    }
}


/* NEW BY NC */
void AdvancedSequencer::OutputMessage( MIDITimedBigMessage& msg ) {
    for (int i = 1; i < 1000; i++)  // retry for 1 second
    {
        if (driver->CanOutputMessage()) {
            driver->OutputMessage( msg );
            return;
        }
        jdks_wait( 1 ); /* note by NC: this may not be accurate, however waits for a minimum period */
    }
    std::cerr << "OutputMessage failed!" << std::endl;
}


void AdvancedSequencer::SetRepeatPlay ( bool enable, int start_measure, int end_measure )
{
    if ( !file_loaded )
    {
        return;
    }

    if ( start_measure < end_measure && start_measure >= 0 )
    {
        repeat_play_mode = enable;
        repeat_start_measure = start_measure;
        repeat_end_measure = end_measure;
    }

    else
    {
        repeat_play_mode = false;
    }

    mgr.SetRepeatPlay (
        repeat_play_mode,
        repeat_start_measure,
        repeat_end_measure
    );
    /* NEW */
    if (IsPlay())
    {
        Play();     // restarts from start_measure
    }
}

/* NOTE BY NC: soloing and muting has been enhanced to keep count of muted CC, SYSEX changes previously muted
 * so when we unmute a track it sounds with correct parameters even if they weren't tramsmitted before
 * So member function are changed
 */


/* OLD
void AdvancedSequencer::SoloTrack ( int trk )
{
    if ( !file_loaded )
    {
        return;
    }

    if ( trk == -1 )
    {
        seq.SetSoloMode ( false );
        driver->AllNotesOff();

        for ( int i = 0; i < seq.GetNumTracks(); ++i )
        {
            seq.GetTrackState ( i )->note_matrix.Clear();
        }
    }

    else
    {
        seq.SetSoloMode ( true, trk );
        driver->AllNotesOff();

        for ( int i = 0; i < seq.GetNumTracks(); ++i )
        {
            seq.GetTrackState ( i )->note_matrix.Clear();
        }
    }
}
*/
/* NEW BY NC */
void AdvancedSequencer::SoloTrack ( int trk )
{
    if ( !file_loaded )
    {
        return;
    } // unsoloing done by UnSoloTrack()
    if (IsPlay())
    {
        CatchEventsBefore(trk); // track could be muted before soloing: this set appropriate CC, PC, etc not previously sent
    }
    seq.SetSoloMode (true, trk);
    for (int i = 0; i < seq.GetNumTracks(); ++i)
    {
        if (i == trk) continue;
        driver->AllNotesOff(FindFirstChannelOnTrack(i) - 1);
        seq.GetTrackState (i)->note_matrix.Clear();
    }
}

/* OLD
void AdvancedSequencer::UnSoloTrack()
{
    if ( !file_loaded )
    {
        return;
    }

    seq.SetSoloMode ( false );
    driver->AllNotesOff();

    for ( int i = 0; i < seq.GetNumTracks(); ++i )
    {
        seq.GetTrackState ( i )->note_matrix.Clear();
    }
} */

/* NEW */
void AdvancedSequencer::UnSoloTrack()  {
    if ( !file_loaded )
    {
        return;
    }
    if (IsPlay())
    {
        CatchEventsBefore(); // this set appropriate CC, PC, etc for previously muted tracks
    }
    seq.SetSoloMode (false);
}

bool AdvancedSequencer::GetTrackSolo( int trk )
{
    return seq.GetTrackProcessor (trk)->solo;
}


/* OLD
void AdvancedSequencer::SetTrackMute ( int trk, bool f )
{
    if ( !file_loaded )
    {
        return;
    }

    seq.GetTrackProcessor ( trk )->mute = f;
    driver->AllNotesOff();
} */
/* NEW */

void AdvancedSequencer::SetTrackMute ( int trk, bool f )
{
    if ( !file_loaded )
    {
        return;
    }
    seq.GetTrackProcessor ( trk )->mute = f;
    if ( IsPlay() )
    {
        if ( f )
        {
            driver->AllNotesOff( FindFirstChannelOnTrack(trk) - 1 );  // TODO: tieni conto del rechannelize
        }
        else
        {
           CatchEventsBefore( trk );    // track was muted: this set appropriate CC, PC, etc not previously sent
        }
    }
}

bool AdvancedSequencer::GetTrackMute ( int trk )
{
    return seq.GetTrackProcessor (trk)->mute;
}

/* OLD
void AdvancedSequencer::UnmuteAllTracks()
{
    if ( !file_loaded )
    {
        return;
    }

    for ( int i = 0; i < seq.GetNumTracks(); ++i )
    {
        if ( seq.GetTrackProcessor ( i )->mute )
        {
            seq.GetTrackState ( i )->note_matrix.Clear();
            seq.GetTrackProcessor ( i )->mute = false;
        }
    }

    driver->AllNotesOff();
} */

/* NEW */
void AdvancedSequencer::UnmuteAllTracks()
{
    if ( !file_loaded )
    {
        return;
    }
    for ( int i = 0; i < seq.GetNumTracks(); ++i)
    {
        if ( seq.GetTrackProcessor (i)->mute)
        {
            seq.GetTrackState ( i )->note_matrix.Clear();
            seq.GetTrackProcessor ( i )->mute = false;
        }
    }
    driver->AllNotesOff();
    if (IsPlay())
    {
        CatchEventsBefore(); // this set appropriate CC, PC, etc for previously muted tracks
    }
}



void AdvancedSequencer::SetTempoScale ( double scale )
{
    if ( !file_loaded )
    {
        return;
    }

    seq.SetCurrentTempoScale ( static_cast<float> ( scale ) );
}


double AdvancedSequencer::GetTempoWithoutScale() const
{
    return seq.GetCurrentTempo();
}


double AdvancedSequencer::GetTempoWithScale() const
{
    return seq.GetCurrentTempo() * seq.GetCurrentTempoScale();
}

unsigned long AdvancedSequencer::GetCurrentTimeInMs() const {
// NEW: this is effective also during playback
    if ( mgr.IsSeqPlay() )
    {
        return jdks_get_system_time_ms() + mgr.GetSeqOffset() - mgr.GetTimeOffset();
        /* time from sequencer start
         * TODO: BY NC this should be a MIDIManager function (as stated also by Victor in TODO file)
         * perhaps we should put specialized jdks_get_system_time_ms for every OS in the utils.h file
         */
    }
    else
    {
       return seq.GetCurrentTimeInMs();
    }
}


int AdvancedSequencer::GetClksPerBeat() const
{
    return tracks.GetClksPerBeat();
}

int AdvancedSequencer::GetNumTracks() const
/* NOTE BY NC: actually this always returns 17, the number of tracks of the MIDIMultiTrack
 * TODO: modify the MIDIMultiTrack class to keep track of tracks actually used and implement
 * a function GetUsedTracks()
 */
{
    return seq.GetNumTracks();
}

int AdvancedSequencer::GetNumMeasures() const
{
    return num_measures;
}

int AdvancedSequencer::GetMeasure() const
{
    if ( !file_loaded )
    {
        return 0;
    }

    return seq.GetCurrentMeasure();
}


int AdvancedSequencer::GetBeat() const
{
    if ( !file_loaded )
    {
        return 0;
    }

    return seq.GetCurrentBeat();
}



int AdvancedSequencer::GetTimeSigNumerator() const
{
    if ( !file_loaded )
    {
        return 4;
    }

    // return seq.GetTrackState ( 0 )->timesig_numerator;       OLD: see MIDISequencer class changes
    return seq.GetState ()->timesig_numerator;          /* NC */
}


int AdvancedSequencer::GetTimeSigDenominator() const
{
    if ( !file_loaded )
    {
        return 4;
    }

    // return seq.GetTrackState ( 0 )->timesig_denominator;     OLD: see MIDISequencer class changes
    return seq.GetState ()->timesig_denominator;        /* NC */
}



int AdvancedSequencer::GetTrackNoteCount ( int trk ) const
{
    if ( !file_loaded )
    {
        return 0;
    }

    if ( mgr.IsSeqStop() )
    {
        return 0;
    }

    else
    {
        return seq.GetTrackState ( trk )->note_matrix.GetTotalCount();
    }
}


const char *AdvancedSequencer::GetTrackName ( int trk ) const
{
    if ( !file_loaded )
    {
        return "";
    }

    return seq.GetTrackState ( trk )->track_name;
}


int AdvancedSequencer::GetTrackVolume ( int trk ) const
{
    if ( !file_loaded )
    {
        return 100;
    }

    return seq.GetTrackState ( trk )->volume;
}


int AdvancedSequencer::GetTrackProgram ( int trk ) const
{
    if ( !file_loaded )
    {
        return 0;
    }

    return seq.GetTrackState ( trk )->pg;
}


void AdvancedSequencer::SetTrackVelocityScale ( int trk, int scale )
{
    if ( !file_loaded )
    {
        return;
    }

    seq.GetTrackProcessor ( trk )->velocity_scale = scale;
}


int AdvancedSequencer::GetTrackVelocityScale ( int trk ) const
{
    if ( !file_loaded )
    {
        return 100;
    }

    return seq.GetTrackProcessor ( trk )->velocity_scale;
}



void AdvancedSequencer::SetTrackRechannelize ( int trk, int chan )
{
    if ( !file_loaded )
    {
        return;
    }

    seq.GetTrackProcessor ( trk )->rechannel = chan;
    driver->AllNotesOff();
    seq.GetTrackState ( trk )->note_matrix.Clear();
}


int AdvancedSequencer::GetTrackRechannelize ( int trk ) const
{
    if ( !file_loaded )
    {
        return -1;
    }

    return seq.GetTrackProcessor ( trk )->rechannel;
}



void AdvancedSequencer::SetTrackTranspose ( int trk, int trans )
{
    if ( !file_loaded )
    {
        return;
    }

    bool was_playing = mgr.IsSeqPlay();

    if ( mgr.IsSeqPlay() )
    {
        was_playing = true;
        mgr.SeqStop();
    }

    if ( trk == -1 )
    {
        for ( trk = 0; trk < seq.GetNumTracks(); ++trk )
        {
            seq.GetTrackProcessor ( trk )->transpose = trans;
        }
    }

    else
    {
        seq.GetTrackProcessor ( trk )->transpose = trans;
    }

    if ( was_playing )
    {
        // driver->ResetMIDIOut(); // is this needed?
        driver->AllNotesOff();
        seq.GetTrackState ( trk )->note_matrix.Clear();
        mgr.SeqPlay();
    }
}


int AdvancedSequencer::GetTrackTranspose ( int trk ) const
{
    if ( !file_loaded )
    {
        return 0;
    }

    return seq.GetTrackProcessor ( trk )->transpose;
}



/* TODO: these must be revised  (see header comment)
void AdvancedSequencer::ExtractMarkers ( std::vector< std::string > *list )
{
    if ( !file_loaded )
    {
        list->clear();
        num_markers = 0;
        return;
    }

    MIDITrack *t = tracks.GetTrack ( 0 );
    list->clear();
    int cnt = 0;
    int measure = 0;
    int beat = 0;
    int timesig_numerator = 4;
    int timesig_denominator = 4;
    MIDIClockTime last_beat_time = 0;
    MIDIClockTime last_event_time = 0;
    int clks_per_beat = tracks.GetClksPerBeat();

    for ( int i = 0; i < t->GetNumEvents(); ++i )
    {
        MIDITimedBigMessage *m = t->GetEventAddress ( i );

        if ( m )
        {
            // how many beats have gone by since the last event?
            long beats_gone_by = ( m->GetTime() - last_beat_time ) / clks_per_beat;

            if ( beats_gone_by > 0 )
            {
                // calculate what our new measure/beat is
                beat += beats_gone_by;
                // carry over beat overflow to measure
                measure += beat / timesig_numerator;
                beat = beat % timesig_numerator;
                last_beat_time += ( clks_per_beat * beats_gone_by );
            }

            if ( m->IsMetaEvent() && m->IsTimeSig() )
            {
                timesig_numerator = m->GetTimeSigNumerator();
                timesig_denominator = m->GetTimeSigDenominator();
                clks_per_beat = tracks.GetClksPerBeat() * 4 / timesig_denominator;
            }

            if ( m->IsTextEvent() && m->GetSysEx() )
            {
                if ( ( m->GetMetaType() == META_GENERIC_TEXT )
                        || m->GetMetaType() == META_MARKER_TEXT
                        || m->GetMetaType() == META_CUE_POINT )
                {
                    char buf[256];
                    char line[256];
                    memcpy ( buf, m->GetSysEx()->GetBuf(), m->GetSysEx()->GetLengthSE() );
                    buf[ m->GetSysEx()->GetLengthSE() ] = '\0';
                    FixQuotes ( buf );
                    sprintf ( line, "%03d:%d        %s", measure + 1, beat + 1, buf );
                    list->push_back ( std::string ( line ) );
                    marker_times[ cnt++ ] = m->GetTime();
                }
            }

            last_event_time = m->GetTime();
        }
    }

    num_markers = cnt;
}


int AdvancedSequencer::GetCurrentMarker() const
{
    if ( !file_loaded )
    {
        return -1;
    }

    // find marker with largest time that
    // is before cur_time
    MIDIClockTime cur_time = seq.GetCurrentMIDIClockTime();
    cur_time += 20;
    int last = -1;

    for ( int i = 0; i < num_markers; ++i )
    {
        if ( marker_times[i] > cur_time )
        {
            break;
        }

        else
        {
            last = i;
        }
    }

    return last;
}
*/
/* NC the second substituted by this */

const char* AdvancedSequencer::GetCurrentMarker() const
{
    return seq.GetState()->marker_name;
}

void AdvancedSequencer::SetMltChanged()
{
    if ( IsPlay() )
    {
        Stop();     // however you should avoid to edit the MIDIMultiTrack during playback!
    }
    ExtractWarpPositions();
}



//
// protected members
//


bool AdvancedSequencer::OpenMIDI ( int in_port, int out_port, int timer_resolution )
{
 #ifdef WIN32
// TODO: (BY NC) this is a patch: it must be fixed with changes on class MIDIDriver;
    CloseMIDI();

    MIDIDriverWin32 *windriver = static_cast<MIDIDriverWin32*> ( driver );
    if ( !windriver->StartTimer ( timer_resolution ) )
    {
        return false;
    }

    if ( in_port != -1 )
    {
        windriver->OpenMIDIInPort ( in_port );
    }

    if ( windriver->OpenMIDIOutPort ( out_port ) )
    {
        return true;
    }

    else
    {
        return false;
    }
#endif // WIN32

}


void AdvancedSequencer::CloseMIDI()
{
#ifdef WIN32
// TODO: (BY NC) this is a patch: it must be fixed with changes on class MIDIDriver;
    MIDIDriverWin32 * windriver = static_cast<MIDIDriverWin32*> ( driver );
    Stop();
    windriver->StopTimer();
    windriver->AllNotesOff();
    Sleep ( 100 );
    windriver->CloseMIDIInPort();
    windriver->CloseMIDIOutPort();
#endif // WIN32
}


int AdvancedSequencer::FindFirstChannelOnTrack ( int trk )
{
    if ( !file_loaded )
    {
        return -1;
    }

    int first_channel = -1;
    MIDITrack *t = tracks.GetTrack ( trk );

    if ( t )
    {
        // go through all events
        // until we find a channel message
        // and then return the channel number plus 1
        for ( int i = 0; i < t->GetNumEvents(); ++i )
        {
            MIDITimedBigMessage *m = t->GetEventAddress ( i );

            if ( m )
            {
                if ( m->IsChannelMsg() )
                {
                    first_channel = m->GetChannel() + 1;
                    break;
                }
            }
        }
    }

    return first_channel;
}

/* OLD
void AdvancedSequencer::ExtractWarpPositions()
{
    if ( !file_loaded )
    {
        for ( int i = 0; i < num_warp_positions; ++i )
        {
            jdks_safe_delete_object ( warp_positions[i] );
        }

        num_warp_positions = 0;
        return;
    }

    Stop();
    // delete all our current warp positions

    for ( int i = 0; i < num_warp_positions; ++i )
    {
        jdks_safe_delete_object ( warp_positions[i] );
    }

    num_warp_positions = 0;

    while ( num_warp_positions < MAX_WARP_POSITIONS )
    {
        if ( !seq.GoToMeasure ( num_warp_positions * MEASURES_PER_WARP, 0 ) )
        {
            break;
        }

        // save current sequencer state at this position
        warp_positions[num_warp_positions++] =
            new MIDISequencerState (
            *seq.GetState()
        );
    }

    seq.GoToMeasure ( 0, 0 );
}
*/
/* NEW by NC */
void AdvancedSequencer::ExtractWarpPositions()
{
    if ( !file_loaded )
    {
        warp_positions.clear();
        return;
    }

    Stop();
    // warp_positions is now a vector of objects ( not pointers ) so we can minimize memory dealloc/alloc

    /*
    for ( int i = 0; i < warp_positions.size(); ++i )
    {
        jdks_safe_delete_object ( warp_positions[i] );
    }
    */

    int num_warp_positions = 0;
    while ( seq.GoToMeasure ( num_warp_positions * MEASURES_PER_WARP ) )
    {

        // save current sequencer state at this position
        if ( num_warp_positions < warp_positions.size() )
        {   // copy if it's already contained ...
            warp_positions[num_warp_positions] = *seq.GetState();
        }
        else
        {   // ... or push back
            warp_positions.push_back( MIDISequencerState( *seq.GetState() ) );
        }
        num_warp_positions++;
    }
    while ( warp_positions.size() > num_warp_positions )
    {   // adjust vector size if it was greater than actual (currently num_warp_positions is the last
        // vector index plus 1, so the comparison is OK)
        warp_positions.pop_back();
    }

    // now find the actual number of measures
    num_measures = (num_warp_positions - 1) * MEASURES_PER_WARP;
    while (seq.GoToMeasure( num_measures + 1 ))
    {
        num_measures++;
    }

    seq.GoToZero();
}


void AdvancedSequencer::CatchEventsBefore()
{
    MIDITimedBigMessage msg;
    const MIDITimedBigMessage *msgp;
    MIDIMultiTrackIterator iter( seq.GetState()->multitrack );
    int trk;

    iter.GoToTime( 0 );
    while ( iter.GetCurEvent( &trk, &msgp ) && msgp->GetTime() < seq.GetCurrentMIDIClockTime() )
    {
        msg = *msgp;
        if (msg.IsChannelMsg()) // channel messages
        {
            if ( msg.IsControlChange() || msg.IsProgramChange() || msg.IsPitchBend() )
            {   // only send these messages
                OutputMessage(msg);
            }
        }
        else
        if ( msg.IsMetaEvent() )
        {
            if ( msg.IsTempo() )
            {   // discards all meta events except tempo messages
                OutputMessage(msg);
            }
        }
        else
        if ( msg.IsSystemExclusive() )  // TODO: which SysEx should we send?
        {
            OutputMessage(msg);
            Sleep(10);
        }
        else
        {   // TODO: which other messages should we send???
            // DumpMIDITimedBigMessage(&msg);
            OutputMessage( msg );   // prudentially sends an unrecognized message
        }
        iter.GoToNextEvent(); // NOTE BY NC: this GetCurEvent should move the iterator to the
                              // next step, so this should be unneeded
    }
}


void AdvancedSequencer::CatchEventsBefore(int trk) {
    MIDITimedBigMessage msg;
    MIDITrack* t = tracks.GetTrack(trk);

    for (int i = 0; i < t->GetNumEvents(); ++i )
    {
        msg = *t->GetEvent( i );
        if ( msg.GetTime() >= seq.GetCurrentMIDIClockTime() )
        {
            break;
        }

        if (msg.IsChannelMsg()) // channel messages
        {
            if ( msg.IsControlChange() || msg.IsProgramChange() || msg.IsPitchBend() )
            {   // only send these messages
                OutputMessage(msg);
            }
        }
        else
        if ( msg.IsMetaEvent() )
        {
            if ( msg.IsTempo() )
            {   // discards all meta events except tempo messages
                OutputMessage(msg);
            }
        }
        else
        if ( msg.IsSystemExclusive() )      // TODO: which SysEx should we send???
        {
            OutputMessage(msg);
            Sleep(10);
        }
        else
        {   // TODO: which other messages should we send???
            // DumpMIDITimedBigMessage(&msg);
            OutputMessage( msg );   // prudentially sends an unrecognized message
        }
    }
}

}
