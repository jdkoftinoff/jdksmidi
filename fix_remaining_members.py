#!/usr/bin/env python3

import re
import os

# Additional member variable mappings for constructor initializers and direct access
additional_member_fixes = {
    "DeviceId": "device_id",
    "CommandFmt": "command_fmt",
    "Command": "command", 
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

def fix_file(file_path):
    """Fix remaining member variable references"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix direct member variable references (not method calls)
        for old_member, new_member in additional_member_fixes.items():
            # Pattern for direct member access (not followed by parentheses and not preceded by get_/set_)
            pattern = r'(?<!get_)(?<!set_)\b' + re.escape(old_member) + r'(?!\s*\()'
            content = re.sub(pattern, new_member, content)
        
        # Only write if changes were made
        if content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Fixed: {file_path}")
            return True
        else:
            print(f"No changes: {file_path}")
            return False
            
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False

def main():
    """Main fix function"""
    # Fix both the header and implementation files
    files_to_fix = [
        '../include/jdksmidi/showcontrol.h',
        '../src/jdksmidi_showcontrol.cpp'
    ]
    
    print(f"Fixing {len(files_to_fix)} files...")
    
    updated_count = 0
    
    for file_path in files_to_fix:
        if fix_file(file_path):
            updated_count += 1
    
    print(f"\nFix complete!")
    print(f"Files updated: {updated_count}")

if __name__ == "__main__":
    main()