#ifndef CMD_H
#define CMD_H

#include <string>
#include <functional>
#include <unordered_map>
#include <sstream>

class CommandProcessor {
public:
    CommandProcessor();
    void ProcessCommand(const std::string& input);

private:
    using CommandHandler = std::function<void(std::istringstream&)>;
    std::unordered_map<std::string, CommandHandler> commandMap;

    void Ping(std::istringstream& args);
    void Exit(std::istringstream& args);
    void Create(std::istringstream& args);
    void Set(std::istringstream& args);
    void Get(std::istringstream& args);
    void Start(std::istringstream& args);
    void Stop(std::istringstream& args);
    void Shot(std::istringstream& args);
    void Dump(std::istringstream& args);
    void Load(std::istringstream& args);
    void Finished(std::istringstream& args);
    void Win(std::istringstream& args);
    void Lose(std::istringstream& args);
};

#endif