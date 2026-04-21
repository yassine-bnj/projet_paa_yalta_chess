#include "Piece.hpp"

#include <string>

namespace {
sf::Font& labelFont() {
    static sf::Font font;
    return font;
}

bool labelFontLoaded() {
    static const bool loaded = labelFont().openFromFile("C:/Windows/Fonts/arial.ttf");
    return loaded;
}

char labelForType(PieceType type) {
    switch (type) {
        case PieceType::Pawn:
            return 'p';
        case PieceType::Rook:
            return 'r';
        case PieceType::Knight:
            return 'n';
        case PieceType::Bishop:
            return 'b';
        case PieceType::Queen:
            return 'q';
        case PieceType::King:
        default:
            return 'k';
    }
}

sf::Color labelColorForOwner(PlayerId owner) {
    if (owner == PlayerId::White) {
        return sf::Color::Black;
    }
    return sf::Color::White;
}
}

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

char Piece::getLabel() const {
    return labelForType(type);
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

void Piece::setType(PieceType pieceType) {
    type = pieceType;
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

        if (!labelFontLoaded()) {
            return;
        }

        sf::Text label(labelFont(), std::string(1, getLabel()), 20);
        label.setFillColor(labelColorForOwner(owner));
        label.setOutlineColor(sf::Color::Black);
        label.setOutlineThickness(1.f);

        const sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        label.setPosition(shape.getPosition());
        target.draw(label);
    }
}
