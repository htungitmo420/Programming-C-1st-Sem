#include <iostream>
#include <string>
#include <cstring> // strncmp
#include <fstream> // ifstream, ofstream
#include <vector>
#include <map> // map
#include <ctime> // time_t
#include "FileProcessing.cpp"

using namespace std;

// Hàm hoán đổi hai phần tử
void Swap(pair<string, int>& a, pair<string, int>& b) {
    pair<string, int> temp = a;
    a = b;
    b = temp;
}

// Hàm sắp xếp vector theo tần suất xuất hiện (giảm dần)
void SortLog(vector<pair<string, int>>& vec) {
    int n = vec.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (vec[j].second < vec[j + 1].second) {
                swap(vec[j], vec[j + 1]);
            }
        }
    }
}

void FindMostFrequentRequest(string file_name, int n) {
    ifstream file(file_name);
    map<string, int> frequency;
    
    for (int i = 0; i < logs5XX.size(); i++) {
        string req = logs5XX[i].host + " " + logs5XX[i].method + " " + logs5XX[i].url + " " + logs5XX[i].protocol;
        frequency[req]++;
    }

    vector<pair<string, int>> freq_vec(frequency.begin(), frequency.end());
    SortLog(freq_vec);

    cout << "Top " << n << " most frequent URLs:" << endl;
    for (int i = 0; i < n && i < freq_vec.size(); i++) {
        cout << freq_vec[i].first << ": " << freq_vec[i].second << " times" << endl;
    }
}