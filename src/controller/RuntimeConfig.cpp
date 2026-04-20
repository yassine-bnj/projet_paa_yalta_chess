#include "RuntimeConfig.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

namespace {
std::string trim(const std::string& text) {
    const auto begin = std::find_if_not(text.begin(), text.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
    if (begin == text.end()) {
        return {};
    }

    const auto end = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();

    return std::string(begin, end);
}

std::string toLower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

bool parseBool(const std::string& value, bool& out) {
    const std::string lower = toLower(trim(value));
    if (lower == "1" || lower == "true" || lower == "yes" || lower == "on") {
        out = true;
        return true;
    }

    if (lower == "0" || lower == "false" || lower == "no" || lower == "off") {
        out = false;
        return true;
    }

    return false;
}

bool parseInt(const std::string& value, int& out) {
    const std::string cleaned = trim(value);
    if (cleaned.empty()) {
        return false;
    }

    try {
        std::size_t idx = 0;
        const int parsed = std::stoi(cleaned, &idx);
        if (idx != cleaned.size()) {
            return false;
        }
        out = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
}

RuntimeConfig loadRuntimeConfig(const std::string& filePath) {
    RuntimeConfig config;

    std::ifstream input(filePath);
    if (!input) {
        std::cout << "[Config] Fichier introuvable: " << filePath
                  << " | valeurs par defaut utilisees.\n";
        return config;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        const std::string stripped = trim(line);
        if (stripped.empty() || stripped[0] == '#' || stripped[0] == ';') {
            continue;
        }

        const std::size_t sep = stripped.find('=');
        if (sep == std::string::npos) {
            std::cout << "[Config] Ligne ignoree (pas de '='): " << lineNumber << "\n";
            continue;
        }

        const std::string key = toLower(trim(stripped.substr(0, sep)));
        const std::string value = trim(stripped.substr(sep + 1));

        bool parsedBool = false;
        int parsedInt = 0;

        if (key == "ai.red") {
            if (parseBool(value, parsedBool)) {
                config.aiPlayers[0] = parsedBool;
            }
            continue;
        }

        if (key == "ai.white") {
            if (parseBool(value, parsedBool)) {
                config.aiPlayers[1] = parsedBool;
            }
            continue;
        }

        if (key == "ai.black") {
            if (parseBool(value, parsedBool)) {
                config.aiPlayers[2] = parsedBool;
            }
            continue;
        }

        if (key == "ai.depth") {
            if (parseInt(value, parsedInt)) {
                config.aiSearch.maxDepth = std::clamp(parsedInt, 1, 6);
            }
            continue;
        }

        if (key == "ai.parallel_root") {
            if (parseBool(value, parsedBool)) {
                config.aiSearch.parallelRoot = parsedBool;
            }
            continue;
        }
    }

    std::cout << "[Config] IA Red=" << (config.aiPlayers[0] ? "on" : "off")
              << " White=" << (config.aiPlayers[1] ? "on" : "off")
              << " Black=" << (config.aiPlayers[2] ? "on" : "off")
              << " depth=" << config.aiSearch.maxDepth
              << " parallel_root=" << (config.aiSearch.parallelRoot ? "on" : "off")
              << "\n";

    return config;
}
