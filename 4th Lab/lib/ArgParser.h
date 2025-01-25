#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <functional>

namespace ArgumentParser {

class ArgParser {
public:
    explicit ArgParser(const std::string& name);

    ArgParser& AddStringArgument(const std::string& longName, const std::string& description = "");
    ArgParser& AddStringArgument(char shortName, const std::string& longName, const std::string& description = "");
    ArgParser& AddIntArgument(const std::string& longName, const std::string& description = "");
    ArgParser& AddIntArgument(char shortName, const std::string& longName, const std::string& description = "");
    ArgParser& AddFlag(char shortName, const std::string& longName, const std::string& description = "");

    ArgParser& Default(const std::variant<std::string, int, bool>& defaultValue);
    ArgParser& MultiValue(size_t minCount = 0);
    ArgParser& StoreValue(std::string& ref);
    ArgParser& StoreValue(int& ref);
    ArgParser& StoreValue(bool& ref);
    ArgParser& StoreValues(std::vector<std::string>& ref);
    ArgParser& StoreValues(std::vector<int>& ref);
    ArgParser& StoreFlag(bool& ref);
    ArgParser& Positional();

    ArgParser& Required();

    bool Parse(int argc, char* argv[]);
    bool Parse(const std::vector<std::string>& args);
    std::string GetStringValue(const std::string& name) const;
    int GetIntValue(const std::string& name, size_t index = 0) const;
    bool GetFlag(const std::string& name) const;

    ArgParser& AddHelp(char shortName, const std::string& longName, const std::string& description = "");
    std::string HelpDescription() const;
    bool Help() const;

private:
    struct Argument {
        enum Type { Flag, String, Int } type;
        std::string longName;
        char shortName = '\0';
        std::string description;
        bool required = false;
        bool isPositional = false;
        bool hasDefault = false;
        bool addedInCurrentSession = false;
        std::variant<std::string, int, bool> defaultValue;
        std::vector<std::variant<std::string, int, bool>> values;
        std::function<void(const std::variant<std::string, int, bool>&)> storeFunc;
        size_t minCount = 0;
    };

    Argument* FindArgument(const std::string& name);
    const Argument* FindArgument(const std::string& name) const;
    void AddArgumentInternal(const Argument& argument);
    bool ParseSingleArgument(const std::string& arg, size_t& i, const std::vector<std::string>& args);
    bool ValidateArguments() const;

    std::string programName;
    std::unordered_map<std::string, Argument> arguments;
    std::unordered_map<char, std::string> shortToLong;
    std::string lastAddedArgumentName;
};

} // namespace ArgumentParser