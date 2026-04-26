#include "GameController.hpp"
#include "GameView.hpp"

#include <exception>
#include <cstdio>
#include <optional>

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

    if (plateau.hasPendingPromotion()) {
        return false;
    }

    return aiControlledPlayers[playerIndex(plateau.getCurrentPlayer())];
}

void GameController::handleAiTurn() {
    if (!isAiTurn()) {
        return;
    }

    const PlayerId player = plateau.getCurrentPlayer();

    std::optional<Plateau::Move> move;
    try {
        move = ai.chooseMove(plateau, player);
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "[AI] chooseMove exception: %s\n", ex.what());
        std::fflush(stderr);
        return;
    } catch (...) {
        std::fprintf(stderr, "[AI] chooseMove unknown exception\n");
        std::fflush(stderr);
        return;
    }

    if (!move.has_value()) {
        std::fprintf(stderr, "[AI] no move returned\n");
        std::fflush(stderr);
        return;
    }

    try {
        plateau.applyMove(*move);
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "[AI] applyMove exception: %s\n", ex.what());
        std::fflush(stderr);
    } catch (...) {
        std::fprintf(stderr, "[AI] applyMove unknown exception\n");
        std::fflush(stderr);
    }
}