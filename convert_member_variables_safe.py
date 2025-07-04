#!/usr/bin/env python3

import re
import os
import glob

# Safe member variable conversions - organized by file to avoid conflicts
file_specific_mappings = {
    # Header files - member variable declarations
    'include/jdksmidi/driverdump.h': {
        'f': '_f'
    },
    'include/jdksmidi/driverwin32.h': {
        'dest_window': '_dest_window',
        'window_msg': '_window_msg', 
        'wparam_value': '_wparam_value',
        'en': '_en',
        'in_handle': '_in_handle',
        'out_handle': '_out_handle',
        'timer_id': '_timer_id',
        'timer_res': '_timer_res',
        'in_open': '_in_open',
        'out_open': '_out_open',
        'timer_open': '_timer_open'
    },
    'include/jdksmidi/driver.h': {
        'in_queue': '_in_queue',
        'out_queue': '_out_queue',
        'in_proc': '_in_proc',
        'out_proc': '_out_proc', 
        'thru_proc': '_thru_proc',
        'thru_enable': '_thru_enable',
        'tick_proc': '_tick_proc',
        'out_matrix': '_out_matrix'
    },
    'include/jdksmidi/edittrack.h': {
        'matrix': '_matrix',
        'track': '_track'
    },
    'include/jdksmidi/fileread.h': {
        'f': '_f',
        'no_merge': '_no_merge',
        'cur_time': '_cur_time',
        'skip_init': '_skip_init',
        'to_be_read': '_to_be_read',
        'cur_track': '_cur_track',
        'abort_parse': '_abort_parse',
        'message_buffer': '_message_buffer',
        'msg_index': '_msg_index',
        'header_format': '_header_format',
        'header_ntrks': '_header_ntrks',
        'header_division': '_header_division',
        'input_stream': '_input_stream',
        'event_handler': '_event_handler'
    },
    'include/jdksmidi/filereadmultitrack.h': {
        'multitrack': '_multitrack',
        'cur_track': '_cur_track',
        'the_format': '_the_format',
        'num_tracks': '_num_tracks',
        'division': '_division'
    },
    'include/jdksmidi/fileshow.h': {
        'out': '_out',
        'division': '_division'
    },
    'include/jdksmidi/filewrite.h': {
        'f': '_f',
        'error': '_error',
        'within_track': '_within_track',
        'file_length': '_file_length',
        'track_length': '_track_length',
        'track_time': '_track_time',
        'track_position': '_track_position',
        'running_status': '_running_status',
        'out_stream': '_out_stream'
    },
    'include/jdksmidi/filewritemultitrack.h': {
        'multitrack': '_multitrack',
        'writer': '_writer'
    },
    'include/jdksmidi/keysig.h': {
        'state': '_state',
        'use_sharps': '_use_sharps',
        'sharp_flat': '_sharp_flat',
        'major': '_major'
    },
    'include/jdksmidi/manager.h': {
        'driver': '_driver',
        'sequencer': '_sequencer',
        'sys_time_offset': '_sys_time_offset',
        'seq_time_offset': '_seq_time_offset',
        'play_mode': '_play_mode',
        'stop_mode': '_stop_mode',
        'notifier': '_notifier',
        'repeat_play_mode': '_repeat_play_mode',
        'repeat_start_measure': '_repeat_start_measure',
        'repeat_end_measure': '_repeat_end_measure'
    },
    'include/jdksmidi/matrix.h': {
        'note_on_count': '_note_on_count',
        'channel_count': '_channel_count',
        'hold_pedal': '_hold_pedal',
        'total_count': '_total_count'
    },
    'include/jdksmidi/msg.h': {
        'status': '_status',
        'byte1': '_byte1',
        'byte2': '_byte2',
        'byte3': '_byte3',
        'time': '_time',
        'dtime': '_dtime'
    },
    'include/jdksmidi/multitrack.h': {
        'tracks': '_tracks',
        'num_tracks': '_num_tracks',
        'deletable': '_deletable',
        'clks_per_beat': '_clks_per_beat',
        'multitrack': '_multitrack',
        'state': '_state'
    },
    'include/jdksmidi/parser.h': {
        'tmp_msg': '_tmp_msg',
        'sysex': '_sysex',
        'state': '_state'
    },
    'include/jdksmidi/process.h': {
        'processors': '_processors',
        'num_processors': '_num_processors',
        'trans_amount': '_trans_amount',
        'rechan_map': '_rechan_map'
    },
    'include/jdksmidi/queue.h': {
        'buf': '_buf',
        'bufsize': '_bufsize',
        'next_in': '_next_in',
        'next_out': '_next_out'
    },
    'include/jdksmidi/sequencer.h': {
        'bits': '_bits',
        'f': '_f',
        'en': '_en',
        'seq': '_seq',
        'track_num': '_track_num',
        'notifier': '_notifier',
        'beat_marker_msg': '_beat_marker_msg',
        'solo_mode': '_solo_mode',
        'tempo_scale': '_tempo_scale',
        'num_tracks': '_num_tracks',
        'track_processors': '_track_processors',
        'state': '_state'
    },
    'include/jdksmidi/showcontrol.h': {
        'v1': '_v1',
        'v2': '_v2', 
        'v3': '_v3',
        'num_values': '_num_values',
        'device_id': '_device_id',
        'command_fmt': '_command_fmt',
        'command': '_command',
        'has_time': '_has_time',
        'has_q_number': '_has_q_number',
        'has_q_list': '_has_q_list',
        'has_q_path': '_has_q_path',
        'hours': '_hours',
        'minutes': '_minutes',
        'seconds': '_seconds',
        'frames': '_frames',
        'fract_frames': '_fract_frames',
        'q_number': '_q_number',
        'q_list': '_q_list',
        'q_path': '_q_path',
        'val1': '_val1',
        'val2': '_val2'
    },
    'include/jdksmidi/smpte.h': {
        'smpte_rate': '_smpte_rate',
        'sample_rate': '_sample_rate',
        'sample_number': '_sample_number',
        'hours': '_hours',
        'minutes': '_minutes',
        'seconds': '_seconds',
        'frames': '_frames',
        'sub_frames': '_sub_frames',
        'sample_number_dirty': '_sample_number_dirty'
    },
    'include/jdksmidi/song.h': {
        'track': '_track',
        'seq': '_seq',
        'title': '_title'
    },
    'include/jdksmidi/sysex.h': {
        'buffer': '_buffer',
        'max_len': '_max_len',
        'chk_sum': '_chk_sum'
    },
    'include/jdksmidi/tempo.h': {
        'tempo': '_tempo'
    },
    'include/jdksmidi/track.h': {
        'buf': '_buf',
        'chunk': '_chunk',
        'buf_size': '_buf_size',
        'num_events': '_num_events'
    }
}

def convert_file(file_path):
    """Convert member variables in a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        changes_made = []
        
        # Check if this file has specific mappings
        if file_path in file_specific_mappings:
            mappings = file_specific_mappings[file_path]
            
            for old_name, new_name in mappings.items():
                # Pattern for member variable declarations (in class definitions)
                # Matches: type var_name; or type var_name[size];
                pattern1 = rf'\b{re.escape(old_name)}\b(?=\s*[;\[])'
                
                # Pattern for member access and assignments
                pattern2 = rf'\b{re.escape(old_name)}\b(?=\s*[=\.])'
                
                # Pattern for constructor initializer lists
                pattern3 = rf'(?<=:\s){re.escape(old_name)}(?=\s*\()'
                pattern4 = rf'(?<=,\s){re.escape(old_name)}(?=\s*\()'
                
                # Apply all patterns
                for pattern in [pattern1, pattern2, pattern3, pattern4]:
                    new_content = re.sub(pattern, new_name, content)
                    if new_content != content:
                        if old_name not in [change[0] for change in changes_made]:
                            changes_made.append((old_name, new_name))
                        content = new_content
        
        # For source files, look for any underscore-prefixed variables to update
        if file_path.endswith('.cpp'):
            # Get all mappings from all header files
            all_mappings = {}
            for file_mappings in file_specific_mappings.values():
                all_mappings.update(file_mappings)
            
            for old_name, new_name in all_mappings.items():
                # Pattern for member access in source files
                patterns = [
                    rf'\b{re.escape(old_name)}\b(?=\s*[=;,\)\]\}\s\.])',
                    rf'(?<=:\s){re.escape(old_name)}(?=\s*\()',
                    rf'(?<=,\s){re.escape(old_name)}(?=\s*\()',
                ]
                
                for pattern in patterns:
                    new_content = re.sub(pattern, new_name, content)
                    if new_content != content:
                        if old_name not in [change[0] for change in changes_made]:
                            changes_made.append((old_name, new_name))
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
    print(f"Files with specific mappings: {len(file_specific_mappings)}")
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

if __name__ == "__main__":
    main()