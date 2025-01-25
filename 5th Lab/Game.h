// Game.h
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <cstdint>
#include <string>

enum class ShotResult {
    MISS,
    HIT,
    KILL
};

struct Ship {
    int size;
    char orientation; // 'h' for horizontal, 'v' for vertical
    uint64_t x;
    uint64_t y;
    bool isKill;

    Ship(int s, char orient, uint64_t posX, uint64_t posY)
        : size(s), orientation(orient), x(posX), y(posY), isKill(false) {}
};

class Game {
public:
    static Game& Instance();

    bool Initialize(const std::string& mode);
    void Reset();

    bool SetWidth(uint64_t width);
    uint64_t GetWidth() const;
    bool SetHeight(uint64_t height);
    uint64_t GetHeight() const;

    bool SetShipCount(int type, uint32_t count);
    uint32_t GetShipCount(int type) const;
    bool SetStrategy(const std::string& strategy);
    bool IsInitialized() const;
    bool IsPlaying() const;
    void StartGame();
    void StopGame();

    std::pair<uint64_t, uint64_t> MakeShot();
    ShotResult ReceiveShot(uint64_t x, uint64_t y);
    void SetLastShotResult(ShotResult result);

    bool IsFinished() const;
    bool DidWin() const;
    bool DidLose() const;

    bool DumpGame(const std::string& path) const;
    bool LoadGame(const std::string& path);

    bool AddShip(const Ship& ship);
    const std::vector<Ship>& GetShips() const;

    bool ChangeStrategy(const std::string& newStrategy);

private:
    Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void PlaceShipsOrdered();
    void PlaceShipsRandomly();
    bool CanPlaceShip(const Ship& newShip) const;

    std::vector<std::vector<char>> grid;

    bool isInitialized;
    bool isPlaying;
    bool gameOver;
    bool won;
    std::string mode; // "master" or "slave"
    std::string strategy; // "ordered" or "custom"
    uint64_t boardWidth;
    uint64_t boardHeight;

    std::vector<uint32_t> shipCounts;
    std::vector<Ship> ships;

    uint64_t lastX;
    uint64_t lastY;
};

#endif