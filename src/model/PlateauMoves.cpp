#include "Plateau.hpp"

#include <algorithm>

void Plateau::recursiveMove(int xStart, int yStart, int xMove, int yMove, int side, std::vector<sf::Vector2i>& possibleMoves) const {
    int xDestination = xStart + xMove;
    int yDestination = yStart + yMove;

    if (isWallInMatrix5(xStart, yStart, xMove, yMove) || isWallInMatrix4(xStart, yStart, xMove, yMove)) {
        return;
    }

    const auto addIfEnemyOrEmpty = [&](int x, int y) {
        if (isOutOfBoard(x, y) || isSquareExist(x, y)) {
            return;
        }
        const auto index = pieceAt({x, y});
        if (index == pieces.size()) {
            possibleMoves.emplace_back(x, y);
            recursiveMove(x, y, xMove, yMove, side, possibleMoves);
        } else if (sideOf(pieces[index].getOwner()) != side) {
            possibleMoves.emplace_back(x, y);
        }
    };

    if (isCenterDiagonalTransition(xStart, yStart, xMove, yMove)) {
        for (const auto& destination : centerTransitionDestinations(xStart, yStart, xMove, yMove)) {
            addIfEnemyOrEmpty(destination.x, destination.y);
        }
        return;
    }

    adjustCoordinates(xStart, yStart, xDestination, yDestination, xMove, yMove);
    addIfEnemyOrEmpty(xDestination, yDestination);
}

void Plateau::knightMove(int xStart, int yStart, int xMove, int yMove, int side, std::vector<sf::Vector2i>& possibleMoves, int verification, bool methodTwo) const {
    int xDestination = xStart + xMove;
    int yDestination = yStart + yMove;
    if (isWallInMatrix5(xStart, yStart, xMove, yMove) || isWallInMatrix4(xStart, yStart, xMove, yMove)) {
        return;
    }

    adjustCoordinates(xStart, yStart, xDestination, yDestination, xMove, yMove);
    if (isOutOfBoard(xDestination, yDestination) || isSquareExist(xDestination, yDestination)) {
        return;
    }

    if (verification == 0) {
        if (methodTwo) {
            knightMove(xDestination, yDestination, xMove, yMove, side, possibleMoves, 1, true);
        } else {
            if (xMove == 0) {
                knightMove(xDestination, yDestination, -1, 0, side, possibleMoves, 1, false);
                knightMove(xDestination, yDestination, 1, 0, side, possibleMoves, 1, false);
            } else {
                knightMove(xDestination, yDestination, 0, -1, side, possibleMoves, 1, false);
                knightMove(xDestination, yDestination, 0, 1, side, possibleMoves, 1, false);
            }
        }
        return;
    }

    if (verification == 1) {
        if (methodTwo) {
            if (xMove == 0) {
                knightMove(xDestination, yDestination, -1, 0, side, possibleMoves, 2, true);
                knightMove(xDestination, yDestination, 1, 0, side, possibleMoves, 2, true);
            } else {
                knightMove(xDestination, yDestination, 0, -1, side, possibleMoves, 2, true);
                knightMove(xDestination, yDestination, 0, 1, side, possibleMoves, 2, true);
            }
        } else {
            knightMove(xDestination, yDestination, xMove, yMove, side, possibleMoves, 2, false);
        }
        return;
    }

    const auto index = pieceAt({xDestination, yDestination});
    if (index == pieces.size() || sideOf(pieces[index].getOwner()) != side) {
        possibleMoves.emplace_back(xDestination, yDestination);
    }
}

void Plateau::kingMove(int xStart, int yStart, int xMove, int yMove, int side, std::vector<sf::Vector2i>& possibleMoves) const {
    int xDestination = xStart + xMove;
    int yDestination = yStart + yMove;
    if (isWallInMatrix5(xStart, yStart, xMove, yMove) || isWallInMatrix4(xStart, yStart, xMove, yMove)) {
        return;
    }

    const auto tryAdd = [&](int x, int y) {
        if (isOutOfBoard(x, y) || isSquareExist(x, y)) {
            return;
        }
        const auto index = pieceAt({x, y});
        if (index == pieces.size() || sideOf(pieces[index].getOwner()) != side) {
            possibleMoves.emplace_back(x, y);
        }
    };

    if (isCenterDiagonalTransition(xStart, yStart, xMove, yMove)) {
        for (const auto& destination : centerTransitionDestinations(xStart, yStart, xMove, yMove)) {
            tryAdd(destination.x, destination.y);
        }
        return;
    }

    adjustCoordinates(xStart, yStart, xDestination, yDestination, xMove, yMove);
    tryAdd(xDestination, yDestination);
}

void Plateau::pawnMove(int xStart, int yStart, int side, std::vector<sf::Vector2i>& possibleMoves, int stop) const {
    if (stop == 2) {
        return;
    }

    const int matrixStart = determineSubMatrix(xStart, yStart);
    int xMove = -1;
    if (side == 1 || (side == 2 && (matrixStart == 5 || matrixStart == 6)) || (side == 3 && (matrixStart == 3 || matrixStart == 4))) {
        xMove = 1;
    }

    if (stop == 0) {
        captureByPawn(xStart, yStart, xMove, 1, side, possibleMoves);
        captureByPawn(xStart, yStart, xMove, -1, side, possibleMoves);
        captureOnBoardCenter(xStart, yStart, side, possibleMoves);
    }

    int yMove = 0;
    int xDestination = xStart + xMove;
    int yDestination = yStart + yMove;
    if (isWallInMatrix4(xStart, yStart, xMove, yMove)) {
        return;
    }

    adjustCoordinates(xStart, yStart, xDestination, yDestination, xMove, yMove);
    if (!isOutOfBoard(xDestination, yDestination) && !isSquareExist(xDestination, yDestination)) {
        if (!isOccupied({xDestination, yDestination})) {
            possibleMoves.emplace_back(xDestination, yDestination);
            pawnMove(xDestination, yDestination, side, possibleMoves, stop + 1);
        }
    }
}

void Plateau::pawnMove2(int xStart, int yStart, int side, std::vector<sf::Vector2i>& possibleMoves) const {
    const int matrixStart = determineSubMatrix(xStart, yStart);
    int xMove = -1;
    if (side == 1 || (side == 2 && (matrixStart == 5 || matrixStart == 6)) || (side == 3 && (matrixStart == 3 || matrixStart == 4))) {
        xMove = 1;
    }

    captureByPawn(xStart, yStart, xMove, 1, side, possibleMoves);
    captureByPawn(xStart, yStart, xMove, -1, side, possibleMoves);
    captureOnBoardCenter(xStart, yStart, side, possibleMoves);

    int yMove = 0;
    int xDestination = xStart + xMove;
    int yDestination = yStart + yMove;
    if (isWallInMatrix4(xStart, yStart, xMove, yMove)) {
        return;
    }

    adjustCoordinates(xStart, yStart, xDestination, yDestination, xMove, yMove);
    if (!isOutOfBoard(xDestination, yDestination) && !isSquareExist(xDestination, yDestination)) {
        if (!isOccupied({xDestination, yDestination})) {
            possibleMoves.emplace_back(xDestination, yDestination);
        }
    }
}

std::vector<sf::Vector2i> Plateau::getLegalMovesForPiece(std::size_t index) const {
    const Piece& piece = pieces[index];
    const int xStart = piece.getCell().x;
    const int yStart = piece.getCell().y;
    const int side = sideOf(piece.getOwner());

    const std::vector<sf::Vector2i> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
        {1, 1}, {1, 0}, {1, -1}, {0, -1}
    };

    std::vector<sf::Vector2i> moves;
    switch (piece.getType()) {
        case PieceType::Rook:
            recursiveMove(xStart, yStart, directions[5].x, directions[5].y, side, moves);
            recursiveMove(xStart, yStart, directions[1].x, directions[1].y, side, moves);
            recursiveMove(xStart, yStart, directions[3].x, directions[3].y, side, moves);
            recursiveMove(xStart, yStart, directions[7].x, directions[7].y, side, moves);
            break;
        case PieceType::Bishop:
            recursiveMove(xStart, yStart, directions[0].x, directions[0].y, side, moves);
            recursiveMove(xStart, yStart, directions[2].x, directions[2].y, side, moves);
            recursiveMove(xStart, yStart, directions[4].x, directions[4].y, side, moves);
            recursiveMove(xStart, yStart, directions[6].x, directions[6].y, side, moves);
            break;
        case PieceType::Queen:
            for (const auto& dir : directions) {
                recursiveMove(xStart, yStart, dir.x, dir.y, side, moves);
            }
            break;
        case PieceType::Knight:
            knightMove(xStart, yStart, directions[1].x, directions[1].y, side, moves, 0, false);
            knightMove(xStart, yStart, directions[3].x, directions[3].y, side, moves, 0, false);
            knightMove(xStart, yStart, directions[5].x, directions[5].y, side, moves, 0, false);
            knightMove(xStart, yStart, directions[7].x, directions[7].y, side, moves, 0, false);

            knightMove(xStart, yStart, directions[1].x, directions[1].y, side, moves, 0, true);
            knightMove(xStart, yStart, directions[3].x, directions[3].y, side, moves, 0, true);
            knightMove(xStart, yStart, directions[5].x, directions[5].y, side, moves, 0, true);
            knightMove(xStart, yStart, directions[7].x, directions[7].y, side, moves, 0, true);

            for (const auto& transition : knightCenterTransitions(xStart, yStart)) {
                moves.push_back(transition);
            }
            break;
        case PieceType::King:
            for (const auto& dir : directions) {
                kingMove(xStart, yStart, dir.x, dir.y, side, moves);
            }
            break;
        case PieceType::Pawn:
        default:
            if (piece.hasMoved()) {
                pawnMove2(xStart, yStart, side, moves);
            } else {
                pawnMove(xStart, yStart, side, moves, 0);
            }
            break;
    }

    std::sort(moves.begin(), moves.end(), [](const sf::Vector2i& a, const sf::Vector2i& b) {
        if (a.x != b.x) {
            return a.x < b.x;
        }
        return a.y < b.y;
    });
    moves.erase(std::unique(moves.begin(), moves.end(), [](const sf::Vector2i& a, const sf::Vector2i& b) {
        return a.x == b.x && a.y == b.y;
    }), moves.end());

    moves.erase(std::remove_if(moves.begin(), moves.end(), [this](const sf::Vector2i& move) {
        return !isInsideBoard(move);
    }), moves.end());

    return moves;
}
