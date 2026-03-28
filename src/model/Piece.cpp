#include "Piece.hpp"

Piece::Piece(PieceType pieceType, PlayerId pieceOwner, sf::Vector2i pieceCell, sf::Vector2f position, float radius, sf::Color color)
    : type(pieceType), owner(pieceOwner), cell(pieceCell), alive(true), moved(false), enPassantEligible(false) {
    shape.setRadius(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition(position);
    shape.setFillColor(color);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color::Black);
}

PieceType Piece::getType() const {
    return type;
}

PlayerId Piece::getOwner() const {
    return owner;
}

sf::Vector2i Piece::getCell() const {
    return cell;
}

bool Piece::isAlive() const {
    return alive;
}

bool Piece::hasMoved() const {
    return moved;
}

bool Piece::isEnPassantEligible() const {
    return enPassantEligible;
}

void Piece::setCell(sf::Vector2i pieceCell, sf::Vector2f position) {
    cell = pieceCell;
    shape.setPosition(position);
}

void Piece::setAlive(bool isAlive) {
    alive = isAlive;
}

void Piece::setHasMoved(bool isMoved) {
    moved = isMoved;
}

void Piece::setEnPassantEligible(bool eligible) {
    enPassantEligible = eligible;
}

void Piece::draw(sf::RenderTarget& target) const {
    if (alive) {
        target.draw(shape);
    }
}
