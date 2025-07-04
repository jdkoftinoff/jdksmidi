#!/usr/bin/env python3

import re
import os
import glob

# Function and member mappings for showcontrol.h conversion
showcontrol_mappings = {
    # MIDICue class methods
    "GetNumValues": "get_num_values",
    "GetV1": "get_v1", 
    "GetV2": "get_v2",
    "GetV3": "get_v3",
    "SetNumValues": "set_num_values",
    "SetV1": "set_v1",
    "SetV2": "set_v2", 
    "SetV3": "set_v3",
    
    # MIDIShowControlPacket getter/setter methods
    "GetDeviceId": "get_device_id",
    "SetDeviceId": "set_device_id",
    "GetCommandFmt": "get_command_fmt",
    "SetCommandFmt": "set_command_fmt", 
    "GetCommand": "get_command",
    "SetCommand": "set_command",
    "GetHasTime": "get_has_time",
    "SetHasTime": "set_has_time",
    "GetHasQNumber": "get_has_q_number",
    "SetHasQNumber": "set_has_q_number",
    "GetHasQList": "get_has_q_list",
    "SetHasQList": "set_has_q_list",
    "GetHasQPath": "get_has_q_path", 
    "SetHasQPath": "set_has_q_path",
    "GetHours": "get_hours",
    "SetHours": "set_hours",
    "GetMinutes": "get_minutes",
    "SetMinutes": "set_minutes",
    "GetSeconds": "get_seconds",
    "SetSeconds": "set_seconds",
    "GetFrames": "get_frames",
    "SetFrames": "set_frames",
    "GetFractFrames": "get_fract_frames",
    "SetFractFrames": "set_fract_frames",
    "GetQNumber": "get_q_number",
    "SetQNumber": "set_q_number",
    "GetQList": "get_q_list",
    "SetQList": "set_q_list",
    "GetQPath": "get_q_path",
    "SetQPath": "set_q_path",
    "GetVal1": "get_val1",
    "SetVal1": "set_val1",
    "GetVal2": "get_val2",
    "SetVal2": "set_val2",
    "GetMacroNum": "get_macro_num",
    "SetMacroNum": "set_macro_num",
    "GetControlNum": "get_control_num",
    "SetControlNum": "set_control_num",
    "GetControlVal": "get_control_val",
    "SetControlVal": "set_control_val",
    
    # Member variables (for direct access and in constructor initializers)
    # Note: these need careful handling to avoid conflicts with local variables
}

# Member variable mappings (these need special handling in showcontrol.h)
member_mappings = {
    "DeviceId": "device_id",
    "CommandFmt": "command_fmt", 
    "Command": "command",
    "HasTime": "has_time",
    "HasQNumber": "has_q_number",
    "HasQList": "has_q_list",
    "HasQPath": "has_q_path",
    "Hours": "hours",
    "Minutes": "minutes", 
    "Seconds": "seconds",
    "Frames": "frames",
    "FractFrames": "fract_frames",
    "QNumber": "q_number",
    "QList": "q_list",
    "QPath": "q_path",
    "Val1": "val1",
    "Val2": "val2",
}

def convert_file(file_path):
    """Convert function names in a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Apply function conversions
        for old_name, new_name in showcontrol_mappings.items():
            # Pattern matches: word_boundary + function_name + ( 
            pattern = r'\b' + re.escape(old_name) + r'(?=\s*\()'
            content = re.sub(pattern, new_name, content)
        
        # Special handling for showcontrol.h member variables
        if file_path.endswith('showcontrol.h'):
            # Convert member variable declarations and references
            for old_member, new_member in member_mappings.items():
                # Convert member variable declarations
                pattern = r'\b' + re.escape(old_member) + r'(?=\s*[;,])'
                content = re.sub(pattern, new_member, content)
                
                # Convert member variable access (return/assignment)
                pattern = r'\breturn\s+' + re.escape(old_member) + r'\b'
                content = re.sub(pattern, f'return {new_member}', content)
                
                pattern = r'\b' + re.escape(old_member) + r'\s*='
                content = re.sub(pattern, f'{new_member} =', content)
        
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
        '../include/jdksmidi/*.h',
        '../src/*.cpp',
        '../tests/*.cpp', 
        '../examples/*.cpp'
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
    print(f"Function mappings applied: {len(showcontrol_mappings)}")
    print(f"Member mappings applied: {len(member_mappings)}")

if __name__ == "__main__":
    main()