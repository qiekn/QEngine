#!/usr/bin/env python3

import sys
import os
import subprocess
import argparse

def create_gdb_commands(run_mode=True, core_file=None):
    """Create a temporary GDB command file."""
    cmd_file = ".gdb_commands.txt"
    
    with open(cmd_file, "w") as f:
        f.write("set print elements 0\n")  
        f.write("set print array on\n")
        f.write("set print pretty on\n")
        
        if run_mode:
            f.write("run\n")
        elif core_file:
            f.write(f"core-file {core_file}\n")
        
        f.write("backtrace full\n")
        f.write("info locals\n")
        f.write("frame 0\n")
        f.write("disassemble\n")
        f.write("info registers\n")
        
        f.write("print Zeytin::get().is_scene_ready()\n")
        f.write("print Zeytin::get().is_play_mode()\n")
        
        f.write("print m_running\n")
        f.write("print m_initialized\n")
        
        f.write("echo \nDebug session ready. Type 'q' to quit or continue debugging.\n")
    
    return cmd_file

def main():
    parser = argparse.ArgumentParser(description="Debug Zeytin with GDB")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--run", action="store_true", help="Run the program under GDB")
    group.add_argument("--core", metavar="CORE_FILE", help="Analyze a core dump file")
    
    args = parser.parse_args()
    
    binary_path = "runtime/bin/EDITOR_MODE/Zeytin"
    if not os.path.exists(binary_path):
        binary_path = "./bin/EDITOR_MODE/Zeytin"
        if not os.path.exists(binary_path):
            print("Error: Zeytin binary not found")
            return 1
    
    cmd_file = create_gdb_commands(args.run, args.core)
    
    gdb_cmd = ["gdb", "-x", cmd_file, binary_path]
    
    try:
        process = subprocess.Popen(gdb_cmd)
        process.wait()
    except KeyboardInterrupt:
        process.terminate()
    finally:
        if os.path.exists(cmd_file):
            os.remove(cmd_file)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
