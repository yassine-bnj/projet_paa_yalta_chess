#include "GameView.hpp"

#include <iostream>

namespace {
std::string playerToFrench(PlayerId player) {
    switch (player) {
        case PlayerId::Red:
            return "Rouge";
        case PlayerId::White:
            return "Blanc";
        case PlayerId::Black:
        default:
            return "Noir";
    }
}
}

GameView::GameView(Plateau& plateauRef)
    : plateau(plateauRef) {
    const bool loaded = hudFont.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (loaded) {
        turnText.emplace(hudFont, "", 24);
        turnText->setFillColor(sf::Color(245, 245, 245));
        turnText->setPosition(sf::Vector2f(24.f, 14.f));
        updateTurnText(plateau.getCurrentPlayer());
    }
    plateau.addObserver(this);
}

GameView::~GameView() {
    plateau.removeObserver(this);
}

void GameView::onPlateauEvent(const PlateauEvent& event) {
    std::string eventMessage;
    
    switch (event.type) {
        case PlateauEventType::TurnChanged:
            if (!gameOver) {
                updateTurnText(event.currentPlayer);
                statusPrefix.clear();
            }
            break;
        case PlateauEventType::Check:
            eventMessage = "ATTENTION! " + playerToFrench(event.currentPlayer) + " est en ECHEC!";
            break;
        case PlateauEventType::Checkmate:
            eventMessage = "ECHEC ET MAT contre " + playerToFrench(event.currentPlayer) + "!";
            statusPrefix = "Mat: ";
            break;
        case PlateauEventType::PlayerEliminated:
            eventMessage = "Joueur " + playerToFrench(event.currentPlayer) + " elimine du jeu.";
            statusPrefix = "Eliminated: ";
            break;
        case PlateauEventType::WinnerDeclared:
            gameOver = true;
            updateTurnText(event.currentPlayer);
            eventMessage = "VICTOIRE! Joueur " + playerToFrench(event.currentPlayer) + " remporte la partie!";
            if (turnText.has_value()) {
                turnText->setString("Victoire du joueur: " + playerToFrench(event.currentPlayer));
            }
            break;
        case PlateauEventType::CapturePlayed:
            if (event.from.has_value() && event.to.has_value()) {
                eventMessage = "Capture! " + playerToFrench(event.currentPlayer) + " prend une piece.";
            }
            break;
        case PlateauEventType::PieceSelected:
            eventMessage = playerToFrench(event.currentPlayer) + " selectionne une piece.";
            break;
        default:
            break;
    }

    if (!eventMessage.empty()) {
        std::cout << "[" << playerToFrench(event.currentPlayer) << "] " << eventMessage << '\n';
    }
    
    modelDirty = true;
}

void GameView::updateTurnText(PlayerId player) {
    if (turnText.has_value()) {
        turnText->setString(statusPrefix + "Tour du joueur: " + playerToFrench(player));
    }
}

void GameView::render(sf::RenderWindow& window) {
    if (!modelDirty) {
        return;
    }

    window.clear(sf::Color(33, 37, 41));
    plateau.draw(window);

    sf::RectangleShape hudBackground(sf::Vector2f(330.f, 48.f));
    hudBackground.setPosition(sf::Vector2f(12.f, 8.f));
    if (gameOver) {
        hudBackground.setFillColor(sf::Color(90, 25, 25, 205));
    } else {
        hudBackground.setFillColor(sf::Color(20, 20, 25, 190));
    }
    hudBackground.setOutlineThickness(1.f);
    hudBackground.setOutlineColor(sf::Color(230, 230, 230, 60));
    if (turnText.has_value()) {
        window.draw(hudBackground);
        window.draw(*turnText);
    }

    window.display();
    modelDirty = false;
}
