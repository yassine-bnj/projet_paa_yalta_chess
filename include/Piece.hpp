#pragma once

#include <SFML/Graphics.hpp>

enum class PlayerId {
    Red,
    White,
    Black
};

enum class PieceType {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King
};

class Piece {
public:
    Piece(PieceType type, PlayerId owner, sf::Vector2i cell, sf::Vector2f position, float radius, sf::Color color);

    PieceType getType() const;
    PlayerId getOwner() const;
    sf::Vector2i getCell() const;
    bool isAlive() const;
    bool hasMoved() const;
    bool isEnPassantEligible() const;

    void setCell(sf::Vector2i cell, sf::Vector2f position);
    void setAlive(bool alive);
    void setHasMoved(bool moved);
    void setEnPassantEligible(bool eligible);
    void draw(sf::RenderTarget& target) const;

private:
    PieceType type;
    PlayerId owner;
    sf::Vector2i cell;
    bool alive;
    bool moved;
    bool enPassantEligible;
    sf::CircleShape shape;
};
