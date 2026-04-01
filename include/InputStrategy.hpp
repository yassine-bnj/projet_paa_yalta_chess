#pragma once

#include <SFML/Graphics.hpp>

#include "Plateau.hpp"

class InputStrategy {
public:
    virtual ~InputStrategy() = default;
    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window, Plateau& plateau) = 0;
};

class MouseInputStrategy final : public InputStrategy {
public:
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, Plateau& plateau) override;
};
