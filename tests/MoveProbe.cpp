#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "Plateau.hpp"

namespace {
std::string cellToString(const sf::Vector2i& cell) {
    return "(" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ")";
}

std::string movesToString(const std::vector<sf::Vector2i>& moves) {
    if (moves.empty()) {
        return "[aucun mouvement]";
    }

    std::string text;
    for (std::size_t index = 0; index < moves.size(); ++index) {
        text += cellToString(moves[index]);
        if (index + 1 < moves.size()) {
            text += ", ";
        }
    }
    return text;
}

std::vector<sf::Vector2i> normalize(std::vector<sf::Vector2i> moves) {
    std::sort(moves.begin(), moves.end(), [](const sf::Vector2i& left, const sf::Vector2i& right) {
        if (left.x != right.x) {
            return left.x < right.x;
        }
        return left.y < right.y;
    });
    moves.erase(std::unique(moves.begin(), moves.end(), [](const sf::Vector2i& left, const sf::Vector2i& right) {
        return left.x == right.x && left.y == right.y;
    }), moves.end());
    return moves;
}

bool equalMoves(std::vector<sf::Vector2i> expected, std::vector<sf::Vector2i> actual) {
    expected = normalize(std::move(expected));
    actual = normalize(std::move(actual));
    return expected == actual;
}

bool containsMoves(const std::vector<sf::Vector2i>& actual, const std::vector<sf::Vector2i>& required) {
    const std::vector<sf::Vector2i> normActual = normalize(actual);
    for (const auto& move : required) {
        const auto it = std::find_if(normActual.begin(), normActual.end(), [&](const sf::Vector2i& probe) {
            return probe.x == move.x && probe.y == move.y;
        });
        if (it == normActual.end()) {
            return false;
        }
    }
    return true;
}

void runCheckCase(const std::string& name, const Plateau& plateau, PlayerId player, bool expected) {
    const bool actual = plateau.isKingInCheck(player);
    std::cout << ((actual == expected) ? "PASS " : "FAIL ") << name;
    std::cout << " | expected=" << (expected ? "check" : "safe");
    std::cout << " | got=" << (actual ? "check" : "safe") << '\n';
}

void runCheckmateCase(const std::string& name, const Plateau& plateau, PlayerId player, bool expected) {
    const bool actual = plateau.isCheckmate(player);
    std::cout << ((actual == expected) ? "PASS " : "FAIL ") << name;
    std::cout << " | expected=" << (expected ? "mate" : "not-mate");
    std::cout << " | got=" << (actual ? "mate" : "not-mate") << '\n';
}

void runExactCase(const std::string& name, const Plateau& plateau, const sf::Vector2i& source, std::vector<sf::Vector2i> expected) {
    const std::string piece = plateau.debugPieceSummaryForCell(source);
    const std::vector<sf::Vector2i> moves = plateau.debugLegalMovesForCell(source);
    const bool ok = equalMoves(expected, moves);

    std::cout << (ok ? "PASS " : "FAIL ") << name;
    std::cout << " | piece=" << piece;
    std::cout << " | source=" << cellToString(source);
    std::cout << " | expected=" << movesToString(expected);
    std::cout << " | got=" << movesToString(moves) << '\n';
}

void runContainsCase(const std::string& name, const Plateau& plateau, const sf::Vector2i& source, std::vector<sf::Vector2i> required) {
    const std::string piece = plateau.debugPieceSummaryForCell(source);
    const std::vector<sf::Vector2i> moves = plateau.debugLegalMovesForCell(source);
    const bool ok = containsMoves(moves, required);

    std::cout << (ok ? "PASS " : "FAIL ") << name;
    std::cout << " | piece=" << piece;
    std::cout << " | source=" << cellToString(source);
    std::cout << " | required=" << movesToString(required);
    std::cout << " | got=" << movesToString(moves) << '\n';
}

Plateau makeBoard(const std::function<void(Plateau&)>& setup) {
    Plateau board;
    board.debugClearPieces();
    board.debugSetCurrentPlayer(PlayerId::White);
    setup(board);
    return board;
}
}

int main() {
    std::cout << "=== Yalta move probe ===\n";

    {
        Plateau colorBoard;
        const auto conflicts = colorBoard.debugColorConflicts();
        if (conflicts.empty()) {
            std::cout << "PASS Color alternation adjacency\n";
        } else {
            std::cout << "FAIL Color alternation adjacency | conflicts=" << conflicts.size() << '\n';
            for (const auto& line : conflicts) {
                std::cout << "INFO " << line << '\n';
            }
        }
    }

    const Plateau enPassantBoard = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {1, 4}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::Black, {1, 5}, true, true);
    });
    runExactCase("En passant white pawn", enPassantBoard, {1, 4}, {{2, 4}, {2, 5}});

    const Plateau centerCase1 = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {3, 3}, false, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {2, 2}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {2, 4}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {4, 2}, true, false);
    });
    runContainsCase("Center transition A", centerCase1, {3, 3}, {{4, 8}, {8, 4}});

    const Plateau centerCase2 = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {3, 4}, false, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {2, 3}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {2, 5}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {8, 5}, true, false);
    });
    runContainsCase("Center transition B", centerCase2, {3, 4}, {{4, 3}, {8, 8}});

    const Plateau centerCase3 = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {4, 3}, false, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {3, 2}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {5, 2}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {5, 8}, true, false);
    });
    runContainsCase("Center transition C", centerCase3, {4, 3}, {{3, 4}, {8, 8}});

    const Plateau centerCase4 = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {8, 8}, false, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {7, 7}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {7, 9}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {9, 7}, true, false);
    });
    runContainsCase("Center transition D", centerCase4, {8, 8}, {{3, 4}, {4, 3}});

    const Plateau centerCase5 = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {8, 4}, false, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {7, 3}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {7, 5}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {9, 5}, true, false);
    });
    runContainsCase("Center transition E", centerCase5, {8, 4}, {{3, 3}, {4, 8}});

    const Plateau centerCase6 = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {4, 8}, false, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {3, 7}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {3, 9}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {5, 7}, true, false);
    });
    runContainsCase("Center transition F", centerCase6, {4, 8}, {{3, 3}, {8, 4}});

    const Plateau sextantEdgeCaptureA = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Rook, PlayerId::White, {8, 7}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::Black, {8, 5}, true, false);
    });
    runContainsCase("Sextant edge capture 6->5", sextantEdgeCaptureA, {8, 7}, {{8, 5}});

    const Plateau sextantEdgeCaptureB = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Rook, PlayerId::White, {7, 8}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::Black, {5, 8}, true, false);
    });
    runContainsCase("Sextant edge capture 6->4", sextantEdgeCaptureB, {7, 8}, {{5, 8}});

    const Plateau checkByBishopCenter = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::King, PlayerId::White, {3, 4}, true, false);
        board.debugAddPiece(PieceType::Bishop, PlayerId::Black, {8, 8}, true, false);
    });
    runCheckCase("Check detection bishop center", checkByBishopCenter, PlayerId::White, true);

    const Plateau blockedRookCheck = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::King, PlayerId::White, {5, 7}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {7, 7}, true, false);
        board.debugAddPiece(PieceType::Rook, PlayerId::Black, {8, 7}, true, false);
    });
    runCheckCase("Check detection blocked rook", blockedRookCheck, PlayerId::White, false);

    const Plateau noKingBoard = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Rook, PlayerId::Black, {8, 7}, true, false);
    });
    runCheckCase("Check detection missing king", noKingBoard, PlayerId::White, false);

    const Plateau legalFilterUnderCheck = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::King, PlayerId::White, {3, 4}, true, false);
        board.debugAddPiece(PieceType::Knight, PlayerId::White, {0, 7}, true, false);
        board.debugAddPiece(PieceType::Bishop, PlayerId::Black, {8, 8}, true, false);
    });
    runExactCase("Legal filtering under check (knight)", legalFilterUnderCheck, {0, 7}, {});

    const Plateau checkmateCorner = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::King, PlayerId::White, {0, 0}, true, false);
        board.debugAddPiece(PieceType::Rook, PlayerId::Black, {0, 2}, true, false);
        board.debugAddPiece(PieceType::Rook, PlayerId::Black, {2, 0}, true, false);
        board.debugAddPiece(PieceType::Bishop, PlayerId::Black, {2, 2}, true, false);
    });
    runExactCase("Checkmate corner king legal moves", checkmateCorner, {0, 0}, {});
    runCheckmateCase("Checkmate detection corner", checkmateCorner, PlayerId::White, true);

    runCheckmateCase("Checkmate detection bishop center (escape exists)", checkByBishopCenter, PlayerId::White, false);

    std::cout << "\n--- Capture highlight test ---\n";
    const Plateau captureTest = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {1, 2}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::Black, {2, 3}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::Black, {2, 1}, true, false);
    });
    const std::size_t pawnIndex = 0;
    const std::vector<sf::Vector2i> captureLegalMoves = captureTest.debugLegalMovesForCell({1, 2});
    std::cout << "White Pawn at (1,2) legal moves: " << movesToString(captureLegalMoves) << '\n';
    for (const auto& move : captureLegalMoves) {
        const bool highlightAsCapture = captureTest.debugIsCaptureMoveForPiece(pawnIndex, move);
        std::cout << "  " << cellToString(move) << (highlightAsCapture ? " [HIGHLIGHT_RED]" : " [highlight_green]") << '\n';
    }

    std::cout << "--- Snapshot de validation ---\n";
    const Plateau snapshot = makeBoard([](Plateau& board) {
        board.debugAddPiece(PieceType::Pawn, PlayerId::White, {1, 4}, true, false);
        board.debugAddPiece(PieceType::Pawn, PlayerId::Black, {1, 5}, true, true);
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {3, 3}, false, false);
        board.debugAddPiece(PieceType::Bishop, PlayerId::White, {8, 8}, false, false);
    });

    for (int x = 0; x < 12; ++x) {
        for (int y = 0; y < 12; ++y) {
            const sf::Vector2i cell{x, y};
            const std::string piece = snapshot.debugPieceSummaryForCell(cell);
            if (!piece.empty()) {
                std::cout << "INFO " << piece << " @ " << cellToString(cell)
                          << " -> " << movesToString(snapshot.debugLegalMovesForCell(cell)) << '\n';
            }
        }
    }

    return 0;
}
