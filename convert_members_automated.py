#!/usr/bin/env python3

import re
import os
import glob
import subprocess

def run_build_test():
    """Run build and return True if successful"""
    try:
        result = subprocess.run(['make'], cwd='build', capture_output=True, text=True)
        return result.returncode == 0
    except:
        return False

def run_unit_tests():
    """Run unit tests and return True if successful"""
    try:
        result = subprocess.run(['ctest'], cwd='build', capture_output=True, text=True)
        return result.returncode == 0
    except:
        return False

def backup_file(file_path):
    """Create a backup of the file"""
    backup_path = file_path + '.backup'
    with open(file_path, 'r') as original:
        with open(backup_path, 'w') as backup:
            backup.write(original.read())
    return backup_path

def restore_file(file_path):
    """Restore file from backup"""
    backup_path = file_path + '.backup'
    if os.path.exists(backup_path):
        with open(backup_path, 'r') as backup:
            with open(file_path, 'w') as original:
                original.write(backup.read())
        os.remove(backup_path)

def find_class_member_variables(file_path):
    """Find all private/protected member variables in a file"""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Find all classes and their member variables
    class_pattern = r'class\s+(\w+).*?\{(.*?)(?=class|\Z)'
    member_vars = {}
    
    for class_match in re.finditer(class_pattern, content, re.DOTALL):
        class_name = class_match.group(1)
        class_body = class_match.group(2)
        
        # Find private and protected sections
        access_pattern = r'(private|protected):\s*(.*?)(?=public:|private:|protected:|\};|\Z)'
        
        vars_for_class = []
        for access_match in re.finditer(access_pattern, class_body, re.DOTALL):
            access_level = access_match.group(1)
            section_content = access_match.group(2)
            
            # Find variable declarations (exclude functions)
            # Match patterns like: type var_name; or type var_name[size];
            var_pattern = r'^\s*(?:(?:const\s+)?(?:static\s+)?(?:volatile\s+)?(?:mutable\s+)?(?:\w+(?:::\w+)*(?:<[^>]*>)?(?:\s*\*)*\s+))(\w+)(?:\[[^\]]*\])?(?:\s*=\s*[^;]+)?;'
            
            for line in section_content.split('\n'):
                var_match = re.match(var_pattern, line.strip())
                if var_match and not re.search(r'\b(?:friend|static|virtual|inline|explicit)\b', line):
                    var_name = var_match.group(1)
                    if not var_name.startswith('_'):  # Don't process already prefixed vars
                        vars_for_class.append(var_name)
        
        if vars_for_class:
            member_vars[class_name] = vars_for_class
    
    return member_vars

def convert_member_variables_in_file(file_path, member_vars_map):
    """Convert member variables in a single file"""
    if not member_vars_map:
        return False
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    original_content = content
    changes_made = []
    
    # Process each class's member variables
    for class_name, var_list in member_vars_map.items():
        for var_name in var_list:
            new_var_name = f'_{var_name}'
            
            # Pattern 1: Member variable declarations
            pattern1 = rf'(\s+{re.escape(var_name)})(\s*[;\[])'
            replacement1 = rf'\1_{var_name}\2'
            new_content = re.sub(pattern1, replacement1, content)
            
            # Pattern 2: Member variable access (this->var, obj.var)
            pattern2 = rf'\b{re.escape(var_name)}\b(?=\s*[=\.\[]|\s*\+\+|\s*--)'
            new_content = re.sub(pattern2, new_var_name, new_content)
            
            # Pattern 3: Constructor initializer lists
            pattern3 = rf'(?<=:\s){re.escape(var_name)}(?=\s*\()'
            new_content = re.sub(pattern3, new_var_name, new_content)
            pattern4 = rf'(?<=,\s){re.escape(var_name)}(?=\s*\()'
            new_content = re.sub(pattern4, new_var_name, new_content)
            
            # Pattern 4: Function parameters and returns  
            pattern5 = rf'\breturn\s+{re.escape(var_name)}\b'
            new_content = re.sub(pattern5, f'return {new_var_name}', new_content)
            
            if new_content != content:
                changes_made.append((var_name, new_var_name))
                content = new_content
    
    if content != original_content:
        with open(file_path, 'w') as f:
            f.write(content)
        return changes_made
    
    return False

def process_files_incrementally():
    """Process files one by one, testing after each"""
    
    # Get all header files first
    header_files = glob.glob('include/jdksmidi/*.h')
    source_files = glob.glob('src/*.cpp')
    
    # Skip tempo.h since we already did it
    header_files = [f for f in header_files if not f.endswith('tempo.h')]
    
    total_processed = 0
    total_updated = 0
    
    print("Processing header files first...")
    
    for file_path in header_files:
        print(f"\nProcessing: {file_path}")
        
        # Find member variables in this file
        member_vars = find_class_member_variables(file_path)
        
        if not member_vars:
            print(f"  No member variables found")
            continue
        
        print(f"  Found classes: {list(member_vars.keys())}")
        for class_name, vars_list in member_vars.items():
            print(f"    {class_name}: {vars_list}")
        
        # Backup the file
        backup_path = backup_file(file_path)
        
        try:
            # Convert member variables
            changes = convert_member_variables_in_file(file_path, member_vars)
            
            if changes:
                print(f"  Applied changes: {changes}")
                
                # Test build
                print("  Testing build...")
                if run_build_test():
                    print("  ✓ Build successful")
                    
                    # Test unit tests
                    print("  Testing unit tests...")
                    if run_unit_tests():
                        print("  ✓ Tests passed")
                        total_updated += 1
                        os.remove(backup_path)  # Remove backup if successful
                    else:
                        print("  ✗ Tests failed - reverting")
                        restore_file(file_path)
                else:
                    print("  ✗ Build failed - reverting")
                    restore_file(file_path)
            else:
                print("  No changes applied")
                os.remove(backup_path)
        
        except Exception as e:
            print(f"  Error: {e} - reverting")
            restore_file(file_path)
        
        total_processed += 1
    
    # Now process source files for any remaining references
    print(f"\nProcessing source files...")
    
    for file_path in source_files:
        print(f"Processing: {file_path}")
        # This would apply any additional patterns needed for source files
        # For now, the header file changes should handle most cases
        total_processed += 1
    
    print(f"\nConversion complete!")
    print(f"Files processed: {total_processed}")
    print(f"Files updated: {total_updated}")

if __name__ == "__main__":
    if not os.path.exists('build'):
        print("Error: build directory not found. Please run from project root.")
        exit(1)
    
    process_files_incrementally()