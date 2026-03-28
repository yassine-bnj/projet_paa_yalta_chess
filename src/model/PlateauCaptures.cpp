#include "Plateau.hpp"

void Plateau::captureByPawn(int x, int y, int xMove, int yMove, int side, std::vector<sf::Vector2i>& moves) const {
    int xCapture = x + xMove;
    int yCapture = y + yMove;
    adjustCoordinates(x, y, xCapture, yCapture, xMove, yMove);
    if (isWallInMatrix5(x, y, xMove, yMove) || isOutOfBoard(xCapture, yCapture)) {
        return;
    }

    const auto index = pieceAt({xCapture, yCapture});
    if (index != pieces.size() && sideOf(pieces[index].getOwner()) != side) {
        moves.emplace_back(xCapture, yCapture);
        return;
    }

    const sf::Vector2i adjacentCell{x, yCapture};
    const auto adjacentIndex = pieceAt(adjacentCell);
    if (adjacentIndex != pieces.size()) {
        const Piece& adjacentPiece = pieces[adjacentIndex];
        if (adjacentPiece.getType() == PieceType::Pawn && sideOf(adjacentPiece.getOwner()) != side && adjacentPiece.isEnPassantEligible()) {
            moves.emplace_back(xCapture, yCapture);
        }
    }
}

void Plateau::captureOnBoardCenter(int x, int y, int side, std::vector<sf::Vector2i>& moves) const {
    for (const auto& move : centerCaptureTargets(x, y)) {
        const auto index = pieceAt(move);
        if (index != pieces.size() && sideOf(pieces[index].getOwner()) != side) {
            moves.push_back(move);
        }
    }
}
