#include "InputStrategy.hpp"

void MouseInputStrategy::handleEvent(const sf::Event& event, sf::RenderWindow& window, Plateau& plateau) {
    if (event.is<sf::Event::Closed>()) {
        window.close();
        return;
    }

    if (event.is<sf::Event::MouseButtonPressed>()) {
        plateau.handleClick(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    }
}
