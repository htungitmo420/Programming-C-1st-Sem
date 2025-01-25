#include <iostream>
#include <string>
#include <fstream> // ifstream, ofstream
#include <vector> //vector
#include <sstream> // istringstream
#include <regex> // regex
#include <ctime> // time_t
#include "Argument.cpp"

using namespace std;

struct Log {
    string host;
    time_t time;
    string timezone;
    string method;
    string url;
    string protocol;
    int statusCode;
    int contentLength;
};


vector<Log> logs; // vector chua cac log (Bien toan cuc)
vector<Log> logs5XX; // vector chua cac log 5XX (Bien toan cuc)

time_t TimeToUNIX(const std::string &timestamp) {
    
    std::tm timeInfo = {};
    
    std::istringstream ss(timestamp);
    ss >> get_time(&timeInfo, "%d/%b/%Y:%H:%M:%S"); // Example format: 03/Jul/1995:10:49:40
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse time");
    }
    
    // Convert to time_t (Unix timestamp)
    return std::mktime(&timeInfo);

}

void ReadFile(string file_name) {
    ifstream file(file_name);
    if (!file.is_open()) {
        cerr << "error! " << file_name << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        // Assuming log format: host timestamp method url protocol statusCode contentLength
        std::regex log_pattern(R"((\S+) \S+ \S+ \[(\d{2}/\w{3}/\d{4}:\d{2}:\d{2}:\d{2}) ([+-]\d{4})\] \"(\S+) (\S+) (\S+)\" (\d+) (\d+))");
        smatch match;
        if (regex_match(line, match, log_pattern)) {
            Log log;
            log.host = match[1];
            log.time = TimeToUNIX(match[2]);
            log.method = match[4];
            log.url = match[5];
            log.protocol = match[6];
            log.statusCode = stoi(match[7]);
            log.contentLength = stoi(match[8]);
            logs.push_back(log);

        }
    }
    if (from_time == 0) {
        from_time = logs[0].time;
    }
    if (to_time == 0) {
        to_time = logs[logs.size() - 1].time;
    }
    file.close();
}

void Find5XX(){
    for (const auto& log : logs) {
        if (log.statusCode >= 500 && log.statusCode < 600) {
            logs5XX.push_back(log);
        }
    }
}

void Print5XXtoFile(string output_file) {
    ofstream file(output_file);
    if (!file.is_open()) {
        cerr << "error! " << output_file << endl;
        return;
    }
    for (const auto& log : logs5XX) {
        file << log.host << " " << log.time << " " << log.timezone << " " << log.method << " " << log.url << " " << log.protocol << " " << log.statusCode << " " << log.contentLength << endl;
    }

    file.close();
}

void Print5XXToConsole() {
    for (const auto& log : logs5XX) {
        cout << log.host << " " << log.time << " " << log.timezone << " " << log.method << " " << log.url << " " << log.protocol << " " << log.statusCode << " " << log.contentLength << endl;
    }
}