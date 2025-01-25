#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <algorithm>

struct WordCount {
    char word[1024];
    int count;
};

void convertToLower(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = std::tolower(str[i]);
    }
}

bool compare(WordCount a, WordCount b) {
    return a.count > b.count;
}

int main(int argc, char* argv[]) {
    char* inputPath = nullptr;
    int topN = 0;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--input") == 0) {
            inputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--top") == 0) {
            topN = std::atoi(argv[++i]);
        }
    }
    std::ifstream file(inputPath);

    WordCount words[2001];
    int wordCount = 0;

    char line[1024];

    while (file.getline(line, sizeof(line))) {
        char* token = std::strtok(line, " ");
        while (token) {
            char word[1024];
            std::strcpy(word, token);
            convertToLower(word);

            bool found = false;
            for (int i = 0; i < wordCount; i++) {
                if (std::strcmp(words[i].word, word) == 0) {
                    words[i].count++;
                    found = true;
                    break;
                }
            }

            if (!found) {
                std::strcpy(words[wordCount].word, word);
                words[wordCount].count = 1;
                wordCount++;
            }

            token = std::strtok(nullptr, " ");
        }
    }
    file.close();

    std::sort(words, words + wordCount, compare);

    for (int i = 0; i < topN && i < wordCount; i++) {
        std::cout << words[i].word << " appears " << words[i].count << " time(s) with topN " << topN << std::endl;
    }

    return 0;
}