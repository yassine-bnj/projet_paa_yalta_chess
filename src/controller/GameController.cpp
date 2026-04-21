#include "GameController.hpp"
#include "GameView.hpp"

GameController::GameController(sf::RenderWindow& renderWindow,
                               Plateau& board,
                                                             std::array<bool, 3> aiPlayers,
                                                             AISearchConfig aiConfig,
                                                             GameView* view)
    : window(renderWindow),
      plateau(board),
      inputStrategy(std::make_unique<MouseInputStrategy>()),
      ai(aiConfig),
      aiControlledPlayers(aiPlayers) {
        this->view = view;
}

void GameController::handleEvents() {
    while (const auto event = window.pollEvent()) {
        // If a promotion is pending, let the view consume promotion clicks first.
        if (plateau.hasPendingPromotion() && view != nullptr) {
            if (view->handlePromotionEvent(*event, window)) {
                continue;
            }
        }

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
