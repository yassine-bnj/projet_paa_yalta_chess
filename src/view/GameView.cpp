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
        case PlateauEventType::PromotionPlayed:
            eventMessage = "Promotion! " + playerToFrench(event.currentPlayer) + " transforme un pion en reine.";
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

    // If a promotion is pending, render a modal choice overlay
    if (plateau.hasPendingPromotion()) {
        const sf::Vector2u winSize = window.getSize();
        sf::RectangleShape backdrop(sf::Vector2f(static_cast<float>(winSize.x), static_cast<float>(winSize.y)));
        backdrop.setFillColor(sf::Color(0, 0, 0, 160));
        window.draw(backdrop);

        const float boxW = 360.f;
        const float boxH = 120.f;
        const sf::Vector2f boxPos((winSize.x - boxW) / 2.f, (winSize.y - boxH) / 2.f);
        sf::RectangleShape box(sf::Vector2f(boxW, boxH));
        box.setPosition(boxPos);
        box.setFillColor(sf::Color(40, 40, 45, 240));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(200, 200, 200, 80));
        window.draw(box);

        const std::vector<std::string> labels = {"Queen", "Rook", "Bishop", "Knight"};
        promotionButtons.clear();
        const float padding = 16.f;
        const float btnW = (boxW - padding * (labels.size() + 1)) / static_cast<float>(labels.size());
        const float btnH = 56.f;
        for (std::size_t i = 0; i < labels.size(); ++i) {
            const float x = boxPos.x + padding + i * (btnW + padding);
            const float y = boxPos.y + (boxH - btnH) / 2.f;
            sf::RectangleShape btn(sf::Vector2f(btnW, btnH));
            btn.setPosition(sf::Vector2f(x, y));
            btn.setFillColor(sf::Color(70, 70, 80));
            btn.setOutlineThickness(1.f);
            btn.setOutlineColor(sf::Color(200, 200, 200, 80));
            window.draw(btn);

            if (hudFont.getInfo().family.size() > 0) {
                sf::Text t(hudFont, labels[i], 20);
                t.setFillColor(sf::Color::White);
                const auto bounds = t.getLocalBounds();
                const float tw = bounds.size.x;
                const float th = bounds.size.y;
                const float left = bounds.position.x;
                const float top = bounds.position.y;
                t.setPosition(sf::Vector2f(x + (btnW - tw) / 2.f - left,
                                           y + (btnH - th) / 2.f - top - 4.f));
                window.draw(t);
            }

            promotionButtons.emplace_back(sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(btnW, btnH)));
        }
    }

    window.display();
    modelDirty = false;
}

bool GameView::handlePromotionEvent(const sf::Event& ev, sf::RenderWindow& window) {
    if (!plateau.hasPendingPromotion()) {
        return false;
    }

    if (!ev.is<sf::Event::MouseButtonPressed>()) {
        return false;
    }

    const auto* mb = ev.getIf<sf::Event::MouseButtonPressed>();
    if (mb == nullptr) {
        return false;
    }
    const sf::Vector2i mousePos = mb->position;
    for (std::size_t i = 0; i < promotionButtons.size(); ++i) {
        if (promotionButtons[i].contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
            const sf::Vector2i target = plateau.getPendingPromotionCell();
            PieceType chosen;
            switch (i) {
                case 0: chosen = PieceType::Queen; break;
                case 1: chosen = PieceType::Rook; break;
                case 2: chosen = PieceType::Bishop; break;
                default: chosen = PieceType::Knight; break;
            }
            plateau.promotePawnAt(target, chosen);
            modelDirty = true;
            promotionButtons.clear();
            return true;
        }
    }

    return false;
}
