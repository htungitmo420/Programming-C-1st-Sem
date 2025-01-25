#include "Game.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game& Game::Instance() {
    static Game instance;
    return instance;
}

Game::Game() //default
    : isInitialized(false), isPlaying(false), gameOver(false), won(false),
      boardWidth(0), boardHeight(0), strategy("ordered"), shipCounts(4, 0), ships(),
      lastX(0), lastY(0)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

bool Game::Initialize(const std::string& modeInput) {
    if (modeInput != "master" && modeInput != "slave") {
        std::cout << "Invalid mode. Choose 'master' or 'slave'.\n";
        return false;
    }

    if (boardWidth == 0 || boardHeight == 0) {
        std::cout << "Board width and height must be set before initializing the game.\n";
        return false;
    }

    bool hasAtLeastOneShip = false;
    int totalShipCells = 0;
    for (size_t i = 0; i < shipCounts.size(); ++i) {
        if (shipCounts[i] > 0) hasAtLeastOneShip = true;
        totalShipCells += shipCounts[i] * static_cast<int>(i + 1);
    }

    if (!hasAtLeastOneShip) {
        std::cout << "At least one ship must be set.\n";
        return false;
    }

    if (totalShipCells > static_cast<int>(boardWidth * boardHeight)) {
        std::cout << "Total ship cells exceed board capacity.\n";
        return false;
    }

    grid = std::vector<std::vector<char>>(boardHeight, std::vector<char>(boardWidth, '.'));

    mode = modeInput;
    isInitialized = true;
    return true;
}

void Game::Reset() {
    isPlaying = false;
    gameOver = false;
    won = false;
    ships.clear();
    boardWidth = 0;
    boardHeight = 0;
    strategy = "ordered";
    shipCounts.assign(4, 0);
    isInitialized = false;
    lastX = 0;
    lastY = 0;
}

bool Game::SetWidth(uint64_t width) {
    if (isInitialized || isPlaying) {
        std::cout << "Cannot set width after the game has started.\n";
        return false;
    }
    if (width <= 0) {
        std::cout << "Width must be greater than 0.\n";
        return false;
    }
    boardWidth = width;
    return true;
}

uint64_t Game::GetWidth() const {
    return boardWidth;
}

bool Game::SetHeight(uint64_t height) {
    if (isInitialized || isPlaying) {
        std::cout << "Cannot set height after the game has started.\n";
        return false;
    }
    if (height <= 0) {
        std::cout << "Height must be greater than 0.\n";
        return false;
    }
    boardHeight = height;
    return true;
}

uint64_t Game::GetHeight() const {
    return boardHeight;
}

bool Game::SetShipCount(int type, uint32_t count) {
    if (isInitialized || isPlaying) {
        std::cout << "Cannot set ship counts after the game has started.\n";
        return false;
    }
    if (type < 1 || type > static_cast<int>(shipCounts.size())) {
        std::cout << "Invalid ship type. Must be between 1 and " << shipCounts.size() << ".\n";
        return false;
    }
    if (count == 0) {
        std::cout << "Ship count must be at least 1.\n";
        return false;
    }
    shipCounts[type - 1] = count;
    return true;
}

uint32_t Game::GetShipCount(int type) const {
    if (type < 1 || type > static_cast<int>(shipCounts.size())) {
        return 0;
    }
    return shipCounts[type - 1];
}

bool Game::SetStrategy(const std::string& strategyInput) {
    if (isInitialized || isPlaying) {
        std::cout << "Cannot set strategy after the game has started.\n";
        return false;
    }
    if (strategyInput != "ordered" && strategyInput != "custom") {
        std::cout << "Invalid strategy. Choose 'ordered' or 'custom'.\n";
        return false;
    }
    strategy = strategyInput;
    return true;
}

bool Game::ChangeStrategy(const std::string& newStrategy) {
    if (!isPlaying) {
        std::cout << "Cannot change strategy when the game is not in progress.\n";
        return false;
    }
    if (newStrategy != "ordered" && newStrategy != "custom") {
        std::cout << "Invalid strategy. Choose 'ordered' or 'custom'.\n";
        return false;
    }
    strategy = newStrategy;
    return true;
}

bool Game::IsInitialized() const {
    return isInitialized;
}

bool Game::IsPlaying() const {
    return isPlaying;
}

void Game::StartGame() {
    if (!isInitialized) {
        std::cout << "Game is not initialized.\n";
        return;
    }
    if (isPlaying) {
        std::cout << "Game is already in progress.\n";
        return;
    }

    if (strategy == "ordered") {
        PlaceShipsOrdered();
    } else if (strategy == "custom") {
        PlaceShipsRandomly();
    } else {
        PlaceShipsOrdered(); //default
    }

    isPlaying = true;
    std::cout << "All ships placed " << (strategy == "ordered" ? "in order.\n" : "randomly.\n");
    std::cout << "Game started.\n";
}

void Game::StopGame() {
    if (!isPlaying) {
        std::cout << "Game is not in progress.\n";
        return;
    }
    isPlaying = false;
    std::cout << "Game stopped.\n";
}

std::pair<uint64_t, uint64_t> Game::MakeShot() {
    if (!isPlaying) {
        std::cout << "Game is not in progress.\n";
        return {0, 0};
    }

    if (strategy == "ordered") {
        if (lastX >= boardWidth) {
            lastX = 0;
            lastY++;
            if (lastY >= boardHeight) {
                gameOver = true;
                std::cout << "All coordinates have been shot. Game Over.\n";
                return {0, 0};
            }
        }
        return {lastX++, lastY};
    } else if (strategy == "custom") {
        uint64_t x = static_cast<uint64_t>(std::rand() % boardWidth);
        uint64_t y = static_cast<uint64_t>(std::rand() % boardHeight);
        return {x, y};
    }
}

ShotResult Game::ReceiveShot(uint64_t x, uint64_t y) {
    if (x >= boardWidth || y >= boardHeight) {
        std::cout << "Shot coordinates out of bounds.\n";
        return ShotResult::MISS;
    }

    if (grid[y][x] == 'X' || grid[y][x] == 'O') {
        std::cout << "Already shot at (" << x << ", " << y << ").\n";
        return ShotResult::MISS;
    }

    for (auto& ship : ships) {
        if (ship.orientation == 'h') {
            if (y == ship.y && x >= ship.x && x < ship.x + ship.size) {
                grid[y][x] = 'X'; // Mark hit on grid
                bool sunk = true;
                for (int i = 0; i < ship.size; ++i) {
                    if (grid[y][ship.x + i] != 'X') {
                        sunk = false;
                        break;
                    }
                }
                ship.isKill = sunk;
                if (sunk) {
                    std::cout << "Hit and sunk ship at (" << x << ", " << y << ")!\n";
                    return ShotResult::KILL;
                }
                std::cout << "Hit at (" << x << ", " << y << ")!\n";
                return ShotResult::HIT;
            }
        } else if (ship.orientation == 'v') {
            if (x == ship.x && y >= ship.y && y < ship.y + ship.size) {
                grid[y][x] = 'X'; // Mark hit on grid
                bool sunk = true;
                for (int i = 0; i < ship.size; ++i) {
                    if (grid[ship.y + i][x] != 'X') {
                        sunk = false;
                        break;
                    }
                }
                ship.isKill = sunk;
                if (sunk) {
                    std::cout << "Hit and sunk ship at (" << x << ", " << y << ")!\n";
                    return ShotResult::KILL;
                }
                std::cout << "Hit at (" << x << ", " << y << ")!\n";
                return ShotResult::HIT;
            }
        }
    }

    grid[y][x] = 'O'; // Mark miss on grid
    std::cout << "Miss at (" << x << ", " << y << ").\n";
    return ShotResult::MISS;
}

void Game::SetLastShotResult(ShotResult result) {
    if (!isPlaying) {
        std::cout << "Game is not in progress.\n";
        return;
    }

    if (result == ShotResult::HIT) {
        bool allSunk = true;
        for (const auto& ship : ships) {
            if (!ship.isKill) {
                allSunk = false;
                break;
            }
        }
        if (allSunk) {
            gameOver = true;
            won = true;
            std::cout << "All ships have been sunk. You win!\n";
        }
    } else if (result == ShotResult::MISS) {
        std::cout << "Missed the target.\n";
    } else if (result == ShotResult::KILL) {
        std::cout << "A ship has been sunk!\n";
    }
}

bool Game::IsFinished() const {
    return gameOver;
}

bool Game::DidWin() const {
    return won;
}

bool Game::DidLose() const {
    return !won && gameOver;
}

bool Game::DumpGame(const std::string& path) const {
    if (!isInitialized) {
        std::cout << "Game is not initialized.\n";
        return false;
    }
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cout << "Failed to open file for dumping game.\n";
        return false;
    }

    ofs << boardWidth << " " << boardHeight << "\n";
    for (const auto& ship : ships) {
        ofs << ship.size << " " << ship.orientation << " " << ship.x << " " << ship.y << "\n";
    }
    ofs.close();
    return true;
}

bool Game::LoadGame(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cout << "Failed to open file for loading game.\n";
        return false;
    }

    uint64_t width, height;
    std::string loadedStrategy;
    ifs >> width >> height;
    ifs >> loadedStrategy;

    Reset();
    boardWidth = width;
    boardHeight = height;
    strategy = loadedStrategy;
    isInitialized = true;

    Ship tempShip(0, 'h', 0, 0);
    while (ifs >> tempShip.size >> tempShip.orientation >> tempShip.x >> tempShip.y) {
        if ((tempShip.orientation != 'h' && tempShip.orientation != 'v') ||
            tempShip.x >= boardWidth ||
            tempShip.y >= boardHeight ||
            tempShip.size == 0 ||
            ((tempShip.orientation == 'h') && (tempShip.x + tempShip.size > boardWidth)) ||
            ((tempShip.orientation == 'v') && (tempShip.y + tempShip.size > boardHeight))) {
            std::cout << "Invalid ship data in file.\n";
            return false;
        }

        bool overlap = false;
        for (const auto& existingShip : ships) {
            if (tempShip.orientation == 'h') {
                if (existingShip.y == tempShip.y &&
                    !(tempShip.x + tempShip.size <= existingShip.x || tempShip.x >= existingShip.x + existingShip.size)) {
                    overlap = true;
                    break;
                }
            } else {
                if (existingShip.x == tempShip.x &&
                    !(tempShip.y + tempShip.size <= existingShip.y || tempShip.y >= existingShip.y + existingShip.size)) {
                    overlap = true;
                    break;
                }
            }
        }

        if (overlap) {
            std::cout << "Ships overlap detected in file.\n";
            return false;
        }

        ships.emplace_back(tempShip.size, tempShip.orientation, tempShip.x, tempShip.y);
    }

    ifs.close();
    return true;
}

bool Game::AddShip(const Ship& ship) {
    if (!isInitialized || isPlaying) {
        std::cout << "Cannot add ships after the game has started.\n";
        return false;
    }
    if (ship.size < 1 || ship.size > static_cast<int>(shipCounts.size())) {
        std::cout << "Invalid ship size.\n";
        return false;
    }
    if (shipCounts[ship.size - 1] == 0) {
        std::cout << "No remaining ships of this type.\n";
        return false;
    }
    if (!CanPlaceShip(ship)) {
        std::cout << "Cannot place ship at the given location.\n";
        return false;
    }

    ships.emplace_back(ship.size, ship.orientation, ship.x, ship.y);
    shipCounts[ship.size - 1]--;
    return true;
}

const std::vector<Ship>& Game::GetShips() const {
    return ships;
}

bool Game::CanPlaceShip(const Ship& newShip) const {
    if (newShip.orientation == 'h') {
        if (newShip.x + newShip.size > boardWidth)
            return false;
    } else if (newShip.orientation == 'v') {
        if (newShip.y + newShip.size > boardHeight)
            return false;
    } else {
        return false;
    }

    for (const auto& ship : ships) {
        if (newShip.orientation == 'h' && ship.orientation == 'h') {
            if (ship.y == newShip.y &&
                !(newShip.x + newShip.size <= ship.x || newShip.x >= ship.x + ship.size)) {
                return false;
            }
        } else if (newShip.orientation == 'v' && ship.orientation == 'v') {
            if (ship.x == newShip.x &&
                !(newShip.y + newShip.size <= ship.y || newShip.y >= ship.y + ship.size)) {
                return false;
            }
        }
    }

    return true;
}

void Game::PlaceShipsRandomly() {
    for (size_t i = 0; i < shipCounts.size(); ++i) {
        int shipSize = static_cast<int>(i + 1);
        for (int count = 0; count < shipCounts[i]; ++count) {
            bool placed = false;
            int attempts = 0;
            const int maxAttempts = 100;

            while (!placed && attempts < maxAttempts) {
                char orientation = (std::rand() % 2 == 0) ? 'h' : 'v';
                uint64_t x = std::rand() % boardWidth;
                uint64_t y = std::rand() % boardHeight;

                Ship tempShip(shipSize, orientation, x, y);

                bool overlap = false;
                for (const auto& existingShip : ships) {
                    if (tempShip.orientation == 'h') {
                        if (existingShip.y == tempShip.y &&
                            !(tempShip.x + tempShip.size <= existingShip.x || tempShip.x >= existingShip.x + existingShip.size)) {
                            overlap = true;
                            break;
                        }
                    } else {
                        if (existingShip.x == tempShip.x &&
                            !(tempShip.y + tempShip.size <= existingShip.y || tempShip.y >= existingShip.y + existingShip.size)) {
                            overlap = true;
                            break;
                        }
                    }
                }

                if (!overlap && CanPlaceShip(tempShip)) {
                    ships.emplace_back(tempShip);
                    if (orientation == 'h') {
                        for (int i = 0; i < shipSize; ++i) {
                            if (x + i < boardWidth && y < boardHeight) {
                                grid[y][x + i] = 'S';
                            }
                        }
                    } else {
                        for (int i = 0; i < shipSize; ++i) {
                            if (y + i < boardHeight && x < boardWidth) {
                                grid[y + i][x] = 'S';
                            }
                        }
                    }
                    placed = true;
                }

                attempts++;
            }

            if (!placed) {
                std::cerr << "Failed to place ship of size " << shipSize << ". (Check board size and ship counts)\n";
            }
        }
    }
}

void Game::PlaceShipsOrdered() {
    uint64_t x = 0;
    uint64_t y = 0;

    for (size_t i = 0; i < shipCounts.size(); ++i) {
        int shipSize = static_cast<int>(i + 1);
        for (int count = 0; count < shipCounts[i]; ++count) {
            bool placed = false;

            if (x + shipSize <= boardWidth) {
                Ship tempShip(shipSize, 'h', x, y);

                bool overlap = false;
                for (const auto& existingShip : ships) {
                    if (existingShip.orientation == 'h') {
                        if (existingShip.y == tempShip.y &&
                            !(tempShip.x + tempShip.size <= existingShip.x || tempShip.x >= existingShip.x + existingShip.size)) {
                            overlap = true;
                            break;
                        }
                    } else {
                        if (existingShip.x == tempShip.x &&
                            !(tempShip.y + tempShip.size <= existingShip.y || tempShip.y >= existingShip.y + existingShip.size)) {
                            overlap = true;
                            break;
                        }
                    }
                }

                if (!overlap && CanPlaceShip(tempShip)) {
                    ships.emplace_back(tempShip);
                    for (int i = 0; i < shipSize; ++i) {
                        if (x + i < boardWidth && y < boardHeight) {
                            grid[y][x + i] = 'S';
                        }
                    }
                    placed = true;
                }
            }
            if (!placed && y + shipSize <= boardHeight) {
                Ship tempShip(shipSize, 'v', x, y);

                bool overlap = false;
                for (const auto& existingShip : ships) {
                    if (existingShip.orientation == 'h') {
                        if (existingShip.y == tempShip.y &&
                            !(tempShip.x + tempShip.size <= existingShip.x || tempShip.x >= existingShip.x + existingShip.size)) {
                            overlap = true;
                            break;
                        }
                    } else {
                        if (existingShip.x == tempShip.x &&
                            !(tempShip.y + tempShip.size <= existingShip.y || tempShip.y >= existingShip.y + existingShip.size)) {
                            overlap = true;
                            break;
                        }
                    }
                }

                if (!overlap && CanPlaceShip(tempShip)) {
                    ships.emplace_back(tempShip);

                    for (int i = 0; i < shipSize; ++i) {
                        if (y + i < boardHeight && x < boardWidth) {
                            grid[y + i][x] = 'S';
                        }
                    }
                    placed = true;
                }
            }

            if (!placed) {
                x = 0;
                y++;
                if (y >= boardHeight) {
                    std::cerr << "Failed to place ship of size " << shipSize << " in ordered strategy.\n";
                    return;
                }
                count--;
            } else {
                x += shipSize;
                if (x >= boardWidth) {
                    x = 0;
                    y++;
                    if (y >= boardHeight) {
                        std::cerr << "Board is full. Cannot place more ships.\n";
                        return;
                    }
                }
            }
        }
    }
}