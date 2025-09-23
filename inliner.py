#!/usr/bin/env python3

import sys
import os
import re
import json
import argparse
import shutil
import stat

# --- Installation Logic ---

def install_script():
    """Copies the script to the user's local bin directory."""
    try:
        target_dir = os.path.expanduser('~/.local/bin')
        dest_path = os.path.join(target_dir, 'inliner')
        
        print(f"Attempting to install the script to '{target_dir}'...")
        if not os.path.exists(target_dir):
            print(f"Creating directory: {target_dir}")
            os.makedirs(target_dir)

        script_path = os.path.abspath(sys.argv[0])
        shutil.copy2(script_path, dest_path)
        print(f"✅ Script copied to '{dest_path}'")
        
        current_permissions = os.stat(dest_path).st_mode
        os.chmod(dest_path, current_permissions | stat.S_IXUSR)
        print("✅ Made the script executable.")

        print("\n--- Important Next Step ---")
        print(f"For the 'inliner' command to work, ensure '{target_dir}' is in your shell's PATH.")
        print("You can check this by running: echo $PATH")
        print("If it's not there, add this line to your ~/.bashrc, ~/.zshrc, or ~/.profile:")
        print(f'\n    export PATH="{target_dir}:$PATH"\n')
        print("Restart your terminal or run 'source ~/.bashrc' for changes to take effect.")
        print("\nInstallation complete!")

    except Exception as e:
        print(f"❌ An error occurred during installation: {e}")
        sys.exit(1)

# --- New Recursive Inlining Logic ---
def recursively_inline(header_path, seen_files):
    """
    Recursively reads a header file and all its local #include directives,
    returning a single string with all content flattened.
    """
    abs_header_path = os.path.abspath(header_path)

    if abs_header_path in seen_files:
        return "" 
    seen_files.add(abs_header_path)

    try:
        with open(abs_header_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"    [WARNING] Header not found: '{header_path}'. Skipping.")
        return ""

    processed_lines = []
    header_dir = os.path.dirname(abs_header_path)

    for line in content.splitlines():
        match = re.search(r'^\s*#include "([^"]+)"', line)
        if match:
            included_header_name = match.group(1)
            next_header_path = os.path.join(header_dir, included_header_name)
            
            print(f"        -> Found and processing nested header: '{included_header_name}'")
            inlined_content = recursively_inline(next_header_path, seen_files)
            processed_lines.append(inlined_content)
        else:
            if line.strip() != '#pragma once':
                processed_lines.append(line)
            
    return "\n".join(processed_lines)

# --- Core Application Logic ---

def load_config(config_path):
    """Loads the configuration from a given path or returns defaults."""
    defaults = {
        "output_dir": "build",
        "minify_style": "none",
        "multiline_max_chars": 120
    }
    
    if config_path and os.path.exists(config_path):
        print(f"-> Using configuration from: '{config_path}'")
        try:
            with open(config_path, 'r') as f:
                content = f.read()
                if not content: return defaults
                user_config = json.loads(content)
                defaults.update(user_config)
                return defaults
        except (json.JSONDecodeError, IOError) as e:
            print(f"Warning: Could not read config '{config_path}'. Error: {e}. Using defaults.")
            return defaults
    else:
        print("-> No config file found. Using default settings.")
        return defaults

def find_config_path(config_arg):
    """Searches for the config file in a specific order."""
    if config_arg:
        if os.path.exists(config_arg): return config_arg
        else:
            print(f"Warning: Specified config file '{config_arg}' not found.")
            return None
    
    local_path = 'inliner_config.json'
    global_dir = os.path.expanduser('~/.config/inliner')
    global_path = os.path.join(global_dir, 'config.json')
    
    if os.path.exists(local_path): return local_path
    if os.path.exists(global_path): return global_path
        
    return None

def strip_comments(content):
    """Removes C/C++ style comments from a string."""
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    content = re.sub(r'//.*', '', content)
    return content

def minify_cpp_line(line):
    """Performs aggressive whitespace reduction on a single line of C++ code."""
    minified = re.sub(r'\s+', ' ', line)
    minified = re.sub(r'\s*([;{}(),<>=\[\]])\s*', r'\1', minified)
    return minified

def process_file(input_filepath, config):
    """Reads a source file and recursively inlines marked headers."""
    source_directory = os.path.dirname(os.path.abspath(input_filepath))
    output_dir = config.get("output_dir", "build")
    minify_style = config.get("minify_style", "none")
    max_chars = config.get("multiline_max_chars", 120)

    os.makedirs(output_dir, exist_ok=True)
    
    base_filename = os.path.basename(input_filepath)
    output_filepath = os.path.join(output_dir, base_filename)

    print(f"Processing '{input_filepath}'...")

    try:
        with open(input_filepath, 'r') as infile, open(output_filepath, 'w') as outfile:
            for line in infile:
                stripped_line = line.strip()

                if stripped_line.startswith('#include "') and stripped_line.endswith('//@'):
                    match = re.search(r'#include "([^"]+)"', stripped_line)
                    if not match:
                        outfile.write(line)
                        continue

                    header_name = match.group(1)
                    header_path = os.path.join(source_directory, header_name)
                    
                    print(f"    -> Recursively inlining '{header_name}' with style: '{minify_style}'...")
                    
                    seen_files = set()
                    flattened_content = recursively_inline(header_path, seen_files)
                    
                    content_no_comments = strip_comments(flattened_content)
                    
                    directives, code_lines = [], []
                    for ln in content_no_comments.splitlines():
                        stripped_ln = ln.strip()
                        if not stripped_ln: continue
                        if stripped_ln.startswith('#'): directives.append(stripped_ln)
                        else: code_lines.append(stripped_ln)

                    final_content = "\n".join(directives)
                    
                    if code_lines:
                        if minify_style == "single_line":
                            minified_code = [minify_cpp_line(ln) for ln in code_lines]
                            final_content += "\n" + " ".join(minified_code)
                        elif minify_style == "multiline":
                            wrapped_code, current_line = "", ""
                            minified_code = [minify_cpp_line(ln) for ln in code_lines]
                            for part in minified_code:
                                if not current_line: current_line = part
                                elif len(current_line) + len(part) + 1 <= max_chars: current_line += " " + part
                                else:
                                    wrapped_code += current_line + "\n"
                                    current_line = part
                            if current_line: wrapped_code += current_line
                            final_content += "\n" + wrapped_code
                        else:
                            final_content += "\n" + "\n".join(code_lines)

                    outfile.write(f"// --- Start of inlined file: {header_name} ---\n")
                    outfile.write(final_content.strip() + "\n")
                    outfile.write(f"// --- End of inlined file: {header_name} ---\n")
                else:
                    outfile.write(line)

        print(f"\n✅ Pre-processing complete. Output is in '{output_filepath}'")

    except FileNotFoundError:
        print(f"❌ [ERROR] Input file not found: '{input_filepath}'")
        sys.exit(1)
    except Exception as e:
        print(f"❌ An unexpected error occurred: {e}")
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="A C++ header inliner and minifier.",
        epilog="Example: python inliner.py main.cpp -c project_config.json"
    )
    parser.add_argument('input_file', nargs='?', default=None, help='The C++ source file to process.')
    parser.add_argument('-c', '--config', help='Path to a JSON configuration file.')
    parser.add_argument('--install', action='store_true', help='Install the script to ~/.local/bin for easy access.')
    
    args = parser.parse_args()

    if args.install:
        install_script()
    elif args.input_file:
        config_path = find_config_path(args.config)
        config = load_config(config_path)
        process_file(args.input_file, config)
    else:
        parser.print_help()

