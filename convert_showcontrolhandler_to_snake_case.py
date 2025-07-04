#!/usr/bin/env python3

import re
import os
import glob

# Function mappings for showcontrolhandler.h conversion
showcontrol_handler_mappings = {
    # MIDISCHandle class methods
    "Dispatch": "dispatch",
    "Go": "go",
    "Stop": "stop", 
    "Resume": "resume",
    "TimedGo": "timed_go",
    "Load": "load",
    "Set": "set",
    "Fire": "fire",
    "AllOff": "all_off",
    "Restore": "restore",
    # "reset" is already snake_case
    "GoOff": "go_off",
    "GoJam": "go_jam",
    "StandbyPlus": "standby_plus",
    "StandbyMinus": "standby_minus",
    "SequencePlus": "sequence_plus",
    "SequenceMinus": "sequence_minus",
    "StartClock": "start_clock",
    "StopClock": "stop_clock",
    "ZeroClock": "zero_clock",
    "SetClock": "set_clock",
    "MTCChaseOn": "mtc_chase_on",
    "MTCChaseOff": "mtc_chase_off",
    "OpenQList": "open_q_list",
    "CloseQList": "close_q_list",
    "OpenQPath": "open_q_path",
    "CloseQPath": "close_q_path",
}

def convert_file(file_path):
    """Convert function names in a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Apply function conversions
        for old_name, new_name in showcontrol_handler_mappings.items():
            # Pattern matches: word_boundary + function_name + ( 
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
    print(f"Function mappings applied: {len(showcontrol_handler_mappings)}")

if __name__ == "__main__":
    main()