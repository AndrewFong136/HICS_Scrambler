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

As > v1.1 requires OpenXLSX for .xlsx support, download the OpenXLSX file here: 
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

There are test files in the `test` folder, make sure to move them to the same folder the binary executable resides in. 

## Algorithm
The team scrambling algorithm is [simulated annealing](https://www.baeldung.com/cs/simulated-annealing):

First, all 48 members are randomly assigned to 12 teams and members within the teams are assigned roles based on their preferences (1 being the highest).

Then, two random members from the total 48 members are chosen to be swapped. 

If the two members are from the same team, that means only their roles are swapped. The change in preferences after assigned new roles is calculated as a cost. The lower the cost, the more desirable the outcome. 

If the two members are from different teams, then not only is the preference cost calculated, but also based on the members' history with past matches within their new teams, a penalty is calculated and added to the total cost. The more past matches, the higher the penalty. 

If the total cost is negative, this means that the swapped state is a better solution and thus it is kept. However, if it is positive, according to simulated annealing the new solution is kept or discarded based on probability.

This process is repeated for a number of times (base code default is 200000 iterations). 

_Note: prior to **v2.01**, the algorithm calculated the preferences and past matches separately. The newest update accounts for both factors when conducting simulated annealing._

