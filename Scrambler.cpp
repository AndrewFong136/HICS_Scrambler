#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <ctime>
#include <array>
#include <cmath>
#include <memory>
#include <sstream>

// OpenXLSX header
#include "OpenXLSX/OpenXLSX.hpp"

using namespace std;
using namespace OpenXLSX;

string teams[12][4];
map<string, array<int, 4>> db;
vector<string> randomised;
unordered_map<string, unordered_set<string>> match_history;

int calculate_penalty(int t, int m1) {
    int penalty = 0;
    for(int i = 0; i < 4; i++){
        if(i != m1) {
            string m2 = teams[t][i];
            if(match_history.find(teams[t][m1]) != match_history.end() && 
               match_history[teams[t][m1]].find(m2) != match_history[teams[t][m1]].end()) {
                penalty++;
            }
        }
    }
    return penalty;
}

int main() {

    cout << "INPUT INSTRUCTIONS:\n";
    cout << "Enter filename along with the file type. Example: test.txt\n";
    cout << "Input Format: Names followed by their ranked preferences (0-4) for positions, and any past matches.\n";
    cout << "Example: Name Situation_Analyst Solutions Kabya's_Lapdog Financials PastMatch1 PastMatch2 ...\n";
    cout << "Type 'END' to finish input.\n\n";

    restart:
    cout << "Enter filename (Supported file types: .xlsx, .csv):\n";

    string filename;
    getline(cin, filename);

    fstream file;
    file.open(filename, ios::in);

    // Set delimiter based on file type
    char delimiter;

    if(filename.find(".xlsx") != string::npos) {
        delimiter = ' ';
        cout << "Processing Excel file...\n";

        XLDocument doc;
        doc.open(filename);
        XLWorksheet worksheet = doc.workbook().sheet(1);

        string outFileName = filename.substr(0, filename.find_last_of('.')) + ".txt";
        ofstream outFile(outFileName);

        // Get the used range
        auto range = worksheet.range();
        int rowCount = range.numRows();
        int colCount = range.numColumns();

        for (int row = 1; row <= rowCount; row++) {
            for (int col = 1; col <= colCount; col++) {
                auto cell = worksheet.cell(row, col);
                if (col > 1) outFile << delimiter; // Add delimiter before each cell except the first
                outFile << cell.value().get<string>();
            }
            outFile << '\n';
        }

        outFile.close();
        doc.close();

        file.open(outFileName, ios::in);
        
    } else if(filename.find(".csv") != string::npos) {
        delimiter = ',';
    } else {
        cerr << "Unsupported file type. Please use .txt or .csv files.\n\n";
        goto restart;
    }

    if(!file.is_open()) {
        cerr << "Error opening file. Please check the filename and try again.\n\n";
        goto restart;
    }

    for(string input; getline(file, input);) {
        if(input == "END") break;
        stringstream ss(input);
        string name;
        if(!(getline(ss, name, delimiter))) continue;
        randomised.push_back(name);

        // Ranked preferences of positions
        array<int, 4> rank;
        for(int i = 0; i < 4; i++) {
            string temp;
            if(!(getline(ss, temp, delimiter))) continue;
            rank[i] = stoi(temp);
        }

        // Match history
        string past;
        while(getline(ss, past, delimiter)) {
            // Hash table of past matches for quick lookup
            match_history[name].insert(past);
            match_history[past].insert(name);
        }
        
        // Store the name and preferences in the database
        db[name] = rank;
    }
    
    file.close();

    //Randomise the names
    random_device rd;
    mt19937 g(rd());
    shuffle(randomised.begin(), randomised.end(), g);

    // Fill the teams with randomised names
    for(int i = 0; i < 48; i++){
        teams[i / 4][i % 4] = randomised[i];
    }

    // Simulated annealing parameters
    double init_temp = 1000.0;
    double cooling_rate = 0.995;
    int iterations = 20000; //adjust as necessary
    int current_penalty = 0;

    // Choose two teams at random and swap members
    uniform_int_distribution<int> team_dist(0, 11);
    uniform_int_distribution<int> member_dist(0, 3);

    for(int itr = 0; itr < iterations; itr++) {
        int t1 = team_dist(g);
        int t2 = team_dist(g);
        while (t1 == t2) t2 = team_dist(g);

        int m1 = member_dist(g);
        int m2 = member_dist(g);
        while (m1 == m2) m2 = member_dist(g);

        int penalty_old = calculate_penalty(t1, m1) + calculate_penalty(t2, m2);

        // Swap members
        swap(teams[t1][m1], teams[t2][m2]);

        int penalty_new = calculate_penalty(t1, m1) + calculate_penalty(t2, m2);

        int delta_penalty = penalty_new - penalty_old;

        double T = init_temp * pow(cooling_rate, itr);
        
        uniform_real_distribution<double> prob_dist(0.0, 1.0);
        double random = prob_dist(g);

        if (delta_penalty < 0 || (delta_penalty >= 0 && exp(-delta_penalty / T) > random)) {
            current_penalty += delta_penalty;
        } else {
            // Swap back if not accepted
            swap(teams[t1][m1], teams[t2][m2]);
        }

        //can add break condition if T < min_temp and unchanging penalty
    }

    //Maps role numbers to roles
    map<int, string> roles_db = {
        {0, "Situation Analyst"},
        {1, "Solutions"},
        {2, "Kabya's Lapdog"},
        {3, "Financials"}
    };

    // Assign roles in teams
    for(int i = 0; i < 12; i++) {
        //rows: team members, columns: positions, values: preferences
        int cost_matrix[4][4];

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                cost_matrix[j][k] = db[teams[i][j]][k];
            }
        }
        
        // Brute force all permutations to find the best assignment
        vector<int> perm = {0, 1, 2, 3};
        vector<int> roles(4);
        int best_sum = INT_MAX;
        do {
            int sum = 0;
            for (int j = 0; j < 4; j++) {
                sum += cost_matrix[j][perm[j]];
            }
            if (sum < best_sum) {
                best_sum = sum;
                roles = perm;
            }
        } while (next_permutation(perm.begin(), perm.end()));

        //Output
        cout << "Team " << i+1 << ":\n";
        for (int j = 0; j < 4; j++) {
            cout << teams[i][j] << ": " << roles_db[roles[j]] <<"\n";
        }
        cout << '\n';
    }

    cout << "Press ENTER to exit...";
    getchar();
}