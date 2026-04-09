#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Case.hpp"
#include "MovementStrategy.hpp"
#include "PlateauObserver.hpp"
#include "PlateauState.hpp"
#include "Piece.hpp"

class Plateau {
public:
    Plateau();

    void draw(sf::RenderTarget& target) const;
    void handleClick(sf::Vector2f position);
    PlayerId getCurrentPlayer() const;
    std::vector<sf::Vector2i> debugLegalMovesForCell(sf::Vector2i cell) const;
    std::string debugPieceSummaryForCell(sf::Vector2i cell) const;
    std::vector<std::string> debugColorConflicts() const;
    void addObserver(IPlateauObserver* observer);
    void removeObserver(IPlateauObserver* observer);
    void debugClearPieces();
    void debugAddPiece(PieceType type, PlayerId owner, sf::Vector2i cell, bool moved = false, bool enPassant = false);
    void debugSetCurrentPlayer(PlayerId owner);

    friend class RookMoveStrategy;
    friend class BishopMoveStrategy;
    friend class QueenMoveStrategy;
    friend class KnightMoveStrategy;
    friend class KingMoveStrategy;
    friend class PawnMoveStrategy;
    friend class BoardIdleState;
    friend class BoardPieceSelectedState;
    friend class BoardGameOverState;

private:
    static constexpr int matrixCount = 6;
    static constexpr int matrixCellCount = 16;
    static constexpr float hexRadius = 32.f;
    static constexpr float originX = 240.f;
    static constexpr float originY = 120.f;

    std::vector<Case> cells;
    std::vector<sf::Vector2i> cellCoords;
    std::vector<Piece> pieces;
    std::optional<std::size_t> selectedPieceIndex;
    std::vector<sf::Vector2i> legalMoves;
    PlayerId currentPlayer;
    std::vector<IPlateauObserver*> observers;
    std::unique_ptr<PlateauState> interactionState;

    void buildBoard();
    void buildPieces();
    void clearSelection();
    void selectPiece(std::size_t index);
    bool tryMoveSelectedPiece(sf::Vector2i destination);

    sf::Vector2i pixelToCell(sf::Vector2f position) const;
    bool isInsideBoard(sf::Vector2i cell) const;
    std::size_t pieceAt(sf::Vector2i cell) const;
    bool isOccupied(sf::Vector2i cell) const;
    bool isCurrentPlayerPieceAt(sf::Vector2i cell) const;
    bool isFriendly(sf::Vector2i cell, PlayerId owner) const;
    bool isEnemy(sf::Vector2i cell, PlayerId owner) const;
    int sideOf(PlayerId owner) const;
    int determineSubMatrix(int x, int y) const;
    bool isOutOfBoard(int x, int y) const;
    bool isSquareExist(int x, int y) const;
    bool isWallInMatrix5(int xStart, int yStart, int xMove, int yMove) const;
    bool isWallInMatrix4(int xStart, int yStart, int xMove, int yMove) const;
    void adjustCoordinates(int xStart, int yStart, int& xDestination, int& yDestination, int& xMove, int& yMove) const;
    bool isCenterDiagonalTransition(int xStart, int yStart, int xMove, int yMove) const;
    std::vector<sf::Vector2i> centerTransitionDestinations(int xStart, int yStart, int xMove, int yMove) const;
    std::vector<sf::Vector2i> centerCaptureTargets(int x, int y) const;
    std::vector<sf::Vector2i> knightCenterTransitions(int x, int y) const;
    sf::Color ownerColor(PlayerId owner) const;
    sf::Vector2f cellCenter(sf::Vector2i cell) const;
    sf::ConvexShape makeHexMarker(sf::Vector2i cell, sf::Color fill, sf::Color outline, float outlineThickness) const;
    void advanceTurn();
    void notifyObservers(const PlateauEvent& event);
    void setIdleState();
    void setPieceSelectedState();
    void setGameOverState();

    std::vector<sf::Vector2i> getLegalMovesForPiece(std::size_t index) const;
    void recursiveMove(int xStart, int yStart, int xMove, int yMove, int side, std::vector<sf::Vector2i>& possibleMoves) const;
    void knightMove(int xStart, int yStart, int xMove, int yMove, int side, std::vector<sf::Vector2i>& possibleMoves, int verification, bool methodTwo) const;
    void kingMove(int xStart, int yStart, int xMove, int yMove, int side, std::vector<sf::Vector2i>& possibleMoves) const;
    void pawnMove(int xStart, int yStart, int side, std::vector<sf::Vector2i>& possibleMoves, int stop) const;
    void pawnMove2(int xStart, int yStart, int side, std::vector<sf::Vector2i>& possibleMoves) const;
    void captureByPawn(int x, int y, int xMove, int yMove, int side, std::vector<sf::Vector2i>& moves) const;
    void captureOnBoardCenter(int x, int y, int side, std::vector<sf::Vector2i>& moves) const;
};
