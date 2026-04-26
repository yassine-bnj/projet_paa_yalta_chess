#include <algorithm>
#include <iostream>
#include <optional>

#include "MinMaxAI.hpp"
#include "Plateau.hpp"

int main() {
    Plateau::setDebugLoggingEnabled(false);

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

    std::cout << "\n--- Pending promotion copy ---\n";
    Plateau promotionBoard;
    promotionBoard.debugClearPieces();
    promotionBoard.debugSetCurrentPlayer(PlayerId::White);
    promotionBoard.debugAddPiece(PieceType::Pawn, PlayerId::White, {6, 0}, true, false);
    promotionBoard.debugAddPiece(PieceType::King, PlayerId::Red, {1, 4}, true, false);
    promotionBoard.debugAddPiece(PieceType::King, PlayerId::Black, {4, 4}, true, false);
    if (!promotionBoard.applyMove({{6, 0}, {7, 0}}) || !promotionBoard.hasPendingPromotion()) {
        std::cout << "FAIL Pending promotion setup | promotion was not triggered\n";
        return 1;
    }

    const Plateau copiedPromotionBoard = promotionBoard;
    Plateau assignedPromotionBoard;
    assignedPromotionBoard = promotionBoard;

    const bool copyPreservedPending = copiedPromotionBoard.hasPendingPromotion() &&
                                      copiedPromotionBoard.getPendingPromotionCell() == promotionBoard.getPendingPromotionCell();
    const bool assignmentPreservedPending = assignedPromotionBoard.hasPendingPromotion() &&
                                            assignedPromotionBoard.getPendingPromotionCell() == promotionBoard.getPendingPromotionCell();
    if (!copyPreservedPending || !assignmentPreservedPending) {
        std::cout << "FAIL Pending promotion copy | copy=" << (copyPreservedPending ? "ok" : "bad")
                  << " assignment=" << (assignmentPreservedPending ? "ok" : "bad") << '\n';
        return 1;
    }

    MinMaxAI blockedAi(AISearchConfig{2, false});
    if (blockedAi.chooseMove(promotionBoard, promotionBoard.getCurrentPlayer()).has_value()) {
        std::cout << "FAIL Pending promotion AI guard | AI should not choose a move while promotion is pending\n";
        return 1;
    }

    std::cout << "PASS Pending promotion copy\n";

    std::cout << "\n--- AI opening stress ---\n";
    Plateau openingBoard;
    openingBoard.debugSetCurrentPlayer(PlayerId::Red);

    MinMaxAI openingAi(AISearchConfig{1, false});
    for (int ply = 0; ply < 12; ++ply) {
        const PlayerId current = openingBoard.getCurrentPlayer();
        const auto move = openingAi.chooseMove(openingBoard, current);
        if (!move.has_value()) {
            std::cout << "FAIL AI opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | no move returned\n";
            return 1;
        }

        if (!openingBoard.applyMove(*move)) {
            std::cout << "FAIL AI opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | applyMove rejected move\n";
            return 1;
        }

        if (openingBoard.hasPendingPromotion()) {
            openingBoard.promotePawnAt(openingBoard.getPendingPromotionCell(), PieceType::Queen);
        }

        if (!openingBoard.isGameOver() && openingBoard.getCurrentPlayer() == current) {
            std::cout << "FAIL AI opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | turn did not advance\n";
            return 1;
        }

        if (openingBoard.isGameOver()) {
            break;
        }
    }

    std::cout << "PASS AI opening stress\n";

    std::cout << "\n--- AI full opening stress ---\n";
    Plateau fullOpeningBoard;
    fullOpeningBoard.debugSetCurrentPlayer(PlayerId::Red);

    MinMaxAI fullOpeningAi(AISearchConfig{2, true});
    for (int ply = 0; ply < 12; ++ply) {
        const PlayerId current = fullOpeningBoard.getCurrentPlayer();
        const auto move = fullOpeningAi.chooseMove(fullOpeningBoard, current);
        if (!move.has_value()) {
            std::cout << "FAIL AI full opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | no move returned\n";
            return 1;
        }

        const auto legalMoves = fullOpeningBoard.getLegalMovesForCurrentPlayer();
        const auto it = std::find_if(legalMoves.begin(), legalMoves.end(), [&](const Plateau::Move& candidate) {
            return candidate.from == move->from && candidate.to == move->to;
        });
        if (it == legalMoves.end()) {
            std::cout << "FAIL AI full opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | illegal move returned\n";
            return 1;
        }

        if (!fullOpeningBoard.applyMove(*move)) {
            std::cout << "FAIL AI full opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | applyMove rejected move\n";
            return 1;
        }

        if (fullOpeningBoard.hasPendingPromotion()) {
            fullOpeningBoard.promotePawnAt(fullOpeningBoard.getPendingPromotionCell(), PieceType::Queen);
        }

        if (!fullOpeningBoard.isGameOver() && fullOpeningBoard.getCurrentPlayer() == current) {
            std::cout << "FAIL AI full opening stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | turn did not advance\n";
            return 1;
        }

        if (fullOpeningBoard.isGameOver()) {
            break;
        }
    }

    std::cout << "PASS AI full opening stress\n";

    std::cout << "\n--- AI multi-turn stress ---\n";
    Plateau stressBoard;
    stressBoard.debugClearPieces();
    stressBoard.debugAddPiece(PieceType::King, PlayerId::Red, {10, 10}, true, false);
    stressBoard.debugAddPiece(PieceType::King, PlayerId::White, {0, 0}, true, false);
    stressBoard.debugAddPiece(PieceType::King, PlayerId::Black, {11, 11}, true, false);
    stressBoard.debugAddPiece(PieceType::Rook, PlayerId::Red, {9, 10}, true, false);
    stressBoard.debugAddPiece(PieceType::Rook, PlayerId::White, {0, 1}, true, false);
    stressBoard.debugAddPiece(PieceType::Rook, PlayerId::Black, {11, 10}, true, false);
    stressBoard.debugSetCurrentPlayer(PlayerId::Red);

    MinMaxAI stressAi(AISearchConfig{3, true});

    for (int ply = 0; ply < 12; ++ply) {
        const PlayerId current = stressBoard.getCurrentPlayer();
        const auto move = stressAi.chooseMove(stressBoard, current);
        if (!move.has_value()) {
            std::cout << "FAIL AI multi-turn stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | no move returned\n";
            return 1;
        }

        const auto legalMoves = stressBoard.getLegalMovesForCurrentPlayer();
        const auto it = std::find_if(legalMoves.begin(), legalMoves.end(), [&](const Plateau::Move& candidate) {
            return candidate.from == move->from && candidate.to == move->to;
        });
        if (it == legalMoves.end()) {
            std::cout << "FAIL AI multi-turn stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | illegal move returned\n";
            return 1;
        }

        if (!stressBoard.applyMove(*move)) {
            std::cout << "FAIL AI multi-turn stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | applyMove rejected move\n";
            return 1;
        }

        if (stressBoard.hasPendingPromotion()) {
            stressBoard.promotePawnAt(stressBoard.getPendingPromotionCell(), PieceType::Queen);
        }

        if (!stressBoard.isGameOver() && stressBoard.getCurrentPlayer() == current) {
            std::cout << "FAIL AI multi-turn stress | ply=" << ply
                      << " player=" << static_cast<int>(current)
                      << " | turn did not advance\n";
            return 1;
        }
    }

    std::cout << "PASS AI multi-turn stress\n";
    return 0;
}
