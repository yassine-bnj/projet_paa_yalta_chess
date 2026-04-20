#pragma once

#include <array>
#include <string>

#include "MinMaxAI.hpp"

struct RuntimeConfig {
    std::array<bool, 3> aiPlayers{true, false, true}; // Red, White, Black
    AISearchConfig aiSearch{};
};

RuntimeConfig loadRuntimeConfig(const std::string& filePath);
