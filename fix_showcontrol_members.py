#!/usr/bin/env python3

import re
import os

# Member variable mappings for direct member access (not through methods)
member_fixes = {
    "HasTime": "has_time",
    "HasQNumber": "has_q_number", 
    "HasQList": "has_q_list",
    "HasQPath": "has_q_path",
}

def fix_file(file_path):
    """Fix remaining member variable references"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix direct member variable references (not method calls)
        for old_member, new_member in member_fixes.items():
            # Pattern for direct member access (not followed by parentheses)
            pattern = r'\b' + re.escape(old_member) + r'(?!\s*\()'
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
    # Only fix the showcontrol implementation file
    files_to_fix = [
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