#!/usr/bin/env python3

import re
import os
import glob

# Comprehensive mapping of member variables that need underscore prefixes
# Format: "variable_name": "class_context" (for disambiguation)
member_variable_mappings = {
    # driverdump.h - MIDIDriverDump
    "f": ["MIDIDriverDump", "MIDIFileReadStreamFile", "MIDIFileWriteStreamFile", "MIDISequencerGUIEventNotifierText"],
    
    # driverwin32.h - MIDISequencerGUIEventNotifierWin32
    "dest_window": ["MIDISequencerGUIEventNotifierWin32"],
    "window_msg": ["MIDISequencerGUIEventNotifierWin32"],
    "wparam_value": ["MIDISequencerGUIEventNotifierWin32"],
    "en": ["MIDISequencerGUIEventNotifierWin32", "MIDISequencerGUIEventNotifierText"],
    
    # driverwin32.h - MIDIDriverWin32
    "in_handle": ["MIDIDriverWin32"],
    "out_handle": ["MIDIDriverWin32"],
    "timer_id": ["MIDIDriverWin32"],
    "timer_res": ["MIDIDriverWin32"],
    "in_open": ["MIDIDriverWin32"],
    "out_open": ["MIDIDriverWin32"],
    "timer_open": ["MIDIDriverWin32"],
    
    # driver.h - MIDIDriver
    "in_queue": ["MIDIDriver"],
    "out_queue": ["MIDIDriver"],
    "in_proc": ["MIDIDriver"],
    "out_proc": ["MIDIDriver"],
    "thru_proc": ["MIDIDriver"],
    "thru_enable": ["MIDIDriver"],
    "tick_proc": ["MIDIDriver"],
    "out_matrix": ["MIDIDriver"],
    
    # edittrack.h - MIDIEditTrack
    "matrix": ["MIDIEditTrack"],
    "track": ["MIDIEditTrack", "MIDISong"],
    
    # fileread.h - MIDIFileRead
    "no_merge": ["MIDIFileRead"],
    "cur_time": ["MIDIFileRead"],
    "skip_init": ["MIDIFileRead"],
    "to_be_read": ["MIDIFileRead"],
    "cur_track": ["MIDIFileRead", "MIDIFileReadMultiTrack"],
    "abort_parse": ["MIDIFileRead"],
    "message_buffer": ["MIDIFileRead"],
    "msg_index": ["MIDIFileRead"],
    "header_format": ["MIDIFileRead"],
    "header_ntrks": ["MIDIFileRead"],
    "header_division": ["MIDIFileRead"],
    "input_stream": ["MIDIFileRead"],
    "event_handler": ["MIDIFileRead"],
    
    # filereadmultitrack.h - MIDIFileReadMultiTrack
    "multitrack": ["MIDIFileReadMultiTrack", "MIDIFileWriteMultiTrack", "MIDIMultiTrackIterator"],
    "the_format": ["MIDIFileReadMultiTrack"],
    "num_tracks": ["MIDIFileReadMultiTrack", "MIDISequencer", "MIDIMultiTrack"],
    "division": ["MIDIFileReadMultiTrack", "MIDIFileShow"],
    
    # fileshow.h - MIDIFileShow
    "out": ["MIDIFileShow"],
    
    # filewrite.h - MIDIFileWrite
    "error": ["MIDIFileWrite"],
    "within_track": ["MIDIFileWrite"],
    "file_length": ["MIDIFileWrite"],
    "track_length": ["MIDIFileWrite"],
    "track_time": ["MIDIFileWrite"],
    "track_position": ["MIDIFileWrite"],
    "running_status": ["MIDIFileWrite"],
    "out_stream": ["MIDIFileWrite"],
    
    # filewritemultitrack.h - MIDIFileWriteMultiTrack
    "writer": ["MIDIFileWriteMultiTrack"],
    
    # keysig.h - MIDIKeySignature
    "state": ["MIDIKeySignature", "MIDIParser", "MIDIMultiTrackIterator", "MIDISequencer"],
    "use_sharps": ["MIDIKeySignature"],
    "sharp_flat": ["MIDIKeySignature"],
    "major": ["MIDIKeySignature"],
    
    # manager.h - MIDIManager
    "driver": ["MIDIManager"],
    "sequencer": ["MIDIManager"],
    "sys_time_offset": ["MIDIManager"],
    "seq_time_offset": ["MIDIManager"],
    "play_mode": ["MIDIManager"],
    "stop_mode": ["MIDIManager"],
    "notifier": ["MIDIManager", "MIDISequencerTrackNotifier"],
    "repeat_play_mode": ["MIDIManager"],
    "repeat_start_measure": ["MIDIManager"],
    "repeat_end_measure": ["MIDIManager"],
    
    # matrix.h - MIDIMatrix
    "note_on_count": ["MIDIMatrix"],
    "channel_count": ["MIDIMatrix"],
    "hold_pedal": ["MIDIMatrix"],
    "total_count": ["MIDIMatrix"],
    
    # msg.h - MIDIMessage family
    "status": ["MIDIMessage"],
    "byte1": ["MIDIMessage"],
    "byte2": ["MIDIMessage"],
    "byte3": ["MIDIMessage"],
    "time": ["MIDITimedMessage", "MIDITimedBigMessage"],
    "dtime": ["MIDIDeltaTimedMessage", "MIDIDeltaTimedBigMessage"],
    
    # multitrack.h - MIDIMultiTrack
    "tracks": ["MIDIMultiTrack"],
    "deletable": ["MIDIMultiTrack"],
    "clks_per_beat": ["MIDIMultiTrack"],
    
    # parser.h - MIDIParser
    "tmp_msg": ["MIDIParser"],
    "sysex": ["MIDIParser"],
    
    # process.h - processor classes
    "processors": ["MIDIMultiProcessor"],
    "num_processors": ["MIDIMultiProcessor"],
    "trans_amount": ["MIDIProcessorTransposer"],
    "rechan_map": ["MIDIProcessorRechannelizer"],
    
    # queue.h - MIDIQueue
    "buf": ["MIDIQueue", "MIDITrackChunk"],
    "bufsize": ["MIDIQueue"],
    "next_in": ["MIDIQueue"],
    "next_out": ["MIDIQueue"],
    
    # sequencer.h - sequencer classes
    "bits": ["MIDISequencerGUIEvent"],
    "seq": ["MIDISequencerTrackNotifier", "MIDISong"],
    "track_num": ["MIDISequencerTrackNotifier"],
    "beat_marker_msg": ["MIDISequencer"],
    "solo_mode": ["MIDISequencer"],
    "tempo_scale": ["MIDISequencer"],
    "track_processors": ["MIDISequencer"],
    
    # showcontrol.h - show control classes
    "v1": ["MIDICue"],
    "v2": ["MIDICue"],
    "v3": ["MIDICue"],
    "num_values": ["MIDICue"],
    "device_id": ["MIDIShowControlPacket"],
    "command_fmt": ["MIDIShowControlPacket"],
    "command": ["MIDIShowControlPacket"],
    "has_time": ["MIDIShowControlPacket"],
    "has_q_number": ["MIDIShowControlPacket"],
    "has_q_list": ["MIDIShowControlPacket"],
    "has_q_path": ["MIDIShowControlPacket"],
    "hours": ["MIDIShowControlPacket", "SMPTE"],
    "minutes": ["MIDIShowControlPacket", "SMPTE"],
    "seconds": ["MIDIShowControlPacket", "SMPTE"],
    "frames": ["MIDIShowControlPacket", "SMPTE"],
    "fract_frames": ["MIDIShowControlPacket"],
    "q_number": ["MIDIShowControlPacket"],
    "q_list": ["MIDIShowControlPacket"],
    "q_path": ["MIDIShowControlPacket"],
    "val1": ["MIDIShowControlPacket"],
    "val2": ["MIDIShowControlPacket"],
    
    # smpte.h - SMPTE
    "smpte_rate": ["SMPTE"],
    "sample_rate": ["SMPTE"],
    "sample_number": ["SMPTE"],
    "sub_frames": ["SMPTE"],
    "sample_number_dirty": ["SMPTE"],
    
    # song.h - MIDISong
    "title": ["MIDISong"],
    
    # sysex.h - MIDISystemExclusive
    "buffer": ["MIDISystemExclusive"],
    "max_len": ["MIDISystemExclusive"],
    "chk_sum": ["MIDISystemExclusive"],
    
    # tempo.h - MIDITempo
    "tempo": ["MIDITempo"],
    
    # track.h - track classes
    "buf_size": ["MIDITrack"],
    "num_events": ["MIDITrack"],
    "chunk": ["MIDITrack"],
}

def get_class_context_from_file(file_path, line_num):
    """Extract the class name context around a given line number"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
        
        # Look backwards from the line to find class declaration
        for i in range(line_num - 1, max(0, line_num - 50), -1):
            line = lines[i].strip()
            if line.startswith('class ') and '{' in line:
                # Extract class name
                match = re.search(r'class\s+(\w+)', line)
                if match:
                    return match.group(1)
        return None
    except:
        return None

def convert_file(file_path):
    """Convert member variables in a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Track changes for reporting
        changes_made = []
        
        # Apply member variable conversions
        for var_name, class_contexts in member_variable_mappings.items():
            # Create patterns for different usage contexts
            patterns = [
                # Direct member access (this->var or obj.var)
                (rf'\b{re.escape(var_name)}\b(?=\s*[=;,\)\]\}\s])', f'_{var_name}'),
                # Member initialization lists
                (rf'(?<=:\s){re.escape(var_name)}(?=\s*\()', f'_{var_name}'),
                # Member variable declarations (in private:/protected: sections)
                (rf'(?<=\s){re.escape(var_name)}(?=\s*[;\[])', f'_{var_name}'),
                # Assignment targets
                (rf'{re.escape(var_name)}(?=\s*[=])', f'_{var_name}'),
            ]
            
            for pattern, replacement in patterns:
                new_content = re.sub(pattern, replacement, content)
                if new_content != content:
                    changes_made.append((var_name, f'_{var_name}'))
                    content = new_content
        
        # Only write if changes were made
        if content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Updated: {file_path}")
            for old_name, new_name in changes_made:
                print(f"  {old_name} -> {new_name}")
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
    print(f"Member variables to update: {len(member_variable_mappings)}")
    print()
    
    updated_count = 0
    total_count = 0
    
    for file_path in files_to_process:
        total_count += 1
        if convert_file(file_path):
            updated_count += 1
    
    print(f"\nConversion complete!")
    print(f"Files processed: {total_count}")
    print(f"Files updated: {updated_count}")
    print(f"Member variables mapped: {len(member_variable_mappings)}")

if __name__ == "__main__":
    main()