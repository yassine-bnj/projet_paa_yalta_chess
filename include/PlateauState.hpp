#pragma once

#include <SFML/System/Vector2.hpp>

class Plateau;

class PlateauState {
public:
    virtual ~PlateauState() = default;
    virtual void handleClick(Plateau& board, sf::Vector2f position) = 0;
};

class BoardIdleState final : public PlateauState {
public:
    void handleClick(Plateau& board, sf::Vector2f position) override;
};

class BoardPieceSelectedState final : public PlateauState {
public:
    void handleClick(Plateau& board, sf::Vector2f position) override;
};

class BoardGameOverState final : public PlateauState {
public:
    void handleClick(Plateau& board, sf::Vector2f position) override;
};
