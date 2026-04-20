#include "GameController.hpp"

GameController::GameController(sf::RenderWindow& renderWindow,
                               Plateau& board,
                               std::array<bool, 3> aiPlayers,
                               AISearchConfig aiConfig)
    : window(renderWindow),
      plateau(board),
      inputStrategy(std::make_unique<MouseInputStrategy>()),
      ai(aiConfig),
      aiControlledPlayers(aiPlayers) {
}

void GameController::handleEvents() {
    while (const auto event = window.pollEvent()) {
        inputStrategy->handleEvent(*event, window, plateau);
    }

    handleAiTurn();
}

std::size_t GameController::playerIndex(PlayerId player) {
    switch (player) {
        case PlayerId::Red:
            return 0;
        case PlayerId::White:
            return 1;
        case PlayerId::Black:
            return 2;
    }

    return 0;
}

bool GameController::isAiTurn() const {
    if (plateau.isGameOver()) {
        return false;
    }

    return aiControlledPlayers[playerIndex(plateau.getCurrentPlayer())];
}

void GameController::handleAiTurn() {
    if (!isAiTurn()) {
        return;
    }

    const PlayerId player = plateau.getCurrentPlayer();
    const auto move = ai.chooseMove(plateau, player);
    if (!move.has_value()) {
        return;
    }

    plateau.applyMove(*move);
}
