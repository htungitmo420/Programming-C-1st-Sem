#include "Cmd.h"
#include "Game.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

CommandProcessor::CommandProcessor() {
    commandMap["ping"] = [this](std::istringstream& args) {Ping(args);};
    commandMap["exit"] = [this](std::istringstream& args) {Exit(args);};
    commandMap["create"] = [this](std::istringstream& args) {Create(args);};
    commandMap["set"] = [this](std::istringstream& args) {Set(args);};
    commandMap["get"] = [this](std::istringstream& args) {Get(args);};
    commandMap["start"] = [this](std::istringstream& args) {Start(args);};
    commandMap["stop"] = [this](std::istringstream& args) {Stop(args);};
    commandMap["shot"] = [this](std::istringstream& args) {Shot(args);};
    commandMap["dump"] = [this](std::istringstream& args) {Dump(args);};
    commandMap["load"] = [this](std::istringstream& args) {Load(args);};
    commandMap["finished"] = [this](std::istringstream& args) {Finished(args);};
    commandMap["win"] = [this](std::istringstream& args) {Win(args);};
    commandMap["lose"] = [this](std::istringstream& args) {Lose(args);};
}

void CommandProcessor::ProcessCommand(const std::string& input) {
    Game& game = Game::Instance();
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    auto it = commandMap.find(command);
    if (it != commandMap.end()) {
        it->second(iss);
    }
    else {
        std::cout << "failed\n";
    }
}

void CommandProcessor::Ping(std::istringstream& args) {
    std::cout << "pong\n";
}

void CommandProcessor::Exit(std::istringstream& args) {
    std::cout << "ok\n";
    std::exit(0);
}

void CommandProcessor::Create(std::istringstream& args) {
    Game& game = Game::Instance();
    std::string mode;
    args >> mode;
    if (!mode.empty() && !game.IsInitialized()) {
        bool success = game.Initialize(mode);
        std::cout << (success ? "ok\n" : "failed\n");
    }
    else {
        std::cout << "failed\n";
    }
}

void CommandProcessor::Set(std::istringstream& args) {
    Game& game = Game::Instance();
    std::string property;
    args >> property;

    if (property == "width") {
        uint64_t width;
        args >> width;
        std::cout << (game.SetWidth(width) ? "ok\n" : "failed\n");
    }
    else if (property == "height") {
        uint64_t height;
        args >> height;
        std::cout << (game.SetHeight(height) ? "ok\n" : "failed\n");
    }
    else if (property == "count") {
        int type;
        uint32_t count;
        args >> type >> count;
        std::cout << (game.SetShipCount(type, count) ? "ok\n" : "failed\n");
    }
    else if (property == "strategy") {
        std::string strategy;
        args >> strategy;
        std::cout << (game.SetStrategy(strategy) ? "ok\n" : "failed\n");
    }
    else if (property == "result") {
        std::string resultStr;
        args >> resultStr;
        if (game.IsPlaying()) {
            ShotResult result;
            if (resultStr == "miss") {
                result = ShotResult::MISS;
            }
            else if (resultStr == "hit") {
                result = ShotResult::HIT;
            }
            else if (resultStr == "kill") {
                result = ShotResult::KILL;
            }
            else {
                std::cout << "failed\n";
                return;
            }
            game.SetLastShotResult(result);
            std::cout << "ok\n";
        }
        else {
            std::cout << "failed\n";
        }
    }
    else {
        std::cout << "failed\n";
    }
}

void CommandProcessor::Get(std::istringstream& args) {
    Game& game = Game::Instance();
    std::string property;
    args >> property;
    if (property == "width") {
        std::cout << game.GetWidth() << "\n";
    }
    else if (property == "height") {
        std::cout << game.GetHeight() << "\n";
    }
    else if (property == "count") {
        int type;
        args >> type;
        std::cout << game.GetShipCount(type) << "\n";
    }
    else {
        std::cout << "failed\n";
    }
}

void CommandProcessor::Start(std::istringstream& args) {
    Game& game = Game::Instance();
    game.StartGame();
    std::cout << (game.IsPlaying() ? "ok\n" : "failed\n");
}

void CommandProcessor::Stop(std::istringstream& args) {
    Game& game = Game::Instance();
    game.StopGame();
    std::cout << "ok\n";
}

void CommandProcessor::Shot(std::istringstream& args) {
    Game& game = Game::Instance();
    if (game.IsPlaying()) {
        uint64_t x, y;
        if (args >> x >> y) {
            ShotResult result = game.ReceiveShot(x, y);
            switch (result) {
                case ShotResult::MISS:
                    std::cout << "miss\n";
                    break;
                case ShotResult::HIT:
                    std::cout << "hit\n";
                    break;
                case ShotResult::KILL:
                    std::cout << "kill\n";
                    break;
            }
        }
        else {
            auto shot = game.MakeShot();
            std::cout << shot.first << " " << shot.second << "\n";
        }
    }
    else {
        std::cout << "failed\n";
    }
}

void CommandProcessor::Dump(std::istringstream& args) {
    Game& game = Game::Instance();
    std::string path;
    args >> path;
    if (path.empty()) {
        std::cout << "failed\n";
        return;
    }
    std::cout << (game.DumpGame(path) ? "ok\n" : "failed\n");
}

void CommandProcessor::Load(std::istringstream& args) {
    Game& game = Game::Instance();
    std::string path;
    args >> path;
    if (path.empty()) {
        std::cout << "failed\n";
        return;
    }
    std::cout << (game.LoadGame(path) ? "ok\n" : "failed\n");
}

void CommandProcessor::Finished(std::istringstream& args) {
    Game& game = Game::Instance();
    if (game.IsPlaying()) {
        std::cout << (game.IsFinished() ? "yes\n" : "no\n");
    }
    else {
        std::cout << "no\n";
    }
}

void CommandProcessor::Win(std::istringstream& args) {
    Game& game = Game::Instance();
    if (game.IsPlaying()) {
        std::cout << (game.DidWin() ? "yes\n" : "no\n");
    }
    else {
        std::cout << "no\n";
    }
}

void CommandProcessor::Lose(std::istringstream& args) {
    Game& game = Game::Instance();
    if (game.IsPlaying()) {
        std::cout << (game.DidLose() ? "yes\n" : "no\n");
    }
    else {
        std::cout << "no\n";
    }
}