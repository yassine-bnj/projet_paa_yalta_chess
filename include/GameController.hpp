#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <memory>

#include "InputStrategy.hpp"
#include "MinMaxAI.hpp"
#include "Plateau.hpp"
class GameView;

class GameController {
public:
    GameController(sf::RenderWindow& window,
                   Plateau& plateau,
                   std::array<bool, 3> aiPlayers = {true, false, true},
                   AISearchConfig aiConfig = {},
                   GameView* view = nullptr);

    void handleEvents();

private:
    sf::RenderWindow& window;
    Plateau& plateau;
    std::unique_ptr<InputStrategy> inputStrategy;
    MinMaxAI ai;
    std::array<bool, 3> aiControlledPlayers;
    GameView* view = nullptr;

    static std::size_t playerIndex(PlayerId player);
    bool isAiTurn() const;
    void handleAiTurn();
};
