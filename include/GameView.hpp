#pragma once

#include <SFML/Graphics.hpp>

#include "Plateau.hpp"

class GameView {
public:
    explicit GameView(Plateau& plateau);

    void render(sf::RenderWindow& window);

private:
    Plateau& plateau;
};
