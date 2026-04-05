#include "GameView.hpp"

GameView::GameView(Plateau& plateauRef)
    : plateau(plateauRef) {
    plateau.addObserver(this);
}

GameView::~GameView() {
    plateau.removeObserver(this);
}

void GameView::onPlateauEvent(const PlateauEvent&) {
    modelDirty = true;
}

void GameView::render(sf::RenderWindow& window) {
    if (!modelDirty) {
        return;
    }

    window.clear(sf::Color(33, 37, 41));
    plateau.draw(window);
    window.display();
    modelDirty = false;
}
