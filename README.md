# HICS_Scrambler

A team scrambler for the HKU International Case Society aiming to minimise team member past matches and maximise role allocation based on preferences. 

v2.0 allows for reassigning teams and the options to view, update or delete past matching history.

## Current constraints:
- Only accepts exactly 48 members to form 12 teams of 4
- Only accepts XLSX, CSV or TSV files
- Only accepts 4 specific roles
- File input specific requirements:
    Name Preferences(1 - 4, separated by space)

## Prerequisites:
- CMake installed
- [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/?q=build+tools#build-tools-for-visual-studio-2022)

You can choose to directly run the binary EXE, or build from source.

## Build from source:

As v1.1 requires OpenXLSX for .xlsx support, download the OpenXLSX file here: 
https://github.com/troldal/OpenXLSX/tree/5723411d47643ce3b5b9994064c26ca8cd841f13/OpenXLSX

In your `/path/to/HICS_Scrambler/`, place the OpenXLSX file inside.

If you haven't already, from your Visual Studio Installer, choose "Desktop development with C++" as shown below:

<img width="1710" height="572" alt="image" src="https://github.com/user-attachments/assets/cdd490b1-d929-46e1-9df5-d5d024356800" />
<br/><br/>

Open up the Developer Command Prompt for VS from your search toolbar (run as administrator), and run the following commands:
```
cd /path/to/HICS_Scrambler/
mkdir build
cd build
```
```
cmake ..
cmake --build . --config Release
```

Your program should be ready in `/path/to/HICS_Scrambler/build/Release/Scrambler.exe`.

## Algorithm
The team scrambling algorithm is [simulated annealing](https://www.baeldung.com/cs/simulated-annealing):

First, all 48 members are randomly assigned to 12 teams.

Then, two random members from different teams are chosen and based on the members' history with past matches within their teams, a penalty is calculated. The more past matches, the higher the penalty.

Afterwards, the two members are swapped and new penalty is calculated with the swapped members in new teams. 

If the change in penalty is negative, this means that the swapped state is a better solution and thus it is kept. However, if the change in penalty is positive, according to simulated annealing the new solution is kept or discarded based on probability.

This process is repeated for a number of times (base code default is 20000 iterations). 

After the teams are created, members within the teams are assigned roles based on their preferences (1 being the highest). This is done by brute-forcing all 24 permutations of different roles and choosing the solution with maximum satisfaction. 

