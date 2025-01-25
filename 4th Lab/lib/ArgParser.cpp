#include "ArgParser.h"
#include <sstream>
#include <iostream>
namespace ArgumentParser {

ArgParser::ArgParser(const std::string& name) : programName(name) {}

ArgParser::Argument* ArgParser::FindArgument(const std::string& name) {
    auto it = arguments.find(name);
    if (it != arguments.end()) {
        return &it->second;
    }
    return nullptr;
}

const ArgParser::Argument* ArgParser::FindArgument(const std::string& name) const {
    auto it = arguments.find(name);
    if (it != arguments.end()) {
        return &it->second;
    }
    return nullptr;
}

void ArgParser::AddArgumentInternal(const Argument& argument) {
    if (!argument.longName.empty()) {
        arguments[argument.longName] = argument;
        lastAddedArgumentName = argument.longName;
    }
    if (argument.shortName != '\0') {
        shortToLong[argument.shortName] = argument.longName;
    }
}

ArgParser& ArgParser::AddStringArgument(const std::string& longName, const std::string& description) {
    Argument arg;
    arg.type = Argument::String;
    arg.longName = longName;
    arg.description = description;
    AddArgumentInternal(arg);
    return *this;
}

ArgParser& ArgParser::AddStringArgument(char shortName, const std::string& longName, const std::string& description) {
    Argument arg;
    arg.type = Argument::String;
    arg.shortName = shortName;
    arg.longName = longName;
    arg.description = description;
    AddArgumentInternal(arg);
    return *this;
}

ArgParser& ArgParser::AddIntArgument(const std::string& longName, const std::string& description) {
    Argument arg;
    arg.type = Argument::Int;
    arg.longName = longName;
    arg.description = description;
    AddArgumentInternal(arg);
    return *this;
}

ArgParser& ArgParser::AddIntArgument(char shortName, const std::string& longName, const std::string& description) {
    Argument arg;
    arg.type = Argument::Int;
    arg.shortName = shortName;
    arg.longName = longName;
    arg.description = description;
    AddArgumentInternal(arg);
    return *this;
}

ArgParser& ArgParser::AddFlag(char shortName, const std::string& longName, const std::string& description) {
    Argument arg;
    arg.type = Argument::Flag;
    arg.shortName = shortName;
    arg.longName = longName;
    arg.description = description;
    AddArgumentInternal(arg);
    return *this;
}

ArgParser& ArgParser::Default(const std::variant<std::string, int, bool>& defaultValue) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->hasDefault = true;
        lastAddedArgument->defaultValue = defaultValue;
    }
    return *this;
}

ArgParser& ArgParser::MultiValue(size_t minCount) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->minCount = minCount;
    }
    return *this;
}

ArgParser& ArgParser::StoreValue(std::string& ref) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->storeFunc = [&ref](const std::variant<std::string, int, bool>& value) {
            if (std::holds_alternative<std::string>(value)) {
                ref = std::get<std::string>(value);
            }
        };
    }
    return *this;
}

ArgParser& ArgParser::StoreValue(int& ref) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->storeFunc = [&ref](const std::variant<std::string, int, bool>& value) {
            if (std::holds_alternative<int>(value)) {
                ref = std::get<int>(value);
            }
        };
    }
    return *this;
}

ArgParser& ArgParser::StoreValue(bool& ref) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->storeFunc = [&ref](const std::variant<std::string, int, bool>& value) {
            if (std::holds_alternative<bool>(value)) {
                ref = std::get<bool>(value);
            }
        };
    }
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<std::string>& ref) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->storeFunc = [&ref](const std::variant<std::string, int, bool>& value) {
            if (std::holds_alternative<std::string>(value)) {
                ref.push_back(std::get<std::string>(value));
            }
        };
    }
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<int>& ref) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->storeFunc = [&ref](const std::variant<std::string, int, bool>& value) {
            if (std::holds_alternative<int>(value)) {
                ref.push_back(std::get<int>(value));
            }
        };
    }
    return *this;
}

ArgParser& ArgParser::StoreFlag(bool& ref) {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->storeFunc = [&ref](const std::variant<std::string, int, bool>& value) {
            if (std::holds_alternative<bool>(value)) {
                ref = std::get<bool>(value);
            }
        };
    }
    return *this;
}

ArgParser& ArgParser::Positional() {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->isPositional = true;
    }
    return *this;
}

ArgParser& ArgParser::Required() {
    Argument* lastAddedArgument = FindArgument(lastAddedArgumentName);
    if (lastAddedArgument) {
        lastAddedArgument->required = true;
    }
    return *this;
}

bool ArgParser::Parse(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    for (auto& [key, argument] : arguments) {
        argument.values.clear();
        if (argument.hasDefault) {
            argument.values.push_back(argument.defaultValue);
            if (argument.storeFunc) {
                argument.storeFunc(argument.defaultValue);
            }
        }
    }

    std::vector<std::string> positionalArgs;
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg.rfind("--", 0) == 0 || (arg.rfind("-", 0) == 0 && arg.length() > 1)) {
            if (!ParseSingleArgument(arg, i, args)) {
                return false;
            }
        } else {
            positionalArgs.push_back(arg);
        }
    }

    size_t posIndex = 0;
    for (auto& [key, argument] : arguments) {
        if (argument.isPositional) {
            size_t assigned = 0;
            while (posIndex < positionalArgs.size()) {
                if (argument.type == Argument::String) {
                    argument.values.emplace_back(positionalArgs[posIndex]);
                    if (argument.storeFunc) {
                        argument.storeFunc(positionalArgs[posIndex]);
                    }
                } else if (argument.type == Argument::Int) {
                    try {
                        int intValue = std::stoi(positionalArgs[posIndex]);
                        argument.values.emplace_back(intValue);
                        if (argument.storeFunc) {
                            argument.storeFunc(intValue);
                        }
                    } catch (...) {
                        return false;
                    }
                }
                posIndex++;
                assigned++;

                if (argument.minCount == 0) {
                    break;
                }
            }

            if (argument.minCount > 0 && argument.values.size() < argument.minCount) {
                return false;
            }

            if (argument.required && argument.values.empty()) {
                return false;
            }
        }
    }

    if (posIndex < positionalArgs.size()) {
        return false;
    }
    if (!ValidateArguments()) {
        return false;
    }

    return true;
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
    if (args.empty()) return false;

    int argc = static_cast<int>(args.size());
    std::vector<char*> argv(argc);
    for (size_t i = 0; i < args.size(); ++i) {
        argv[i] = const_cast<char*>(args[i].c_str());
    }

    return Parse(argc, argv.data());
}

bool ArgParser::ParseSingleArgument(const std::string& arg, size_t& i, const std::vector<std::string>& args) {
    if (arg.rfind("--", 0) == 0) {
        size_t delimiterPos = arg.find('=', 2);
        std::string key = (delimiterPos != std::string::npos) ? 
                          arg.substr(2, delimiterPos - 2) : 
                          arg.substr(2);
        std::string value = (delimiterPos != std::string::npos) ? 
                            arg.substr(delimiterPos + 1) : "";

        Argument* argument = FindArgument(key);
        if (!argument) return false;

        if (argument->type == Argument::Flag) {
            argument->values.emplace_back(true);
            if (argument->storeFunc) argument->storeFunc(true);
        } else {
            if (value.empty()) {
                if (i + 1 < args.size()) {
                    value = args[++i];
                } else {
                    return false;
                }
            }
            if (argument->type == Argument::String) {
                argument->values.emplace_back(value);
                if (argument->storeFunc) argument->storeFunc(value);
            } else if (argument->type == Argument::Int) {
                try {
                    int intValue = std::stoi(value);
                    argument->values.emplace_back(intValue);
                    if (argument->storeFunc) argument->storeFunc(intValue);
                } catch (...) {
                    return false;
                }
            }
        }
    } else if (arg.rfind("-", 0) == 0 && arg.length() > 1) {
        size_t j = 1;
        while (j < arg.length()) {
            char shortName = arg[j];
            auto it = shortToLong.find(shortName);
            if (it == shortToLong.end()) return false;

            Argument* argument = FindArgument(it->second);
            if (!argument) return false;

            j++;

            if (argument->type == Argument::Flag) {
                argument->values.emplace_back(true);
                if (argument->storeFunc) argument->storeFunc(true);
                continue;
            }

            std::string value;
            if (j < arg.length() && arg[j] == '=') {
                value = arg.substr(j + 1);
                j = arg.length();
            } else if (j < arg.length()) {
                value = arg.substr(j);
                j = arg.length();
            } else {
                if (i + 1 < args.size()) {
                    value = args[++i];
                } else {
                    return false;
                }
            }

            if (argument->type == Argument::String) {
                argument->values.emplace_back(value);
                if (argument->storeFunc) argument->storeFunc(value);
            } else if (argument->type == Argument::Int) {
                try {
                    int intValue = std::stoi(value);
                    argument->values.emplace_back(intValue);
                    if (argument->storeFunc) argument->storeFunc(intValue);
                } catch (...) {
                    return false;
                }
            }
            break;
        }
    } else {
        return false;
    }
    return true;
}

bool ArgParser::ValidateArguments() const {
    if (Help()) {
        return true;
    }

    for (const auto& [key, argument] : arguments) {
        if (argument.required && argument.values.empty()) {
            return false;
        }
        if (argument.minCount > 0 && argument.values.size() < argument.minCount) {
            return false;
        }
    }
    return true;
}

std::string ArgParser::GetStringValue(const std::string& name) const {
    const Argument* arg = FindArgument(name);
    if (arg && !arg->values.empty()) {
        for (const auto& val : arg->values) {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val);
            }
        }
    }
    return "";
}

int ArgParser::GetIntValue(const std::string& name, size_t index) const {
    const Argument* arg = FindArgument(name);
    if (arg && index < arg->values.size() && std::holds_alternative<int>(arg->values[index])) {
        return std::get<int>(arg->values[index]);
    }
    return 0;
}

bool ArgParser::GetFlag(const std::string& name) const {
    const Argument* arg = FindArgument(name);
    if (arg && !arg->values.empty()) {
        for (const auto& val : arg->values) {
            if (std::holds_alternative<bool>(val)) {
                return std::get<bool>(val);
            }
        }
    }
    return false;
}

ArgParser& ArgParser::AddHelp(char shortName, const std::string& longName, const std::string& description) {
    Argument arg;
    arg.type = Argument::Flag;
    arg.shortName = shortName;
    arg.longName = longName;
    arg.description = description;
    AddArgumentInternal(arg);
    return *this;
}

std::string ArgParser::HelpDescription() const {
    std::ostringstream oss;
    oss << "Usage: " << programName << " [options]";
    for (const auto& [key, argument] : arguments) {
        if (argument.isPositional) {
            oss << " <" << argument.longName << ">";
        }
    }
    oss << "\n\nOptions:\n";
    for (const auto& [key, argument] : arguments) {
        oss << "  ";
        if (argument.shortName != '\0') {
            oss << "-" << argument.shortName << ", ";
        } else {
            oss << "    ";
        }
        oss << "--" << argument.longName;
        if (argument.type == Argument::String) {
            oss << " <string>";
        } else if (argument.type == Argument::Int) {
            oss << " <int>";
        }
        oss << "\t" << argument.description;
        if (argument.hasDefault) {
            oss << " (default: ";
            if (std::holds_alternative<std::string>(argument.defaultValue)) {
                oss << std::get<std::string>(argument.defaultValue);
            } else if (std::holds_alternative<int>(argument.defaultValue)) {
                oss << std::get<int>(argument.defaultValue);
            } else if (std::holds_alternative<bool>(argument.defaultValue)) {
                oss << (std::get<bool>(argument.defaultValue) ? "true" : "false");
            }
            oss << ")";
        }
        if (argument.minCount > 0) {
            oss << " (requires at least " << argument.minCount << " value(s))";
        }
        oss << "\n";
    }
    return oss.str();
}

bool ArgParser::Help() const {
    const Argument* arg = FindArgument("help");
    if (arg && !arg->values.empty()) {
        for (const auto& val : arg->values) {
            if (std::holds_alternative<bool>(val) && std::get<bool>(val)) {
                return true;
            }
        }
    }
    return false;
}

} // namespace ArgumentParser