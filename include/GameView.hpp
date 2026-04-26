#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>

#include "Plateau.hpp"

class GameView : public IPlateauObserver {
public:
    explicit GameView(Plateau& plateau);
    ~GameView() override;

    void render(sf::RenderWindow& window);
    void onPlateauEvent(const PlateauEvent& event) override;
    // Returns true if the event was consumed (used for promotion UI)
    bool handlePromotionEvent(const sf::Event& ev, sf::RenderWindow& window);

private:
    Plateau& plateau;
    sf::Font hudFont;
    std::optional<sf::Text> turnText;
    std::optional<sf::Text> eventText;
    bool gameOver = false;
    std::string statusPrefix;
    std::string statusMessage;
    bool modelDirty = true;
    std::vector<sf::FloatRect> promotionButtons;

    void updateTurnText(PlayerId player);
};
