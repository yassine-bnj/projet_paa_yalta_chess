#include "GameView.hpp"

GameView::GameView(Plateau& plateauRef)
    : plateau(plateauRef) {
}

void GameView::render(sf::RenderWindow& window) {
    window.clear(sf::Color(33, 37, 41));
    plateau.draw(window);
    window.display();
}
