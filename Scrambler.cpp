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
#include <map>
#include <regex>
#include <filesystem>
#include <thread>
#include <chrono>

#include "OpenXLSX/OpenXLSX.hpp"
#include "include/sqlite3.h"
#include "include/art.h"

using namespace std;
using namespace OpenXLSX;
namespace fs = std::filesystem;

map<string, array<int, 4>> preferences;
vector<string> members;
unordered_map<string, unordered_set<string>> match_history;
unordered_map<string, int> roles;
unordered_map<string, int> teams;

string dbPath = "past_matches.db";
sqlite3* pastdb;
char* errMsg = nullptr;
int rc;

double match_penalty(string m) {
    double penalty = 0;
    int t = teams[m];

    for(int k = 0; k < 4; k++){
        string c = members[t * 4 + k];
        if(c != m) {
            if(match_history.find(m) != match_history.end() &&
               match_history[m].find(c) != match_history[m].end()) {
                penalty++;
            }
        }
    }
    return penalty;
}

double calculate_penalty(int i, int j, bool flag) {
    string m1 = members[i];
    string m2 = members[j];
    double old_rank = preferences[m1][roles[m1]] + preferences[m2][roles[m2]];
    double new_rank = preferences[m1][roles[m2]] + preferences[m2][roles[m1]];
    double delta_rank = new_rank - old_rank;

    if(flag) return delta_rank;

    return match_penalty(m1) + match_penalty(m2) + delta_rank;
}

string trim(const string& str) {
    string s = regex_replace(str, regex("^\\s+"), "");
    s = regex_replace(s, regex("\\s+$"), "");
    return s;
}

int callback (void* idk, int col, char** data, char** colName) {
    for (int i = 0; i < col; i+=2) {
        match_history[string(data[i])].insert(string(data[i + 1]));
        match_history[string(data[i + 1])].insert(string(data[i]));
    }
    return 0;
}

void update_db() {
    // Open/Create database
    rc = sqlite3_open("past_matches.db", &pastdb);

    // Create table
    const char* createTable =
        "CREATE TABLE IF NOT EXISTS Members ("
        "name TEXT NOT NULL, "
        "past_matches TEXT);";

    rc = sqlite3_exec(pastdb, createTable, nullptr, nullptr, &errMsg);

    string insertTable;
    stringstream it;
    it <<  "INSERT INTO Members (name, past_matches) VALUES ";
    bool first = true;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++){
            for (int k = j + 1; k < 4; k++){
                if (!first) it << ", ";
                it << "('" << members[i * 4 + j] << "', '" << members[i * 4 + k] << "'), ('" << members[i * 4 + k] << "', '" << members[i * 4 + j] << "')";
                first = false;
            }
        }
    }
    it << ";";

    insertTable = it.str();

    rc = sqlite3_exec(pastdb, insertTable.c_str(), nullptr, nullptr, &errMsg);

    rc = sqlite3_exec(pastdb, "SELECT * FROM Members;", callback, nullptr, &errMsg);

    sqlite3_close(pastdb);
    return;
}

void save() {
    cin.ignore();
    bool flag = true;
    while (1) {
        cout <<"Update matching history for future team scrambling? [Y/n] ";
        string ans;
        getline(cin, ans);
        if (ans == "y" || ans == "Y" || ans == "") {
            update_db();

            cout << "\nMatching history updated.\n\n";
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }
        if (ans == "n" || ans == "N") {
            cout << "\n\n";
            return;
        }

        cerr << "\nType 'y' for 'yes' or 'n' for 'no'.";
        if (flag) {
            cerr << "(It's not that deep .____.)";
        }
        cout << "\n\n";
    }
}

int main() {
    //Sync to match_history for data processing if database exists

    if(fs::exists(dbPath)){
        rc = sqlite3_open(dbPath.c_str(), &pastdb);
        rc = sqlite3_exec(pastdb, "SELECT * FROM Members;", callback, nullptr, &errMsg);
    } else {
        rc = sqlite3_open(dbPath.c_str(), &pastdb);  //else create a new database
    }

    sqlite3_close(pastdb);

    fputs(BANNER_ART, stdout);
    fputs("\n\n", stdout);

    cout << "What would you like to do?\n\n";

    bool first_assign = true, first_input = true;

    loop:
    if (first_assign) cout << "1 -> Assign teams\n";
    else cout << "1 -> Reassign teams\n";
    cout << "2 -> View namelist\n"
            "3 -> View matching history\n"
            "4 -> President's feet\n"
            "5 -> Exit\n\n";

    char input;
    cin >> input;
    cout << '\n';

    if(input > '6' || input < '1') {
        if(first_input){
            cerr << "Really? It's just numbers 1 - 5.\nWell, at least we know who shouldn't be responsible for financials.\n";
            first_input = false;
        }
        cerr << "Try Again.\n\n";
        goto loop;
    }

    switch (input) {
        case '1': {
            if (first_assign) {
                first_assign = false;
                cin.ignore();
                goto assign_;
            }
            goto reassign;
        }
        case '2': {
            cout << "Under construction.\n\n";
            goto loop;
        }
        case '3': {
            if (match_history.empty()) {
                cout << "No matching history.\n\n";
            } else {
                for (auto & i: match_history) {
                    cout << i.first << ": ";
                    for(auto & j: i.second) {
                        cout << j << " ";
                    }
                    cout << "\n\n";
                }
                goto mh_processing;
            }
            goto loop;
        }
        case '4': {
            cout << "Ew, no. Pervert. Admire our president in his glorious, 120kg form instead:\n\n";
            this_thread::sleep_for(chrono::seconds(1));

            fputs(PRESIDENT_ART, stdout);
            fputc('\n', stdout);
            goto loop;
        }
        case '5': {
            return 0;
        }
        case '6': {
            cout << "Congratulations! You found the release notes! Not that you'll be particularly interested in them anyways.\n\n"
                           "v1.0:\n - Team scrambler based on ranked preferences and minimising past matches\n - Support for .txt, and .csv files\n\n"
                           "v1.1:\n - Added support for .xlsx and .tsv files\n - Removed .txt support since google sheets does not have the option to download as .txt\n\n"
                           "v2.0:\n - Separated match history into a separate database\n - Allows the match history to be updated based on newly assigned teams for next team scrambling \n - Cleaner interface\n - Included a little bit of attitude :)\n\n\n"
                           "Press ENTER to go back.\n\n";

            cin.ignore();
            string tmp;
            getline(cin, tmp);
            goto loop;
        }
    }

    assign_: { //first time assign teams

        cout << "INPUT INSTRUCTIONS:\n"
                "Enter filename along with the file type. Example: test.xlsx\n"
                "Input Format: Names followed by their ranked preferences (0-4)\n"
                "Example: Kabya 3 0 1 2\n\n";

        bool first_file = true;

        restart:
        cout << "Enter filename (Supported file types: .xlsx, .csv, .tsv) or type EXIT to go back:\n";

        string filename;
        getline(cin, filename);

        fstream file;
        file.open(filename, ios::in);
        cout << '\n';

        // Set delimiter based on file type
        char delimiter;

        if (filename == "EXIT"){
            first_assign = true;
            goto loop;
        } else if(filename.find(".xlsx") != string::npos) {

            if(!file.is_open()) {
                if (first_file){
                        cerr << "Who let the illiterate neanderthal be HICS exco?\n";
                        first_file = false;
                }
                cerr << "Error opening file. Please check the filename and try again.\n\n";
                goto restart;
            }

            delimiter = ',';
            cout << "Processing Excel file...";

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
                    outFile << cell.value();
                }
                outFile << '\n';
            }

            outFile.close();
            doc.close();
            file.close();
            cout << "DONE\nData extracted to " << outFileName << ".\n\n";

            // Reopen the newly created text file for further processing
            file.open(outFileName, ios::in);

        } else if(filename.find(".csv") != string::npos) {
            delimiter = ',';
        } else if(filename.find(".tsv") != string::npos) {
            delimiter = '\t';
        } else {
                if (first_file){
                        cerr << "Who let the illiterate neanderthal be HICS exco?\n";
                        first_file = false;
                }
            cerr << "Unsupported file type. Please use .xlsx, .csv or .tsv files.\n\n";
            goto restart;
        }

        if(!file.is_open()) {
                if (first_file){
                        cerr << "Who let the illiterate neanderthal be HICS exco?\n";
                        first_file = false;
                }
            cerr << "Error opening file. Please check the filename and try again.\n\n";
            goto restart;
        }

        for(string input; getline(file, input);) {
            stringstream ss(input);
            string name;

            if(!(getline(ss, name, delimiter))) continue;

            name = trim(name);
            if(name.empty()) continue;
            members.push_back(name);


            // Ranked preferences of positions
            array<int, 4> ranks;
            for(int i = 0; i < 4; i++) {
                string temp;
                if(!(getline(ss, temp, delimiter))) continue;
                temp = trim(temp);
                ranks[i] = stoi(temp);
            }

            // Store the name and preferences in the database
            preferences[name] = ranks;
        }

        file.close();
    }

    reassign: {
       //Randomise the names
        random_device rd;
        mt19937 g(rd());
        shuffle(members.begin(), members.end(), g);

        for(int i = 0; i < 48; i++) {
            teams[members[i]] = i / 4;
        }

        for(int i = 0; i < 12; i++){
            // Brute force all permutations to find the best assignment
            vector<int> perm = {0, 1, 2, 3};
            vector<int> best_roles(4);
            int best_sum = INT_MAX;
            do {
                int sum = 0;
                for (int j = 0; j < 4; j++) {
                    sum += preferences[members[i * 4 + j]][perm[j]];
                }
                if (sum < best_sum) {
                    best_sum = sum;
                    best_roles = perm;
                }
            } while (next_permutation(perm.begin(), perm.end()));

            for(int j = 0; j < 4; j++) {
                roles[members[i * 4 + j]] = best_roles[j];
            }
        }

        // Simulated annealing parameters
        double init_temp = 1000.0;
        double cooling_rate = 0.995;
        int iterations = 200000; //adjust as necessary

        // Swap members at random
        uniform_int_distribution<int> member_dist(0, 47);
        double T = init_temp;

        for(int itr = 0; itr < iterations; itr++) {
            int m1 = member_dist(g);
            int m2 = member_dist(g);
            while (m1 == m2) m2 = member_dist(g);

            bool teams_flag = (teams[members[m1]] == teams[members[m2]]);

            double delta = calculate_penalty(m1, m2, teams_flag);

            uniform_real_distribution<double> prob_dist(0.0, 1.0);
            double random = prob_dist(g);

            if (delta < 0 || (delta > 0 && exp(-delta / T) > random)) {
                swap(members[m1], members[m2]);

                //swap roles
                int tmp = roles[members[m1]];
                roles[members[m1]] = roles[members[m2]];
                roles[members[m2]] = tmp;

                if(teams_flag) continue;

                //swap teams
                tmp = teams[members[m1]];
                teams[members[m1]] = teams[members[m2]];
                teams[members[m2]] = tmp;
            }

            T *= cooling_rate;
            //can add break condition if T < min_temp and unchanging penalty
        }

        //Maps role numbers to roles
        map<int, string> roles_db = {
            {0, "Situation Analyst"},
            {1, "Solutions"},
            {2, "Kabya's Lapdog"},
            {3, "Financials"}
        };

        // Output
        for(int i = 0; i < 12; i++) {
            cout << "Team " << i+1 << ":\n";
            for (int j = 0; j < 4; j++) {
                string m = members[i * 4 + j];
                cout << m << ": " << roles_db[roles[m]] <<"\n";
            }
            cout << '\n';
        }
    }

    cout << "Teams have been created. \n\n";

    loop_reassign:

    cout << "1 -> Reassign Teams\n"
            "2 -> Go back\n"
            "3 -> Exit\n\n";

    cin >> input;
    cout << '\n';

    if(input > '3' || input < '1') {
        if(first_input){
            cerr << "Really? It's just numbers 1 - 3.\nWell, at least we know who shouldn't be responsible for financials.\n";
            first_input = false;
        }
        cerr << "Try Again.\n\n";
        goto loop_reassign;
    }

    switch(input) {
        case '1':
            goto reassign;
        case '2': {
            cout << endl;
            save();
            goto loop;
        }
        case '3': {
            cout << endl;
            save();
            return 0;
        }
    }

    mh_processing:

    cout << "1 -> Search for a specific member\n"
            "2 -> Delete the matching history (for database issues)\n"
            "3 -> Go back\n"
            "4 -> Exit\n\n";

    cin >> input;
    cout << '\n';

    if(input > '4' || input < '1') {
        if(first_input){
            cerr << "Really? It's just numbers 1 - 3.\nWell, at least we know who shouldn't be responsible for financials.\n";
            first_input = false;
        }
        cerr << "Try Again.\n\n";
        goto loop_reassign;
    }

    switch(input) {
        case '1': {
            cout << "Under construction.\n\n";
            goto mh_processing;
        }
        case '2': {
            ofstream del(dbPath.c_str(), ios::trunc);
            match_history = {};

            cout << "Database cleared.\n\n";
            del.close();

            goto mh_processing;
        }
        case '3': {
            cout << endl;
            goto loop;
        }
        case '4': {
            cout << endl;
            return 0;
        }
    }
}
