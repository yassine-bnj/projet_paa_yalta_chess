#pragma once

#include <optional>

#include "Plateau.hpp"

struct AISearchConfig {
    int maxDepth = 2;
    bool parallelRoot = true;
};

class MinMaxAI {
public:
    explicit MinMaxAI(AISearchConfig config = {});

    std::optional<Plateau::Move> chooseMove(const Plateau& board, PlayerId player) const;

private:
    AISearchConfig config;
};
