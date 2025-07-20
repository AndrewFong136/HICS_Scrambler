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

## Algorithm
The team scrambling algorithm is [simulated annealing](https://www.baeldung.com/cs/simulated-annealing):

First, all 48 members are randomly assigned to 12 teams.

Then, two random members from different teams are chosen and based on the members' history with past matches within their teams, a penalty is calculated. The more past matches, the higher the penalty.

Afterwards, the two members are swapped and new penalty is calculated with the swapped members in new teams. 

If the change in penalty is negative, this means that the swapped state is a better solution and thus it is kept. However, if the change in penalty is positive, according to simulated annealing the new solution is kept or discarded based on probability.

This process is repeated for a number of times (base code default is 20000 iterations). 

After the teams are created, members within the teams are assigned roles based on their preferences (1 being the highest). This is done by brute-forcing all 24 permutations of different roles and choosing the solution with maximum satisfaction. 

