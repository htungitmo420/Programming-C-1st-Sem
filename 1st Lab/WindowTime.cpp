#include <iostream>
#include <string>
#include <cstring> // strncmp
#include <fstream> // ifstream, ofstream
#include <vector>
#include "FindMostFrequentRequest.cpp"

using namespace std;

void WindowTime(string file_name, int window_duration, int from_time, int to_time) {
    ifstream file(file_name);
    int max_req = 0;
    int max_start_time = 0;
    int max_end_time = 0;

    for (int i = 0; i < logs.size(); i++) {
        if (logs[i].time < from_time) {
            continue;
        }
        if (logs[i].time > to_time) {
            break;
        }
        int req = 0;
        for (int j = i; j < logs.size(); j++) {
            if (logs[j].time - logs[i].time > window_duration) {
                break;
            }
            req++;
        }
        if (req > max_req) {
            max_req = req;
            max_start_time = logs[i].time;
            max_end_time = logs[i + req - 1].time;
        }
    }
    cout << "Max requests: " << max_req << " in window [" << max_start_time << ", " << max_end_time << "]" << endl;
}