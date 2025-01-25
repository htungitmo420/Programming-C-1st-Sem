// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <utility>

struct Ship {
    int size;
    bool isVertical;
    int startX;
    int startY;
    bool isKill;

    Ship(int s, bool vertical, int x, int y)
        : size(s), isVertical(vertical), startX(x), startY(y), isKill(false) {}
};

class Player {
private:
    int width, height;
    std::vector<std::vector<char>> grid; // 'O' = water, 'S' = ship, 'X' = hit, '.' = miss
    std::vector<Ship> ships;

public:
    Player(int width, int height);
    bool placeShip(int size, bool isVertical, int x, int y);
    std::string takeShot(int x, int y);
    bool hasLost() const;
};

#endif // PLAYER_H