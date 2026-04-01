#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "InputStrategy.hpp"
#include "Plateau.hpp"

class GameController {
public:
    GameController(sf::RenderWindow& window, Plateau& plateau);

    void handleEvents();

private:
    sf::RenderWindow& window;
    Plateau& plateau;
    std::unique_ptr<InputStrategy> inputStrategy;
};
