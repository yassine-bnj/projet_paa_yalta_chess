#pragma once

#include <SFML/Graphics.hpp>

#include <optional>

#include "Plateau.hpp"

class GameView : public IPlateauObserver {
public:
    explicit GameView(Plateau& plateau);
    ~GameView() override;

    void render(sf::RenderWindow& window);
    void onPlateauEvent(const PlateauEvent& event) override;

private:
    Plateau& plateau;
    sf::Font hudFont;
    std::optional<sf::Text> turnText;
    bool gameOver = false;
    bool modelDirty = true;

    void updateTurnText(PlayerId player);
};
