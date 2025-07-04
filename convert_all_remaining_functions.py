#!/usr/bin/env python3

import re
import os
import glob

# Comprehensive function mappings for all remaining UpperCamelCase functions
function_mappings = {
    # advancedsequencer.h functions
    "OpenMIDI": "open_midi",
    "CloseMIDI": "close_midi",
    "SetMIDIThruEnable": "set_midi_thru_enable",
    "GetMIDIThruEnable": "get_midi_thru_enable",
    "SetMIDIThruChannel": "set_midi_thru_channel",
    "GetMIDIThruChannel": "get_midi_thru_channel",
    "SetMIDIThruTranspose": "set_midi_thru_transpose",
    "GetMIDIThruTranspose": "get_midi_thru_transpose",
    "Play": "play",
    "RepeatPlay": "repeat_play",
    "Pause": "pause",
    "IsPlay": "is_play",
    "UnmuteAllTracks": "unmute_all_tracks",
    "SoloTrack": "solo_track",
    "UnSoloTrack": "unsolo_track",
    "SetTrackMute": "set_track_mute",
    "SetTempoScale": "set_tempo_scale",
    "GetTempoWithoutScale": "get_tempo_without_scale",
    "GetTempoWithScale": "get_tempo_with_scale",
    "GetMeasure": "get_measure",
    "GetBeat": "get_beat",
    "GetTrackNoteCount": "get_track_note_count",
    "GetTrackName": "get_track_name",
    "GetTrackVolume": "get_track_volume",
    "SetTrackVelocityScale": "set_track_velocity_scale",
    "GetTrackVelocityScale": "get_track_velocity_scale",
    "SetTrackRechannelize": "set_track_rechannelize",
    "GetTrackRechannelize": "get_track_rechannelize",
    "SetTrackTranspose": "set_track_transpose",
    "GetTrackTranspose": "get_track_transpose",
    "ExtractMarkers": "extract_markers",
    "GetCurrentMarker": "get_current_marker",
    "FindFirstChannelOnTrack": "find_first_channel_on_track",
    "ExtractWarpPositions": "extract_warp_positions",
    "IsChainMode": "is_chain_mode",

    # driverwin32.h functions  
    "Notify": "notify",
    "GetEnable": "get_enable",
    "SetEnable": "set_enable",
    "ResetMIDIOut": "reset_midi_out",
    "StartTimer": "start_timer",
    "OpenMIDIInPort": "open_midi_in_port",
    "OpenMIDIOutPort": "open_midi_out_port",
    "StopTimer": "stop_timer",
    "CloseMIDIInPort": "close_midi_in_port",
    "CloseMIDIOutPort": "close_midi_out_port",

    # edittrack.h functions
    "Match": "match",
    "Truncate": "truncate",
    "Merge": "merge",
    "Erase": "erase",
    "Delete": "delete_events",
    "Insert": "insert",
    "Shift": "shift",

    # file.h functions
    "ConvertTempoToFreq": "convert_tempo_to_freq",
    "To32Bit": "to_32_bit",
    "To16Bit": "to_16_bit",
    "ReadVariableLengthNumber": "read_variable_length_number",
    "WriteVariableLengthNumber": "write_variable_length_number",

    # fileread.h / filereadmultitrack.h functions
    "AddEventToMultiTrack": "add_event_to_multi_track",

    # filewrite.h functions
    "Seek": "seek",
    "IsValid": "is_valid",
    "Error": "error_handler",
    "WriteCharacter": "write_character",
    "IncrementCounters": "increment_counters",
    "WriteShort": "write_short",
    "Write3Char": "write_3_char",
    "WriteLong": "write_long",
    "WriteVariableNum": "write_variable_num",
    "WriteDeltaTime": "write_delta_time",

    # filewritemultitrack.h functions
    "Write": "write",
    "PreWrite": "pre_write",
    "PostWrite": "post_write",

    # keysig.h functions
    "IsMajor": "is_major",
    "SetSharpFlats": "set_sharp_flats",
    "GetSharpFlats": "get_sharp_flats",
    "GetNoteStatus": "get_note_status",
    "ConvertMIDINote": "convert_midi_note",
    "ProcessWhiteNote": "process_white_note",
    "ProcessBlackNote": "process_black_note",

    # manager.h functions
    "SetSeq": "set_seq",
    "GetSeq": "get_seq",
    "GetDriver": "get_driver",
    "SetTimeOffset": "set_time_offset",
    "GetTimeOffset": "get_time_offset",
    "SetSeqOffset": "set_seq_offset",
    "GetSeqOffset": "get_seq_offset",
    "SeqPlay": "seq_play",
    "SeqStop": "seq_stop",
    "SetRepeatPlay": "set_repeat_play",
    "IsSeqPlay": "is_seq_play",
    "IsSeqStop": "is_seq_stop",
    "IsSeqRepeat": "is_seq_repeat",
    "TimeTickPlayMode": "time_tick_play_mode",
    "TimeTickStopMode": "time_tick_stop_mode",

    # multitrack.h functions
    "Find": "find",

    # queue.h functions
    "Put": "put",
    "Get": "get",
    "Next": "next",
    "Peek": "peek",

    # sequencer.h functions
    "GetEventGroup": "get_event_group",
    "GetEventSubGroup": "get_event_sub_group", 
    "GetEventItem": "get_event_item",

    # smpte.h functions
    "GetSMPTERateFrequency": "get_smpte_rate_frequency",
    "GetSMPTERateFrequencyLong": "get_smpte_rate_frequency_long",
    "GetSampleRateFrequency": "get_sample_rate_frequency",
    "GetSampleRateFrequencyLong": "get_sample_rate_frequency_long",
    "SetSMPTERate": "set_smpte_rate",
    "GetSMPTERate": "get_smpte_rate",
    "SetSampleRate": "set_sample_rate",
    "GetSampleRate": "get_sample_rate",
    "SetSampleNumber": "set_sample_number",
    "GetSampleNumber": "get_sample_number",
    "AddHours": "add_hours",
    "AddMinutes": "add_minutes",
    "AddSeconds": "add_seconds",
    "AddFrames": "add_frames",
    "AddSubFrames": "add_sub_frames",
    "AddSamples": "add_samples",
    "IncHours": "inc_hours",
    "IncMinutes": "inc_minutes",
    "IncSeconds": "inc_seconds",
    "IncFrames": "inc_frames",
    "IncSubFrames": "inc_sub_frames",
    "IncSamples": "inc_samples",
    "DecHours": "dec_hours",
    "DecMinutes": "dec_minutes",
    "DecSeconds": "dec_seconds",
    "DecFrames": "dec_frames",
    "DecSubFrames": "dec_sub_frames",
    "DecSamples": "dec_samples",
    "SampleToTime": "sample_to_time",
    "TimeToSample": "time_to_sample",
    "Compare": "compare",
    "Add": "add",
    "Subtract": "subtract",
    "GetSampleRateLong": "get_sample_rate_long",
    "GetSMPTERateLong": "get_smpte_rate_long",

    # song.h functions
    # Note: GetSeq already handled in manager.h mapping
}

def convert_file(file_path):
    """Convert function names in a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Apply function conversions with careful regex patterns
        for old_name, new_name in function_mappings.items():
            # Pattern matches: word_boundary + function_name + optional whitespace + ( 
            # This helps avoid converting non-function uses like in comments
            pattern = r'\b' + re.escape(old_name) + r'(?=\s*\()'
            content = re.sub(pattern, new_name, content)
        
        # Only write if changes were made
        if content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Updated: {file_path}")
            return True
        else:
            print(f"No changes: {file_path}")
            return False
            
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False

def main():
    """Main conversion function"""
    # Find all relevant files
    patterns = [
        'include/jdksmidi/*.h',
        'src/*.cpp',
        'tests/*.cpp', 
        'examples/*.cpp'
    ]
    
    files_to_process = []
    for pattern in patterns:
        files_to_process.extend(glob.glob(pattern))
    
    print(f"Processing {len(files_to_process)} files...")
    
    updated_count = 0
    total_count = 0
    
    for file_path in files_to_process:
        total_count += 1
        if convert_file(file_path):
            updated_count += 1
    
    print(f"\nConversion complete!")
    print(f"Files processed: {total_count}")
    print(f"Files updated: {updated_count}")
    print(f"Function mappings applied: {len(function_mappings)}")

if __name__ == "__main__":
    main()