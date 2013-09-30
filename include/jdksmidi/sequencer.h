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
// MODIFIED by N. Cassetta  ncassetta@tiscali.it
// search /* NC */ for modifies
//

#ifndef JDKSMIDI_SEQUENCER_H
#define JDKSMIDI_SEQUENCER_H

#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/tempo.h"
#include "jdksmidi/matrix.h"
#include "jdksmidi/process.h"

namespace jdksmidi
{

class MIDISequencerGUIEvent;
class MIDISequencerGUIEventNotifier;
class MIDISequencerTrackState;
class MIDISequencer;


///
/// This class holds data for a message that the sequencer can send to the GUI to warn it that
/// something is happened.
/// A MIDISequencerGUIEvent belongs to one of these four groups:
/// - GROUP_ALL: generic group, used by the sequencer to request the GUI for a general refresh
/// - GROUP_CONDUCTOR: events as tempo, time, key change ...
/// - GROUP_TRANSPORT: start, stop ...
/// - GROUP_TRACK: note, program, control change ...
/// Every group has some items, denoting different events
/// For GROUP_TRACK is also used a subgroup parameter, i.e. the track of the event.
///

class MIDISequencerGUIEvent
{
public:

    /// Constructor: a generic event with all attributes set to 0
    MIDISequencerGUIEvent()
    {
        bits = 0;
    }

    /// Copies directly parameters
    MIDISequencerGUIEvent ( unsigned long bits_ )
        : bits ( bits_ )
    {
    }

    /// Copy constructor
    MIDISequencerGUIEvent ( const MIDISequencerGUIEvent &e )
        : bits ( e.bits )
    {
    }

    /// Construct an instance starting from its group, subgroup, item
    MIDISequencerGUIEvent ( int group, int subgroup, int item = 0 )
    {
        bits = ( ( group & 0xff ) << 24 )
               | ( ( subgroup & 0xfff ) << 12 )
               | ( ( item & 0xfff ) << 0 );
    }

    /// Get internal data
    operator unsigned long () const
    {
        return bits;
    }

    /// Set the event group, subgroup and item
    void SetEvent ( int group, int subgroup = 0, int item = 0 )
    {
        bits = ( ( group & 0xff ) << 24 )
               | ( ( subgroup & 0xfff ) << 12 )
               | ( ( item & 0xfff ) << 0 );
    }

    /// @name The 'Get' methods
    //@{

    int GetEventGroup() const
    {
        return ( int ) ( ( bits >> 24 ) & 0xff );
    }

    int GetEventSubGroup() const
    {
        return ( int ) ( ( bits >> 12 ) & 0xfff );
    }

    int GetEventItem() const
    {
        return ( int ) ( ( bits >> 0 ) & 0xfff );
    }
    //@}

    /// Main groups
    enum
    {
        GROUP_ALL = 0,              ///< Generic group: used by the MIDISequencer to request a full GUI reset
        GROUP_CONDUCTOR,            ///< Conductor group
        GROUP_TRANSPORT,            ///< Transport group
        GROUP_TRACK                 ///< Track group (the subgroup is the track of the event)
    };

    /// Items in conductor group
    enum
    {
        GROUP_CONDUCTOR_ALL = 0,    ///< Generic event (not currently used)
        GROUP_CONDUCTOR_TEMPO,      ///< Tempo change
        GROUP_CONDUCTOR_TIMESIG,    ///< Timesig change
        GROUP_CONDUCTOR_KEYSIG,     ///< Keysig change
        GROUP_CONDUCTOR_MARKER      ///< Marker
    };

    /// Items in transport group
    enum
    {
        GROUP_TRANSPORT_ALL = 0,    ///< Generic event (not currently used)
        GROUP_TRANSPORT_MODE,       ///< Sequencer start and stop
        GROUP_TRANSPORT_MEASURE,    ///< Start of a measure
        GROUP_TRANSPORT_BEAT,       ///< Beat marker
        GROUP_TRANSPORT_ENDOFSONG   ///< End of playback
    };

    /// Items in track group (the track is in the subgroup)
    enum
    {
        GROUP_TRACK_ALL = 0,        ///< Generic event (not currently used)
        GROUP_TRACK_NAME,           ///< Track got its name
        GROUP_TRACK_PG,             ///< Program change
        GROUP_TRACK_NOTE,           ///< Note
        GROUP_TRACK_VOLUME          ///< Volume change
    };

private:
    unsigned long bits;
};


///
/// This is the object that sends messsages to the GUI.
/// The base class is pure virtual, because we need GUI details for really sending messages; currently
/// there are two implementations: a text notifier and a WIN32 specific GUI notifier
/// (see MIDISequencerGUIEventNotifierText, MIDISequencerGUIEventNotifierWin32)
///

class MIDISequencerGUIEventNotifier
{
public:

    /// The constructor.
    MIDISequencerGUIEventNotifier()
    {
    }

    /// The destructor.
    virtual ~MIDISequencerGUIEventNotifier()
    {
    }

    /// Notifies the MIDISequencerGUIEvent e, originated from the MIDISequencer seq
    virtual void Notify ( const MIDISequencer *seq, MIDISequencerGUIEvent e ) = 0;

    /// Returns the enable/disable status.
    virtual bool GetEnable() const = 0;

    /// Sets message sending on/off.
    virtual void SetEnable ( bool f ) = 0;
};



///
/// This class inherits from the pure virtual MIDISequencerGUIEventNotifier, and notifies text messages
/// to a FILE class.
///

class MIDISequencerGUIEventNotifierText :
    public MIDISequencerGUIEventNotifier
{
public:

    /// The constructor. The class will send text messages to the FILE *f
    MIDISequencerGUIEventNotifierText ( FILE *f_ )
        : f ( f_ ), en ( true )
    {
    }

    /// The destructor
    virtual ~MIDISequencerGUIEventNotifierText()
    {
    }

    virtual void Notify ( const MIDISequencer *seq, MIDISequencerGUIEvent e );

    virtual bool GetEnable() const
    {
        return en;
    }

    virtual void SetEnable ( bool f )
    {
        en = f;
    }

private:

    FILE *f;
    bool en;
};


/* NOTE BY NC: I eliminated this class because its name was somewhat confusing: it is a notifier,
   but inherits by a MIDIProcessor and CONTAINS a notifier.
   The class was only inherited by the MIDISequencerTrackState class and had no utility for the end user,
   so its features are incorporated in the latter

class MIDISequencerTrackNotifier : public MIDIProcessor
{
public:
    MIDISequencerTrackNotifier (
        const MIDISequencer *seq_,
        int trk,
        MIDISequencerGUIEventNotifier *n
    );

    virtual ~MIDISequencerTrackNotifier();

    void SetNotifier (
        const MIDISequencer *seq_,
        int trk,
        MIDISequencerGUIEventNotifier *n
    )
    {
        seq = seq_;
        track_num = trk;
        notifier = n;
    }


    void Notify ( int item );
    void NotifyConductor ( int item );      // NOTE by NC: this is now unneeded: could be eliminated

private:
    const MIDISequencer *seq;
    int track_num;
    MIDISequencerGUIEventNotifier *notifier;
};
*/



///
/// This class inherits from the pure virtual MIDIProcessor and processes MIDI messages
/// implementing muting, soloing, rechanneling, velocity scaling and transposing.
/// Moreover, you can set a custom MIDIProcessor pointer which extra-processes messages.
/// The MIDISequencer class contains an independent MIDISequencerTrackProcessor for every MIDI Track.
/// Advanced classes like MIDISequencer and AdvancedSequencer allow you to set muting, tramsposing,
/// etc. without dealing with it: the only useful function for the user is the extra processing hook.
///

class MIDISequencerTrackProcessor : public MIDIProcessor
{
public:
    /// The constructor. Default is no processing (MIDI messages leave the processor unchanged)
    MIDISequencerTrackProcessor();

    /// The destructor
    virtual ~MIDISequencerTrackProcessor()
    {
    }

    /// Resets all values to default state
    virtual void Reset();

    /// Processes message msg, changing its parameters according the state of the processor
    virtual bool Process ( MIDITimedBigMessage *msg );

    bool mute;                  ///< track is muted
    bool solo;                  ///< track is soloed
    int velocity_scale;         ///< current velocity scale value for note ons, 100=normal
    int rechannel;              ///< rechannelization value, or -1 for none
    int transpose;              ///< amount to transpose note values
    MIDIProcessor *extra_proc;  ///< extra midi processing for this track
};


///
/// This class stores curremt MIDI parameters for a track.
/// It stores program, volume, track name, amd a matrix with notes on and off. Furthermore it inherits from
/// the pure virtual MIDIProcessor: the MIDISequencer sends MIDI messages to it and it processes them  remembering
/// actual parameters and notifying chamges to the GUI.
/// The MIDISequencerState class contains an independent MIDISequencerTrackState for every MIDI Track and you can
/// ask it for knowing actual track parameters. However, advanced class AdvancedSequencer allows you to know them
/// without dealing with this, so the use of this class is mainly internal. However, you could subclass it if you
/// want to keep track of other parameters (pan, chorus, etc.)
///

class MIDISequencerTrackState : public MIDIProcessor
{
public:

    /// The constructor.
    /// Initial attributes are pg = -1, volume = 100, bender_value = 0, all notes off, track_name = ""
    /// \param seq_ the seguencer that sends messages (used only for notifying)
    /// \param trk the number of the track
    /// \param n the notifier: if set to 0 no notifying
    MIDISequencerTrackState (
        const MIDISequencer *seq_,
        int trk,
        MIDISequencerGUIEventNotifier *n
    );

    /// The destructor
    virtual ~MIDISequencerTrackState()
    {
    }

    /// Resets default values, but not track_name and got_good_track_name
    virtual void GoToZero();

    /// As above, but resets also track_name and got_good_track_name
    virtual void Reset();

    /// Processes the message msg, notifying the GUI if needed
    virtual bool Process ( MIDITimedBigMessage *msg );

    /// Notifies events to the GUI
    void Notify ( int item );

    int pg;                         ///< current program change, or -1 if not set
    int volume;                     ///< current volume controller value
    int bender_value;               ///< last seen bender value
    char track_name[256];           ///< track name
    bool got_good_track_name;       ///< true if we dont want to use generic text events for track name

    bool notes_are_on;              ///< true if there are any notes currently on
    MIDIMatrix note_matrix;         ///< to keep track of all notes on

    const MIDISequencer* seq;       ///< the sequencer to which the track belongs
    int track;                      ///< the number of the track
    MIDISequencerGUIEventNotifier* notifier;        ///< the notifier: if 0 there is no notifying
};



///
/// This class stores current MIDI general parameters.
/// It contains a MIDIMultiTrackIterator, allowing to set a 'now' time: when the current time changes (because
/// the sequencer is playing or it is moved by the user) the class stores current timesig, keysig, tempo(BPM),
/// marker, measure and beat data. Furthermore it inherits from the pure virtual MIDIProcessor:
/// the MIDISequencer sends to it MIDI messages and it processes them  remembering actual parameters and
/// notifying chamges to the GUI.
/// This class contains an independent MIDISequencerTrackState for every MIDI Track and you can
/// ask it for knowing actual parameters. However, advanced class AdvancedSequencer allows you to know them
/// without dealing with this, so the use of this class is mainly internal.
/// However, you could subclass it if you want to keep track of other parameters.
///

class MIDISequencerState : public MIDIProcessor
{
public:

    /// The constructor sets sequencer current time to 0. It allocates memory to hold a MIDISequencerTrackState
    /// for every track. Parameters are not owned.
    MIDISequencerState ( const MIDISequencer *s,
                         const MIDIMultiTrack *multitrack_,
                         MIDISequencerGUIEventNotifier *n );

    /// The copy constructor
    MIDISequencerState ( const MIDISequencerState &s );

    /// The destructor frees allocated memory
    ~MIDISequencerState();

    /// The equal operator
    const MIDISequencerState & operator = ( const MIDISequencerState &s );

    /// Resets the state to current time = 0.
    void Reset();                                       /* NC */    // new

    /// Processes the message msg: if it is a channel message (or a track name meta) send it to a
    /// MIDISequencerTrackState, otherwise notify the GUI directly
    bool Process( MIDITimedBigMessage* msg );           /* NC */    // new

    /// Notifies events to GUI
    void Notify( int group, int item = 0 );             /* NC */    // new

    MIDISequencerGUIEventNotifier *notifier;
    const MIDISequencer* seq;                           /* NC for notifying */
    const MIDIMultiTrack *multitrack;
    int num_tracks;                                 ///< nunber of tracks of the sequencer

    MIDISequencerTrackState *track_state[64];
    MIDIMultiTrackIterator iterator;
    MIDIClockTime cur_clock;                        ///< current time MIDI clock
    float cur_time_ms;                              ///< current time in ms
    int cur_beat;                                   ///< current beat
    int cur_measure;                                ///< current measure
    MIDIClockTime next_beat_time;                   ///< used internally by Process()
    float tempobpm;                                 ///< current tempo in beats per minute
    char timesig_numerator;                         ///< numerator of current time signature
    char timesig_denominator;                       ///< denominator of current time signature
    char keysig_sharpflat;                          ///< current key signature accidentals
    char keysig_mode;                               ///< major/minor mode
    char marker_name[40];                           ///< current marker name
    int last_event_track;                           ///< used internally by Process()
};


///
/// A complete sequencer. This class holds:
/// - a MIDIMultiTrack for storing MIDI messages
/// - a MIDISequencerTrackProcessor for every track, allowing muting, soloing, transposition, ecc.
/// - a MIDIMultiTrackIterator, allowing to set a 'now' time, moving it along
/// - a MIDISequencerGUIEventNotifier, that notifies the GUI about MIDI events
/// \note This class has no playing capacity. For playing MIDI content you must use it together with a
/// MIDIManager. See the example files for effective using. AdvancedSequencer is an all-in-one class for
/// sequencing and playing
///

 class MIDISequencer
{
public:

    /// The constructor.
    /// \param m a pointer to a MIDIMultitrack that will hold MIDI messages
    /// \param n a pointer to a MIDISequencerGUIEventNotifier. If you leave 0 the sequencer will not notify
    /// the GUI.
    MIDISequencer (
        const MIDIMultiTrack *m,
        MIDISequencerGUIEventNotifier *n = 0
    );

    /// The destructor frees allocated memory. The MIDIMultitrack and the MIDISequencerGUIEventNotifier are not
    /// owned by the MIDISequencer
    virtual ~MIDISequencer();

    /// Resets the corresponding MIDISequencerTrackState and MIDISequencerTrackProcessor. See
    /// MIDISequencerTrackState::Reset() and MIDISequencerTrackProcessor::Reset()
    void ResetTrack ( int trk );

    /// Call ResetTrack() for all tracks
    void ResetAllTracks();

    /// Returns current MIDIClockTime
    MIDIClockTime GetCurrentMIDIClockTime() const
    {
        return state.cur_clock;
    }

    /// Returns curremt time im milliseconds
    double GetCurrentTimeInMs() const
    {
        return state.cur_time_ms;
    }

    /// Returns current beat
    int GetCurrentBeat() const
    {
        return state.cur_beat;
    }

    /// Returns current measure
    int GetCurrentMeasure() const
    {
        return state.cur_measure;
    }

    /// Returns curremt tempo scale (1.00 = no scaling, 2.00 = twice faster, etc.)
    double GetCurrentTempoScale() const
    {
        return ( ( double ) tempo_scale ) * 0.01;
    }

    ///< Returns current tempo (BPM) without scaling ( actual tempo is GetCurrentTempo() * GetCurrentTempoScale() )
    double GetCurrentTempo() const
    {
        return state.tempobpm;
    }

    /// Returns current MIDISequencerState (i.e. the global sequencer state at current time). You can easily
    /// jump from a time to another saving and retrieving sequencer states.
    MIDISequencerState *GetState()
    {
        return &state;
    }

    const MIDISequencerState *GetState() const
    {
        return &state;
    }

    /// Returns the MIDISequencerTrackState for track trk
    MIDISequencerTrackState * GetTrackState ( int trk )
    {
        return state.track_state[trk];
    }

    const MIDISequencerTrackState * GetTrackState ( int trk ) const
    {
        return state.track_state[trk];
    }

    /// Returns the MIDISequencerTrackProcessor for track trk
    MIDISequencerTrackProcessor * GetTrackProcessor ( int trk )
    {
        return track_processors[trk];
    }

    const MIDISequencerTrackProcessor * GetTrackProcessor ( int trk ) const
    {
        return track_processors[trk];
    }

    /// Returns the number of tracks of the MIDIMultiTrack
    int GetNumTracks() const
    {
        return state.num_tracks;
    }

    /// Returns the solo mode on/off
    bool GetSoloMode() const
    {
        return solo_mode;
    }

    /// Copies the MIDISequencerState s into the internal sequencer state. You can easily
    /// jump from a time to another saving and retrieving sequencer states.
    void SetState ( MIDISequencerState *s )
    {
        state = *s;
    }

    /// Sets the tempo scale (1.00 = no scaling, 2.00 = twice faster, etc)
    void SetCurrentTempoScale ( float scale )
    {
        tempo_scale = ( int ) ( scale * 100 );
    }

    /// Soloes/unsoloes a track
    /// \param m on/off
    /// \param trk the nunber of the track if m is true, otherwhise you can leave default value
    void SetSoloMode ( bool m, int trk = -1 );

    /// Sets the 'now' time to the beginning of the song, upgrading the internal status.
    /// Notifies the GUI a GROUP_ALL notifier event to signify a GUI reset
    void GoToZero();

    /// Sets the 'now' time to the MIDI time time_clk, upgrading the internal status.
    /// Notifies the GUI a GROUP_ALL notifier event to signify a GUI reset
    /// \return _true_ if the time time_clk is effectively reached, _false_ otherwise (if time_clk is after
    /// the end of the song)
    bool GoToTime ( MIDIClockTime time_clk );

    /// Same as GoToTime(), but the time is given in milliseconds
    bool GoToTimeMs ( float time_ms );

    /// Sets the 'now' time to the given measure and beat, upgrading the internal status.
    /// \return see GoToTime()
    bool GoToMeasure ( int measure, int beat = 0 );


    bool GetNextEventTimeMs ( float *t );
    bool GetNextEventTimeMs ( double *t );

    /// Get the time of the next event (respect to the 'now' time) in MIDI ticks
    /// \return _true_ if there is a next event (and *t is a valid time) _false_ otherwise (*t is undefined)
    bool GetNextEventTime ( MIDIClockTime *t );

    /// Get the next event respect to the 'now' time.
    /// \param[out] tracknum: the track of the next event
    /// \param[out] msg the MIDI event
    /// \return _true_ if there is a next event (and the paraneters are valid), _false_ otherwise (parameters undefined)
    bool GetNextEvent ( int *tracknum, MIDITimedBigMessage *msg );



    /// Returns the total duration of the song (i.e.\ the time of last not end of track midi event)
    double GetMisicDurationInSeconds();

protected:

    /// Internal use: scans events at 'now' time upgrading the sequencer status
    void ScanEventsAtThisTime();

    MIDITimedBigMessage beat_marker_msg;

    bool solo_mode;
    int tempo_scale;

    int num_tracks;
    MIDISequencerTrackProcessor *track_processors[64];

    MIDISequencerState state;
} ;

}

#endif
