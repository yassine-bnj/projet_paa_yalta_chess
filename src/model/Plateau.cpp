#include "Plateau.hpp"

#include <algorithm>
#include <cmath>

namespace {
bool nearlyEqual(float a, float b, float epsilon = 0.01f) {
    return std::abs(a - b) <= epsilon;
}

bool samePoint(const sf::Vector2f& a, const sf::Vector2f& b) {
    return nearlyEqual(a.x, b.x) && nearlyEqual(a.y, b.y);
}

bool shareEdge(const std::vector<sf::Vector2f>& a, const std::vector<sf::Vector2f>& b) {
    int sharedVertices = 0;
    for (const auto& pa : a) {
        for (const auto& pb : b) {
            if (samePoint(pa, pb)) {
                ++sharedVertices;
                break;
            }
        }
    }
    return sharedVertices >= 2;
}

std::string colorToString(const sf::Color& c) {
    return "(" + std::to_string(c.r) + "," + std::to_string(c.g) + "," + std::to_string(c.b) + ")";
}
}

Plateau::Plateau()
    : currentPlayer(PlayerId::Red) {
    buildBoard();
    buildPieces();
}

void Plateau::buildPieces() {
    pieces.clear();

    const auto matrixIndexToLegacy = [](int matrixId, int index) -> sf::Vector2i {
        int x = 0;
        int y = 0;
        if (matrixId == 1) {
            y = 3 - (index / 4);
            x = 3 - (index % 4);
        } else if (matrixId == 2) {
            x = 3 - index / 4;
            y = (index % 4) + 4;
        } else if (matrixId == 3) {
            x = index / 4 + 4;
            y = 3 - index % 4;
        } else if (matrixId == 4) {
            y = index / 4 + 8;
            x = index % 4 + 4;
        } else if (matrixId == 5) {
            y = index / 4 + 4;
            x = index % 4 + 8;
        } else if (matrixId == 6) {
            x = index / 4 + 8;
            y = index % 4 + 8;
        }
        return sf::Vector2i{x, y};
    };

    const auto addPlayerPieces = [this, &matrixIndexToLegacy](PlayerId owner, int matrixA, int matrixB) {
        const sf::Color color = ownerColor(owner);
        const float radius = hexRadius * 0.50f;

        const std::vector<std::pair<PieceType, sf::Vector2i>> layout = {
            {PieceType::Rook,   {matrixA, 15}},
            {PieceType::Knight, {matrixA, 11}},
            {PieceType::Bishop, {matrixA, 7}},
            {PieceType::Queen,  {matrixA, 3}},
            {PieceType::King,   {matrixB, 12}},
            {PieceType::Bishop, {matrixB, 13}},
            {PieceType::Knight, {matrixB, 14}},
            {PieceType::Rook,   {matrixB, 15}},
            {PieceType::Pawn,   {matrixA, 14}},
            {PieceType::Pawn,   {matrixA, 10}},
            {PieceType::Pawn,   {matrixA, 6}},
            {PieceType::Pawn,   {matrixA, 2}},
            {PieceType::Pawn,   {matrixB, 8}},
            {PieceType::Pawn,   {matrixB, 9}},
            {PieceType::Pawn,   {matrixB, 10}},
            {PieceType::Pawn,   {matrixB, 11}},
        };

        for (const auto& [type, coord] : layout) {
            const sf::Vector2i legacy = matrixIndexToLegacy(coord.x, coord.y);
            pieces.emplace_back(type, owner, legacy, cellCenter(legacy), radius, color);
        }
    };

    addPlayerPieces(PlayerId::White, 1, 2);
    addPlayerPieces(PlayerId::Red, 4, 3);
    addPlayerPieces(PlayerId::Black, 5, 6);
}

void Plateau::draw(sf::RenderTarget& target) const {
    for (const auto& cell : cells) {
        cell.draw(target);
    }

    for (const auto& move : legalMoves) {
        const sf::ConvexShape overlay = makeHexMarker(move, sf::Color(80, 180, 120, 95), sf::Color::Transparent, 0.f);
        target.draw(overlay);
    }

    if (selectedPieceIndex.has_value()) {
        const sf::Vector2i cell = pieces[*selectedPieceIndex].getCell();
        const sf::ConvexShape outline = makeHexMarker(cell, sf::Color::Transparent, sf::Color(255, 215, 0), 4.f);
        target.draw(outline);
    }

    for (const auto& piece : pieces) {
        piece.draw(target);
    }
}

void Plateau::handleClick(sf::Vector2f position) {
    const sf::Vector2i cell = pixelToCell(position);
    if (!isInsideBoard(cell)) {
        clearSelection();
        return;
    }

    if (selectedPieceIndex.has_value()) {
        if (tryMoveSelectedPiece(cell)) {
            return;
        }

        const auto clickedPiece = pieceAt(cell);
        if (clickedPiece != pieces.size() && pieces[clickedPiece].getOwner() == currentPlayer) {
            selectPiece(clickedPiece);
        } else {
            clearSelection();
        }
        return;
    }

    const auto clickedPiece = pieceAt(cell);
    if (clickedPiece != pieces.size() && pieces[clickedPiece].getOwner() == currentPlayer) {
        selectPiece(clickedPiece);
    }
}

std::vector<sf::Vector2i> Plateau::debugLegalMovesForCell(sf::Vector2i cell) const {
    const auto index = pieceAt(cell);
    if (index == pieces.size()) {
        return {};
    }

    return getLegalMovesForPiece(index);
}

std::string Plateau::debugPieceSummaryForCell(sf::Vector2i cell) const {
    const auto index = pieceAt(cell);
    if (index == pieces.size()) {
        return {};
    }

    const Piece& piece = pieces[index];
    std::string owner;
    switch (piece.getOwner()) {
        case PlayerId::White:
            owner = "White";
            break;
        case PlayerId::Red:
            owner = "Red";
            break;
        case PlayerId::Black:
            owner = "Black";
            break;
    }

    std::string type;
    switch (piece.getType()) {
        case PieceType::Pawn:
            type = "Pawn";
            break;
        case PieceType::Rook:
            type = "Rook";
            break;
        case PieceType::Knight:
            type = "Knight";
            break;
        case PieceType::Bishop:
            type = "Bishop";
            break;
        case PieceType::Queen:
            type = "Queen";
            break;
        case PieceType::King:
            type = "King";
            break;
    }

    return owner + " " + type;
}

std::vector<std::string> Plateau::debugColorConflicts() const {
    std::vector<std::string> conflicts;
    if (cells.size() != cellCoords.size()) {
        conflicts.push_back("Internal mismatch: cells.size != cellCoords.size");
        return conflicts;
    }

    float meanY = 0.f;
    for (const auto& cell : cells) {
        meanY += cell.getCenter().y;
    }
    meanY /= static_cast<float>(cells.size());

    for (std::size_t i = 0; i < cells.size(); ++i) {
        const auto pi = cells[i].getWorldPoints();
        const sf::Color ci = cells[i].getFillColor();

        for (std::size_t j = i + 1; j < cells.size(); ++j) {
            const auto pj = cells[j].getWorldPoints();
            if (!shareEdge(pi, pj)) {
                continue;
            }

            const sf::Color cj = cells[j].getFillColor();
            if (ci.r == cj.r && ci.g == cj.g && ci.b == cj.b) {
                const sf::Vector2f c1 = cells[i].getCenter();
                const sf::Vector2f c2 = cells[j].getCenter();
                const bool top = c1.y < meanY && c2.y < meanY;
                const std::string tag = top ? "[TOP] " : "";
                conflicts.push_back(
                    tag + "same-color neighbors " +
                    "A(" + std::to_string(cellCoords[i].x) + "," + std::to_string(cellCoords[i].y) + ") " +
                    "B(" + std::to_string(cellCoords[j].x) + "," + std::to_string(cellCoords[j].y) + ") " +
                    "color=" + colorToString(ci)
                );
            }
        }
    }

    return conflicts;
}

void Plateau::clearSelection() {
    selectedPieceIndex.reset();
    legalMoves.clear();
}

void Plateau::selectPiece(std::size_t index) {
    selectedPieceIndex = index;
    legalMoves = getLegalMovesForPiece(index);
}

bool Plateau::tryMoveSelectedPiece(sf::Vector2i destination) {
    if (!selectedPieceIndex.has_value()) {
        return false;
    }

    if (std::find(legalMoves.begin(), legalMoves.end(), destination) == legalMoves.end()) {
        return false;
    }

    const std::size_t selectedIndex = *selectedPieceIndex;
    const sf::Vector2i startCell = pieces[selectedIndex].getCell();
    const auto targetIndex = pieceAt(destination);
    if (targetIndex != pieces.size()) {
        pieces[targetIndex].setAlive(false);
    }

    for (auto& piece : pieces) {
        piece.setEnPassantEligible(false);
    }

    pieces[selectedIndex].setCell(destination, cellCenter(destination));
    pieces[selectedIndex].setHasMoved(true);

    if (pieces[selectedIndex].getType() == PieceType::Pawn && std::abs(destination.x - startCell.x) == 2) {
        pieces[selectedIndex].setEnPassantEligible(true);
    }

    clearSelection();
    advanceTurn();
    return true;
}

sf::Vector2i Plateau::pixelToCell(sf::Vector2f position) const {
    for (std::size_t index = 0; index < cells.size(); ++index) {
        if (cells[index].contains(position)) {
            return cellCoords[index];
        }
    }

    return sf::Vector2i{-1, -1};
}

bool Plateau::isInsideBoard(sf::Vector2i cell) const {
    return std::find(cellCoords.begin(), cellCoords.end(), cell) != cellCoords.end();
}

std::size_t Plateau::pieceAt(sf::Vector2i cell) const {
    for (std::size_t index = 0; index < pieces.size(); ++index) {
        if (pieces[index].isAlive() && pieces[index].getCell() == cell) {
            return index;
        }
    }
    return pieces.size();
}

bool Plateau::isOccupied(sf::Vector2i cell) const {
    return pieceAt(cell) != pieces.size();
}

bool Plateau::isFriendly(sf::Vector2i cell, PlayerId owner) const {
    const auto index = pieceAt(cell);
    return index != pieces.size() && pieces[index].getOwner() == owner;
}

bool Plateau::isEnemy(sf::Vector2i cell, PlayerId owner) const {
    const auto index = pieceAt(cell);
    return index != pieces.size() && pieces[index].getOwner() != owner;
}

void Plateau::debugClearPieces() {
    pieces.clear();
    clearSelection();
}

void Plateau::debugAddPiece(PieceType type, PlayerId owner, sf::Vector2i cell, bool moved, bool enPassant) {
    const sf::Color color = ownerColor(owner);
    const float radius = hexRadius * 0.50f;
    pieces.emplace_back(type, owner, cell, cellCenter(cell), radius, color);
    pieces.back().setHasMoved(moved);
    pieces.back().setEnPassantEligible(enPassant);
}

void Plateau::debugSetCurrentPlayer(PlayerId owner) {
    currentPlayer = owner;
}

void Plateau::advanceTurn() {
    switch (currentPlayer) {
        case PlayerId::Red:
            currentPlayer = PlayerId::White;
            break;
        case PlayerId::White:
            currentPlayer = PlayerId::Black;
            break;
        case PlayerId::Black:
            currentPlayer = PlayerId::Red;
            break;
    }
}
