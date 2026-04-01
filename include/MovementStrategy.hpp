#pragma once

#include <SFML/System/Vector2.hpp>

#include <vector>

#include "Piece.hpp"

class Plateau;

class MoveStrategy {
public:
    virtual ~MoveStrategy() = default;
    virtual void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const = 0;
};

class RookMoveStrategy final : public MoveStrategy {
public:
    void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const override;
};

class BishopMoveStrategy final : public MoveStrategy {
public:
    void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const override;
};

class QueenMoveStrategy final : public MoveStrategy {
public:
    void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const override;
};

class KnightMoveStrategy final : public MoveStrategy {
public:
    void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const override;
};

class KingMoveStrategy final : public MoveStrategy {
public:
    void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const override;
};

class PawnMoveStrategy final : public MoveStrategy {
public:
    void generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const override;
};
