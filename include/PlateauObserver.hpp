#pragma once

#include <SFML/System/Vector2.hpp>

#include <optional>

#include "Piece.hpp"

enum class PlateauEventType {
    BoardReset,
    SelectionChanged,
    PieceSelected,
    InvalidMove,
    CapturePlayed,
    MovePlayed,
    TurnChanged,
    Checkmate,
    PlayerEliminated,
    WinnerDeclared
};

struct PlateauEvent {
    PlateauEventType type;
    std::optional<sf::Vector2i> from;
    std::optional<sf::Vector2i> to;
    PlayerId currentPlayer;
};

class IPlateauObserver {
public:
    virtual ~IPlateauObserver() = default;
    virtual void onPlateauEvent(const PlateauEvent& event) = 0;
};
