#include "WindowTime.cpp"

using namespace std;

int main(int argc, char const *argv[]) {
    Argument(argc, argv);
    ReadFile(input_file);
    Find5XX();
    if (output_mode == 1) {
        Print5XXToConsole(); 
    } else if (output_mode == 2) {
        Print5XXtoFile(output_file);
    }
    if (stat != 0) {
        FindMostFrequentRequest(input_file, stat);
    }
    if (window_duration != 0) {
        WindowTime(input_file, window_duration, from_time, to_time);
    }
    return 0;
}