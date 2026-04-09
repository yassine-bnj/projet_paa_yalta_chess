#include "GameView.hpp"

#include <iostream>

namespace {
std::string playerToFrench(PlayerId player) {
    switch (player) {
        case PlayerId::Red:
            return "Rouge";
        case PlayerId::White:
            return "Blanc";
        case PlayerId::Black:
        default:
            return "Noir";
    }
}
}

GameView::GameView(Plateau& plateauRef)
    : plateau(plateauRef) {
    plateau.addObserver(this);
}

GameView::~GameView() {
    plateau.removeObserver(this);
}

void GameView::onPlateauEvent(const PlateauEvent& event) {
    if (event.type == PlateauEventType::TurnChanged) {
        std::cout << "Tour du joueur " << playerToFrench(event.currentPlayer) << "\n";
    }
    modelDirty = true;
}

void GameView::render(sf::RenderWindow& window) {
    if (!modelDirty) {
        return;
    }

    window.setTitle("YaltaChess SFML 3 - Tour: " + playerToFrench(plateau.getCurrentPlayer()));
    window.clear(sf::Color(33, 37, 41));
    plateau.draw(window);
    window.display();
    modelDirty = false;
}
