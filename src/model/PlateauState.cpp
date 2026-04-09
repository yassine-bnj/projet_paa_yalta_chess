#include "PlateauState.hpp"

#include "Plateau.hpp"

void BoardIdleState::handleClick(Plateau& board, sf::Vector2f position) {
    const sf::Vector2i cell = board.pixelToCell(position);
    if (!board.isInsideBoard(cell)) {
        board.clearSelection();
        return;
    }

    if (board.isCurrentPlayerPieceAt(cell)) {
        board.selectPiece(board.pieceAt(cell));
        board.setPieceSelectedState();
    } else {
        board.clearSelection();
    }
}

void BoardPieceSelectedState::handleClick(Plateau& board, sf::Vector2f position) {
    const sf::Vector2i cell = board.pixelToCell(position);
    if (!board.isInsideBoard(cell)) {
        board.clearSelection();
        board.setIdleState();
        return;
    }

    if (board.tryMoveSelectedPiece(cell)) {
        board.setIdleState();
        return;
    }

    if (board.isCurrentPlayerPieceAt(cell)) {
        board.selectPiece(board.pieceAt(cell));
        return;
    }

    board.clearSelection();
    board.setIdleState();
}

void BoardGameOverState::handleClick(Plateau&, sf::Vector2f) {
}
