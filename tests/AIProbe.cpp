#include <algorithm>
#include <iostream>

#include "MinMaxAI.hpp"
#include "Plateau.hpp"

int main() {
    Plateau board;
    board.debugClearPieces();

    board.debugAddPiece(PieceType::King, PlayerId::Red, {10, 10}, true, false);
    board.debugAddPiece(PieceType::King, PlayerId::White, {0, 0}, true, false);
    board.debugAddPiece(PieceType::King, PlayerId::Black, {11, 11}, true, false);
    board.debugAddPiece(PieceType::Rook, PlayerId::Red, {9, 10}, true, false);

    board.debugSetCurrentPlayer(PlayerId::Red);

    MinMaxAI ai(AISearchConfig{2, true});
    const auto aiMove = ai.chooseMove(board, PlayerId::Red);
    if (!aiMove.has_value()) {
        std::cout << "FAIL AI move selection | no move returned\n";
        return 1;
    }

    const auto legalMoves = board.getLegalMovesForCurrentPlayer();
    const auto it = std::find_if(legalMoves.begin(), legalMoves.end(), [&](const Plateau::Move& move) {
        return move.from == aiMove->from && move.to == aiMove->to;
    });

    if (it == legalMoves.end()) {
        std::cout << "FAIL AI move selection | returned move is illegal\n";
        return 1;
    }

    if (!board.applyMove(*aiMove)) {
        std::cout << "FAIL AI move application | move rejected by board\n";
        return 1;
    }

    if (board.getCurrentPlayer() == PlayerId::Red) {
        std::cout << "FAIL Turn progression | turn did not advance\n";
        return 1;
    }

    std::cout << "PASS AI move selection and application\n";
    return 0;
}
