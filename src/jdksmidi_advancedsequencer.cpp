#include "jdksmidi/advancedsequencer.h"
#include "jdksmidi/driverdump.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/manager.h"
#include "jdksmidi/matrix.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/process.h"
#include "jdksmidi/sequencer.h"
#include "jdksmidi/sysex.h"
#include "jdksmidi/track.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace jdksmidi {
static void FixQuotes(char* s_)
{
    auto* s = (std::uint8_t*)s_;

    while (*s) {
        if (*s == 0xd2 || *s == 0xd3) {
            *s = '"';
        }

        else if (*s == 0xd5) {
            *s = '\'';
        }

        else if (*s >= 0x80) {
            *s = ' ';
        }

        s++;
    }
}

AdvancedSequencer::AdvancedSequencer()
    : thru_processor(2)
    , thru_transposer()
    , thru_rechannelizer()
    , driver(256, stdout)
    , tracks(17)
    , notifier(stdout)
    , seq(&tracks, &notifier)
    , mgr(&driver, &notifier, &seq)
    , repeat_start_measure(0)
    , repeat_end_measure(0)
    , repeat_play_mode(false)
    , num_warp_positions(0)
    , file_loaded(false)
    , chain_mode(false)
{}

AdvancedSequencer::~AdvancedSequencer()
{
    stop();
    close_midi();

    for (int i = 0; i < num_warp_positions; ++i) {
        delete warp_positions[i];
    }
}

bool AdvancedSequencer::open_midi(int in_port, int out_port, int timer_resolution)
{
#if 0
    close_midi();

    if ( !driver.start_timer ( timer_resolution ) )
    {
        return false;
    }

    if ( in_port != -1 )
    {
        driver.open_midi_in_port ( in_port );
    }

    if ( driver.open_midi_out_port ( out_port ) )
    {
        return true;
    }

    else
    {
        return false;
    }

#else
    return true;
#endif
}

void AdvancedSequencer::close_midi()
{
    stop();
#if 0
    driver.stop_timer();
    driver.all_notes_off();
    Sleep ( 100 );
    driver.close_midi_in_port();
    driver.close_midi_out_port();
#endif
}

void AdvancedSequencer::set_midi_thru_enable(bool f)
{
    driver.set_thru_enable(f);
}

bool AdvancedSequencer::get_midi_thru_enable() const
{
    return driver.get_thru_enable();
}

void AdvancedSequencer::set_midi_thru_channel(int chan)
{
    thru_rechannelizer.set_all_rechan(chan);
    driver.all_notes_off();
}

int AdvancedSequencer::get_midi_thru_channel() const
{
    return thru_rechannelizer.get_rechan_map(0);
}

void AdvancedSequencer::set_midi_thru_transpose(int val)
{
    thru_transposer.set_all_transpose(val);
    driver.all_notes_off();
}

int AdvancedSequencer::get_midi_thru_transpose() const
{
    return thru_transposer.get_transpose_channel(0);
}

bool AdvancedSequencer::load(char const* fname)
{
    char realname[1024];
    strcpy(realname, fname);
    int orignamelen = strlen(fname);
    chain_mode = false;

    if (orignamelen > 0) {
        if (realname[orignamelen - 1] == '+') {
            realname[orignamelen - 1] = 0;
            chain_mode = true;
        }
    }

    MIDIFileReadStreamFile mfreader_stream(realname);
    MIDIFileReadMultiTrack track_loader(&tracks);
    MIDIFileRead reader(&mfreader_stream, &track_loader);
    stop();
    driver.all_notes_off();
    tracks.clear();
    seq.reset_all_tracks();

    if (reader.parse()) {
        file_loaded = true;
        reset();
        go_to_measure(0);
        extract_warp_positions();
    }

    else {
        file_loaded = false;
    }

    return file_loaded;
}

void AdvancedSequencer::reset()
{
    stop();
    driver.all_notes_off();
    unmute_all_tracks();
    unsolo_track();
    set_tempo_scale(1.00);
    seq.reset_all_tracks();
    go_to_measure(0);
}

void AdvancedSequencer::go_to_time(MIDIClockTime t)
{
    if (mgr.is_seq_play()) {
        stop();
        seq.go_to_time(t + 1);
        play();
    }

    else {
        seq.go_to_time(t + 1);
    }
}

void AdvancedSequencer::go_to_measure(int measure, int beat)
{
    if (!file_loaded) {
        return;
    }

    // figure out which warp item we use
    // try warp to the last warp point BEFORE the
    // requested measure
    int warp_to_item = (measure - 1) / MEASURES_PER_WARP;

    if (warp_to_item >= num_warp_positions)
        warp_to_item = num_warp_positions - 1;

    if (warp_to_item < 0)
        warp_to_item = 0;

    if (mgr.is_seq_play()) {
        stop();

        if (warp_positions[warp_to_item]) {
            seq.set_state(warp_positions[warp_to_item]);
        }

        seq.go_to_measure(measure, beat);
        play();
    }

    else {
        if (warp_positions[warp_to_item]) {
            seq.set_state(warp_positions[warp_to_item]);
        }

        seq.go_to_measure(measure, beat);

        for (int i = 0; i < seq.get_num_tracks(); ++i) {
            seq.get_track_state(i)->note_matrix.clear();
        }
    }
}

void AdvancedSequencer::play(int clock_offset)
{
    if (!file_loaded) {
        return;
    }

    stop();

    for (int i = 0; i < seq.get_num_tracks(); ++i) {
        seq.get_track_state(i)->note_matrix.clear();
    }

    if (repeat_play_mode) {
        seq.go_to_measure(repeat_start_measure);
    }

    MIDIClockTime cur_time = seq.get_current_midi_clock_time();

    if (static_cast<std::int32_t>(cur_time) > -clock_offset)
        cur_time += clock_offset;

    seq.go_to_time(cur_time);
    mgr.set_seq_offset(static_cast<std::uint32_t>(seq.get_current_time_in_ms()));
    mgr.set_time_offset(0);
    mgr.seq_play();
}

void AdvancedSequencer::repeat_play(bool enable, int start_measure, int end_measure)
{
    if (!file_loaded) {
        return;
    }

    if (start_measure < end_measure && start_measure >= 0) {
        repeat_play_mode = enable;
        repeat_start_measure = start_measure;
        repeat_end_measure = end_measure;
    }

    else {
        repeat_play_mode = false;
    }

    mgr.set_repeat_play(repeat_play_mode, repeat_start_measure, repeat_end_measure);
}

void AdvancedSequencer::pause()
{
    if (!file_loaded) {
        return;
    }

    stop();
}

void AdvancedSequencer::stop()
{
    if (!file_loaded) {
        return;
    }

    if (!mgr.is_seq_stop()) {
        mgr.seq_stop();
        driver.all_notes_off();

        for (int i = 0; i < seq.get_num_tracks(); ++i) {
            seq.get_track_state(i)->note_matrix.clear();
        }
    }
}

void AdvancedSequencer::unmute_all_tracks()
{
    if (!file_loaded) {
        return;
    }

    for (int i = 0; i < seq.get_num_tracks(); ++i) {
        if (seq.get_track_processor(i)->mute) {
            seq.get_track_state(i)->note_matrix.clear();
            seq.get_track_processor(i)->mute = false;
        }
    }

    driver.all_notes_off();
}

void AdvancedSequencer::solo_track(int trk)
{
    if (!file_loaded) {
        return;
    }

    if (trk == -1) {
        seq.set_solo_mode(false);
        driver.all_notes_off();

        for (int i = 0; i < seq.get_num_tracks(); ++i) {
            seq.get_track_state(i)->note_matrix.clear();
        }
    }

    else {
        seq.set_solo_mode(true, trk);
        driver.all_notes_off();

        for (int i = 0; i < seq.get_num_tracks(); ++i) {
            seq.get_track_state(i)->note_matrix.clear();
        }
    }
}

void AdvancedSequencer::unsolo_track()
{
    if (!file_loaded) {
        return;
    }

    seq.set_solo_mode(false);
    driver.all_notes_off();

    for (int i = 0; i < seq.get_num_tracks(); ++i) {
        seq.get_track_state(i)->note_matrix.clear();
    }
}

void AdvancedSequencer::set_track_mute(int trk, bool f)
{
    if (!file_loaded) {
        return;
    }

    seq.get_track_processor(trk)->mute = f;
    driver.all_notes_off();
}

void AdvancedSequencer::set_tempo_scale(double scale)
{
    if (!file_loaded) {
        return;
    }

    seq.set_current_tempo_scale(static_cast<float>(scale));
}

double AdvancedSequencer::get_tempo_without_scale() const
{
    return seq.get_current_tempo();
}

double AdvancedSequencer::get_tempo_with_scale() const
{
    return seq.get_current_tempo() * seq.get_current_tempo_scale();
}

int AdvancedSequencer::get_measure() const
{
    if (!file_loaded) {
        return 0;
    }

    return seq.get_current_measure();
}

int AdvancedSequencer::get_beat() const
{
    if (!file_loaded) {
        return 0;
    }

    return seq.get_current_beat();
}

int AdvancedSequencer::get_time_sig_numerator() const
{
    if (!file_loaded) {
        return 4;
    }

    return seq.get_track_state(0)->timesig_numerator;
}

int AdvancedSequencer::get_time_sig_denominator() const
{
    if (!file_loaded) {
        return 4;
    }

    return seq.get_track_state(0)->timesig_denominator;
}

int AdvancedSequencer::get_track_note_count(int trk) const
{
    if (!file_loaded) {
        return 0;
    }

    if (mgr.is_seq_stop()) {
        return 0;
    }

    else {
        return seq.get_track_state(trk)->note_matrix.get_total_count();
    }
}

char const* AdvancedSequencer::get_track_name(int trk) const
{
    if (!file_loaded) {
        return "";
    }

    return seq.get_track_state(trk)->track_name;
}

int AdvancedSequencer::get_track_volume(int trk) const
{
    if (!file_loaded) {
        return 100;
    }

    return seq.get_track_state(trk)->volume;
}

void AdvancedSequencer::set_track_velocity_scale(int trk, int scale)
{
    if (!file_loaded) {
        return;
    }

    seq.get_track_processor(trk)->velocity_scale = scale;
}

int AdvancedSequencer::get_track_velocity_scale(int trk) const
{
    if (!file_loaded) {
        return 100;
    }

    return seq.get_track_processor(trk)->velocity_scale;
}

void AdvancedSequencer::set_track_rechannelize(int trk, int chan)
{
    if (!file_loaded) {
        return;
    }

    seq.get_track_processor(trk)->rechannel = chan;
    driver.all_notes_off();
    seq.get_track_state(trk)->note_matrix.clear();
}

int AdvancedSequencer::get_track_rechannelize(int trk) const
{
    if (!file_loaded) {
        return -1;
    }

    return seq.get_track_processor(trk)->rechannel;
}

void AdvancedSequencer::set_track_transpose(int trk, int trans)
{
    if (!file_loaded) {
        return;
    }

    bool was_playing = mgr.is_seq_play();

    if (mgr.is_seq_play()) {
        was_playing = true;
        mgr.seq_stop();
    }

    if (trk == -1) {
        for (trk = 0; trk < tracks.get_num_tracks(); ++trk) {
            seq.get_track_processor(trk)->transpose = trans;
        }
    }

    else {
        seq.get_track_processor(trk)->transpose = trans;
    }

    if (was_playing) {
#if 0
        driver.reset_midi_out();
        driver.all_notes_off();
#endif
        seq.get_track_state(trk)->note_matrix.clear();
        mgr.seq_play();
    }
}

int AdvancedSequencer::get_track_transpose(int trk) const
{
    if (!file_loaded) {
        return 0;
    }

    return seq.get_track_processor(trk)->transpose;
}

void AdvancedSequencer::extract_markers(std::vector<std::string>* list)
{
    if (!file_loaded) {
        list->clear();
        num_markers = 0;
        return;
    }

    MIDITrack* t = tracks.get_track(0);
    list->clear();
    int cnt = 0;
    int measure = 0;
    int beat = 0;
    int timesig_numerator = 4;
    int timesig_denominator = 4;
    MIDIClockTime last_beat_time = 0;
    int clks_per_beat = tracks.get_clks_per_beat();

    for (int i = 0; i < t->get_num_events(); ++i) {
        auto m = t->get_event_address(i);

        if (m) {
            // how many beats have gone by since the last event?
            std::int32_t beats_gone_by = (m->get_time() - last_beat_time) / clks_per_beat;

            if (beats_gone_by > 0) {
                // calculate what our new measure/beat is
                beat += beats_gone_by;
                // carry over beat overflow to measure
                measure += beat / timesig_numerator;
                beat = beat % timesig_numerator;
                last_beat_time += (clks_per_beat * beats_gone_by);
            }

            if (m->is_meta_event() && m->is_time_sig()) {
                timesig_numerator = m->get_time_sig_numerator();
                timesig_denominator = m->get_time_sig_denominator();
                clks_per_beat = tracks.get_clks_per_beat() * 4 / timesig_denominator;
            }

            if (m->is_text_event() && m->get_sys_ex()) {
                if ((m->get_meta_type() == META_GENERIC_TEXT) ||
                    m->get_meta_type() == META_MARKER_TEXT || m->get_meta_type() == META_CUE_TEXT) {
                    char buf[256];
                    char line[384];
                    memcpy(buf, m->get_sys_ex()->get_buf(), m->get_sys_ex()->get_length());
                    buf[m->get_sys_ex()->get_length()] = '\0';
                    FixQuotes(buf);
                    snprintf(line, sizeof(line), "%03d:%d        %s", measure + 1, beat + 1, buf);
                    list->emplace_back(line);
                    marker_times[cnt++] = m->get_time();
                }
            }
        }
    }

    num_markers = cnt;
}

int AdvancedSequencer::get_current_marker() const
{
    if (!file_loaded) {
        return -1;
    }

    // find marker with largest time that
    // is before cur_time
    MIDIClockTime cur_time = seq.get_current_midi_clock_time();
    cur_time += 20;
    int last = -1;

    for (int i = 0; i < num_markers; ++i) {
        if (marker_times[i] > cur_time) {
            break;
        }

        else {
            last = i;
        }
    }

    return last;
}

int AdvancedSequencer::find_first_channel_on_track(int trk)
{
    if (!file_loaded) {
        return -1;
    }

    int first_channel = -1;
    MIDITrack* t = tracks.get_track(trk);

    if (t) {
        // go through all events
        // until we find a channel message
        // and then return the channel number plus 1
        for (int i = 0; i < t->get_num_events(); ++i) {
            auto m = t->get_event_address(i);

            if (m) {
                if (m->is_channel_msg()) {
                    first_channel = m->get_channel() + 1;
                    break;
                }
            }
        }
    }

    return first_channel;
}

void AdvancedSequencer::extract_warp_positions()
{
    if (!file_loaded) {
        for (int i = 0; i < num_warp_positions; ++i) {
            delete warp_positions[i];
        }

        num_warp_positions = 0;
        return;
    }

    stop();
    // delete all our current warp positions

    for (int i = 0; i < num_warp_positions; ++i) {
        delete warp_positions[i];
    }

    num_warp_positions = 0;

    while (num_warp_positions < MAX_WARP_POSITIONS) {
        if (!seq.go_to_measure(num_warp_positions * MEASURES_PER_WARP, 0)) {
            break;
        }

        // save current sequencer state at this position
        warp_positions[num_warp_positions++] = new MIDISequencerState(*seq.get_state());
    }

    seq.go_to_measure(0, 0);
}

}  // namespace jdksmidi
