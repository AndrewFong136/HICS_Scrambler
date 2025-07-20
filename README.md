# HICS_Scrambler

A team scrambler for the HKU International Case Society aiming to minimise team member past matches and maximise role allocation based on preferences. 

## Current constraints:
- Only accepts exactly 48 members to form 12 teams of 4
- Only accepts TXT or CSV files
- Only accepts 4 specific roles
- File input specific requirements:
    Name Preferences(1 - 4, separated by space) Past matches(no limit, separated by space)

## Prerequisites:
- MinGW installed

You can choose to directly run the binary EXE, or build from source.

## Build from source:

On your command line, run
```
g++ /path/to/Scrambler.cpp -static -o /path/to/Scrambler.exe
```
