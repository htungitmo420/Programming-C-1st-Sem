#ifndef SANDPILE_H
#define SANDPILE_H

#include <string>

class SandpileModel {
private:
    int grid[256][256];  // Fixed-size grid (256x256) for simplicity
    int width, height;
    bool stable;

public:
    SandpileModel();
    void initialize(const std::string &filename);
    void update();
    bool isStable() const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCell(int x, int y) const { return grid[y][x]; }
};

#endif
