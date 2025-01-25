#include <iostream>
#include <string>
#include <fstream> // ifstream, ofstream
#include <vector> //vector
#include <sstream> // istringstream
#include <regex> // regex
#include <ctime> // time_t
#include <iomanip> // get_time

using namespace std;

int output_mode = 1; // 0 = none, 1 = stdout, 2 = file
string output_file = "";
int window_duration = 0; // dat khoang thoi gian cua 1 cua so
int from_time = 0; // unix time
int to_time = 0; // unix time
int stat = 0; // so luong thong ke
string input_file;

int Argument(int argc, char const *argv[]) {
    for (int i = 1; i < argc; i++) {
        // -o --output=
        if (string(argv[i]) == "-o") {
            if (i + 1 < argc) { 
                output_file = string(argv[++i]);
                output_mode = 2;
            } else {
                cerr << "error" << endl;
                return 1;
            }
        } else if (strncmp(argv[i], "--output=", 9) == 0) {
            output_file = string(argv[i]).substr(9);
            output_mode = 2;
        }

        // -p --print
        else if (string(argv[i]) == "-p" || strncmp(argv[i], "--print", 7) == 0) {
            output_mode = 1;
        }

        // -s --stats=
        else if (string(argv[i]) == "-s") {
            if (i + 1 < argc) {
                stat = stoi(argv[++i]);
            } else {
                cerr << "error" << endl;
                return 1;
            }
        } else if (strncmp(argv[i], "--stats=", 8) == 0) {
            stat = stoi(string(argv[i]).substr(8));
        }

        // -w --window=
        else if (string(argv[i]) == "-w") {
            if (i + 1 < argc) {
                window_duration = stoi(argv[++i]);
            } else {
                cerr << "error" << endl;
                return 1;
            }
        } else if (strncmp(argv[i], "--window=", 9) == 0) {
            window_duration = stoi(string(argv[i]).substr(9));
        }

        // -f --from=
        else if (string(argv[i]) == "-f") {
            if (i + 1 < argc) {
                from_time = stoi(argv[++i]);
            } else {
                cerr << "error" << endl;
                return 1;
            }
        } else if (strncmp(argv[i], "--from=", 7) == 0) {
            from_time = stoi(string(argv[i]).substr(7));
        }

        // -e --to=
        else if (string(argv[i]) == "-e") {
            if (i + 1 < argc) {
                to_time = stoi(argv[++i]);
            } else {
                cerr << "error" << endl;
                return 1;
            }
        } else if (strncmp(argv[i], "--to=", 5) == 0) {
            to_time = stoi(string(argv[i]).substr(5));
        } else {
            input_file = string(argv[i]);
        }
    }
}    