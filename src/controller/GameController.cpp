#include "GameController.hpp"

GameController::GameController(sf::RenderWindow& renderWindow, Plateau& board)
    : window(renderWindow), plateau(board), inputStrategy(std::make_unique<MouseInputStrategy>()) {
}

void GameController::handleEvents() {
    while (const auto event = window.pollEvent()) {
        inputStrategy->handleEvent(*event, window, plateau);
    }
}
