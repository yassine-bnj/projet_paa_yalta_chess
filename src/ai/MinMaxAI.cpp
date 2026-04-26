#include "MinMaxAI.hpp"

#include <algorithm>
#include <array>
#include <future>
#include <limits>
#include <random>
#include <vector>

namespace {
using ScoreVector = std::array<int, 3>;

class PlateauLogGuard {
public:
    explicit PlateauLogGuard(bool enabled)
        : previous(Plateau::isDebugLoggingEnabled()) {
        Plateau::setDebugLoggingEnabled(enabled);
    }

    ~PlateauLogGuard() {
        Plateau::setDebugLoggingEnabled(previous);
    }

private:
    bool previous;
};

int playerToIndex(PlayerId player) {
    switch (player) {
        case PlayerId::Red:
            return 0;
        case PlayerId::White:
            return 1;
        case PlayerId::Black:
            return 2;
    }

    return 0;
}

int pieceValue(PieceType type) {
    switch (type) {
        case PieceType::Pawn:
            return 100;
        case PieceType::Knight:
            return 320;
        case PieceType::Bishop:
            return 330;
        case PieceType::Rook:
            return 500;
        case PieceType::Queen:
            return 900;
        case PieceType::King:
            return 20000;
    }

    return 0;
}

int rootUtility(const ScoreVector& score, PlayerId rootPlayer) {
    const int rootIndex = playerToIndex(rootPlayer);
    const int rootScore = score[rootIndex];

    int opponentBest = std::numeric_limits<int>::min();
    for (int i = 0; i < static_cast<int>(score.size()); ++i) {
        if (i == rootIndex) {
            continue;
        }
        opponentBest = std::max(opponentBest, score[i]);
    }

    return rootScore - opponentBest;
}

ScoreVector evaluateBoard(const Plateau& board) {
    constexpr int mateScore = 1'000'000;
    ScoreVector score{0, 0, 0};

    if (board.isGameOver()) {
        score.fill(-mateScore);
        const auto winner = board.getWinner();
        if (winner.has_value()) {
            score[playerToIndex(*winner)] = mateScore;
        }
        return score;
    }

    for (const auto& piece : board.getPieces()) {
        if (!piece.isAlive()) {
            continue;
        }

        score[playerToIndex(piece.getOwner())] += pieceValue(piece.getType());
    }

    for (PlayerId player : {PlayerId::Red, PlayerId::White, PlayerId::Black}) {
        const int index = playerToIndex(player);
        if (!board.isPlayerAlive(player)) {
            score[index] -= 500000;
            continue;
        }

        if (board.isKingInCheck(player)) {
            score[index] -= 75;
        }
    }

    return score;
}

ScoreVector maxNSearch(const Plateau& board, int depth, PlayerId rootPlayer) {
    if (depth <= 0 || board.isGameOver() || board.hasPendingPromotion()) {
        return evaluateBoard(board);
    }

    const auto legalMoves = board.getLegalMovesForCurrentPlayer();
    if (legalMoves.empty()) {
        return evaluateBoard(board);
    }

    const PlayerId current = board.getCurrentPlayer();
    const int currentIndex = playerToIndex(current);

    bool hasCandidate = false;
    ScoreVector bestScore{0, 0, 0};

    for (const auto& move : legalMoves) {
        PlateauLogGuard logGuard(false);
        Plateau nextBoard = board;
        if (!nextBoard.applyMove(move)) {
            continue;
        }

        const ScoreVector candidateScore = maxNSearch(nextBoard, depth - 1, rootPlayer);
        if (!hasCandidate ||
            candidateScore[currentIndex] > bestScore[currentIndex] ||
            (candidateScore[currentIndex] == bestScore[currentIndex] &&
             rootUtility(candidateScore, rootPlayer) > rootUtility(bestScore, rootPlayer))) {
            bestScore = candidateScore;
            hasCandidate = true;
        }
    }

    if (!hasCandidate) {
        return evaluateBoard(board);
    }

    return bestScore;
}
}

MinMaxAI::MinMaxAI(AISearchConfig searchConfig)
    : config(searchConfig) {
}

std::optional<Plateau::Move> MinMaxAI::chooseMove(const Plateau& board, PlayerId player) const {
    if (board.getCurrentPlayer() != player || board.isGameOver() || board.hasPendingPromotion()) {
        return std::nullopt;
    }

    const auto legalMoves = board.getLegalMovesForCurrentPlayer();
    if (legalMoves.empty()) {
        return std::nullopt;
    }

    const int rootIndex = playerToIndex(player);
    const int depth = std::max(1, config.maxDepth);

    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    bool hasBest = false;
    Plateau::Move bestMove = legalMoves.front();
    ScoreVector bestScore{0, 0, 0};
    int bestRandomTie = 0;

    const bool useParallelRoot = config.parallelRoot && legalMoves.size() > 1 && legalMoves.size() <= 12;
    if (useParallelRoot) {
        std::vector<std::future<std::optional<ScoreVector>>> tasks;
        tasks.reserve(legalMoves.size());

        try {
            for (const auto& move : legalMoves) {
                tasks.emplace_back(std::async(std::launch::async, [board, move, depth, player]() -> std::optional<ScoreVector> {
                    try {
                        PlateauLogGuard logGuard(false);
                        Plateau nextBoard = board;
                        if (!nextBoard.applyMove(move)) {
                            return std::nullopt;
                        }

                        return maxNSearch(nextBoard, depth - 1, player);
                    } catch (const std::exception& ex) {
                        return std::nullopt;
                    } catch (...) {
                        return std::nullopt;
                    }
                }));
            }

            for (std::size_t i = 0; i < legalMoves.size(); ++i) {
                const auto scoreOpt = tasks[i].get();
                if (!scoreOpt.has_value()) {
                    continue;
                }

                const ScoreVector& score = *scoreOpt;
                const int randomTie = static_cast<int>(dist(rng) * 1000);
                if (!hasBest ||
                    score[rootIndex] > bestScore[rootIndex] ||
                    (score[rootIndex] == bestScore[rootIndex] && rootUtility(score, player) > rootUtility(bestScore, player)) ||
                    (score[rootIndex] == bestScore[rootIndex] && rootUtility(score, player) == rootUtility(bestScore, player) && randomTie > bestRandomTie)) {
                    bestMove = legalMoves[i];
                    bestScore = score;
                    bestRandomTie = randomTie;
                    hasBest = true;
                }
            }
        } catch (const std::exception&) {
            hasBest = false;
        } catch (...) {
            hasBest = false;
        }
    }

    if (!hasBest) {
        for (const auto& move : legalMoves) {
            try {
                PlateauLogGuard logGuard(false);
                Plateau nextBoard = board;
                if (!nextBoard.applyMove(move)) {
                    continue;
                }

                const ScoreVector score = maxNSearch(nextBoard, depth - 1, player);
                const int randomTie = static_cast<int>(dist(rng) * 1000);
                if (!hasBest ||
                    score[rootIndex] > bestScore[rootIndex] ||
                    (score[rootIndex] == bestScore[rootIndex] && rootUtility(score, player) > rootUtility(bestScore, player)) ||
                    (score[rootIndex] == bestScore[rootIndex] && rootUtility(score, player) == rootUtility(bestScore, player) && randomTie > bestRandomTie)) {
                    bestMove = move;
                    bestScore = score;
                    bestRandomTie = randomTie;
                    hasBest = true;
                }
            } catch (const std::exception&) {
                continue;
            } catch (...) {
                continue;
            }
        }
    }

    if (!hasBest) {
        return std::nullopt;
    }

    return bestMove;
}
