#include "Player.h"

Player::Player(int width, int height) : width(width), height(height), grid(height, std::vector<char>(width, 'O')) {}

bool Player::placeShip(int size, bool isVertical, int x, int y) {
    if ((isVertical && (y + size > height)) || (!isVertical && (x + size > width))) {
        return false;
    }

    for (int i = 0; i < size; ++i) {
        if ((isVertical && grid[y + i][x] != 'O') || (!isVertical && grid[y][x + i] != 'O')) {
            return false;
        }
    }

    for (int i = 0; i < size; ++i) {
        if (isVertical) grid[y + i][x] = 'S';
        else grid[y][x + i] = 'S';
    }
    ships.emplace_back(size, isVertical, x, y);
    return true;
}

std::string Player::takeShot(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height || grid[y][x] == '.' || grid[y][x] == 'X') {
        return "miss";
    }

    if (grid[y][x] == 'S') {
        grid[y][x] = 'X';
        for (auto& ship : ships) {
            if (ship.startX <= x && x < ship.startX + (ship.isVertical ? 1 : ship.size) &&
                ship.startY <= y && y < ship.startY + (ship.isVertical ? ship.size : 1)) {
                bool killed = true;
                for (int i = 0; i < ship.size; ++i) {
                    int tx = ship.isVertical ? ship.startX : ship.startX + i;
                    int ty = ship.isVertical ? ship.startY + i : ship.startY;
                    if (grid[ty][tx] != 'X') {
                        killed = false;
                        break;
                    }
                }
                ship.isKill = killed;
                return killed ? "kill" : "hit";
            }
        }
    }

    grid[y][x] = '.';
    return "miss";
}

bool Player::hasLost() const {
    for (const auto& ship : ships) {
        if (!ship.isKill) return false;
    }
    return true;
}