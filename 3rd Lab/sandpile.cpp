#include "sandpile.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

SandpileModel::SandpileModel() : width(0), height(0), stable(false) {
    for (int i = 0; i < 256; ++i)
        for (int j = 0; j < 256; ++j)
            grid[i][j] = 0;
}

void SandpileModel::initialize(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    width = 0;
    height = 0;
    while (std::getline(file, line)) {
        int x, y, grains;
        std::istringstream iss(line);
        iss >> x >> y >> grains;
        grid[y][x] = grains;
        width = std::max(width, x + 1);   // Update max width
        height = std::max(height, y + 1); // Update max height
        //cout << "Input: " << x << " " << y << " " << grains << endl;
    }
}


void SandpileModel::update() {
    //cout << "updating" << endl;
    stable = true;
    int const maxGridSize = 256;
    int tempGrid[maxGridSize + 1][maxGridSize + 1] = {0};
    for (int y = 0; y <= height; ++y) {
        for (int x = 0; x <= width; ++x) {
            tempGrid[y][x] = 0;
        }
    }
    for (int y = 1; y <= height; ++y) {
        for (int x = 1; x <= width; ++x) {
            tempGrid[y][x] = grid[y - 1][x - 1];
        }
    }
    bool extendH = false;
    bool extendW = false;
    for (int y = 1; y <= height; ++y) {
        for (int x = 1; x <= width; ++x) {
            if (grid[y - 1][x - 1] >= 4) {
                tempGrid[y][x] -= 4;
                if (x > 1) {
                    tempGrid[y][x - 1]++;
                } else if (width < maxGridSize) {
                    extendW = true;
                    tempGrid[y][x - 1]++;
                }

                if (x < width) {
                    tempGrid[y][x + 1]++;
                } else if (width < maxGridSize) {
                    tempGrid[y][x + 1]++;
                    width++;
                }

                if (y > 1) {
                    tempGrid[y - 1][x]++;
                } else if (height < maxGridSize) {
                    extendH = true;
                    tempGrid[y - 1][x]++;
                }

                if (y < height) {
                    tempGrid[y + 1][x]++;
                } else if (height < maxGridSize) {
                    tempGrid[y + 1][x]++;
                    height++;
                }

                stable = false;
            }
        }
    }

    //cout << "updated" << endl;
    for (int y = 0; y < height + extendH; ++y) {
        for (int x = 0; x < width + extendW; ++x) {
            grid[y][x] = tempGrid[y + !extendH][x + !extendW];
            //cout << grid[y][x] << " ";
        }
        //cout << endl;
    }
    //cout << extendW << " " << extendH << endl;*/
    width += extendW;
    height += extendH;
}


bool SandpileModel::isStable() const {
    return stable;
}
