#include "MovementStrategy.hpp"

#include "Plateau.hpp"

namespace {
const std::vector<sf::Vector2i> kDirections = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
    {1, 1}, {1, 0}, {1, -1}, {0, -1}
};
}

void RookMoveStrategy::generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const {
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;

    board.recursiveMove(xStart, yStart, kDirections[5].x, kDirections[5].y, side, moves);
    board.recursiveMove(xStart, yStart, kDirections[1].x, kDirections[1].y, side, moves);
    board.recursiveMove(xStart, yStart, kDirections[3].x, kDirections[3].y, side, moves);
    board.recursiveMove(xStart, yStart, kDirections[7].x, kDirections[7].y, side, moves);
}

void BishopMoveStrategy::generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const {
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;

    board.recursiveMove(xStart, yStart, kDirections[0].x, kDirections[0].y, side, moves);
    board.recursiveMove(xStart, yStart, kDirections[2].x, kDirections[2].y, side, moves);
    board.recursiveMove(xStart, yStart, kDirections[4].x, kDirections[4].y, side, moves);
    board.recursiveMove(xStart, yStart, kDirections[6].x, kDirections[6].y, side, moves);
}

void QueenMoveStrategy::generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const {
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;

    for (const auto& dir : kDirections) {
        board.recursiveMove(xStart, yStart, dir.x, dir.y, side, moves);
    }
}

void KnightMoveStrategy::generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const {
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;

    board.knightMove(xStart, yStart, kDirections[1].x, kDirections[1].y, side, moves, 0, false);
    board.knightMove(xStart, yStart, kDirections[3].x, kDirections[3].y, side, moves, 0, false);
    board.knightMove(xStart, yStart, kDirections[5].x, kDirections[5].y, side, moves, 0, false);
    board.knightMove(xStart, yStart, kDirections[7].x, kDirections[7].y, side, moves, 0, false);

    board.knightMove(xStart, yStart, kDirections[1].x, kDirections[1].y, side, moves, 0, true);
    board.knightMove(xStart, yStart, kDirections[3].x, kDirections[3].y, side, moves, 0, true);
    board.knightMove(xStart, yStart, kDirections[5].x, kDirections[5].y, side, moves, 0, true);
    board.knightMove(xStart, yStart, kDirections[7].x, kDirections[7].y, side, moves, 0, true);

    for (const auto& transition : board.knightCenterTransitions(xStart, yStart)) {
        moves.push_back(transition);
    }
}

void KingMoveStrategy::generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const {
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;

    for (const auto& dir : kDirections) {
        board.kingMove(xStart, yStart, dir.x, dir.y, side, moves);
    }
}

void PawnMoveStrategy::generate(const Plateau& board, const Piece& piece, int side, std::vector<sf::Vector2i>& moves) const {
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;

    if (piece.hasMoved()) {
        board.pawnMove2(xStart, yStart, side, moves);
    } else {
        board.pawnMove(xStart, yStart, side, moves, 0);
    }
}
