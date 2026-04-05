#pragma once

#include <SFML/Graphics.hpp>

#include "Plateau.hpp"

class GameView : public IPlateauObserver {
public:
    explicit GameView(Plateau& plateau);
    ~GameView() override;

    void render(sf::RenderWindow& window);
    void onPlateauEvent(const PlateauEvent& event) override;

private:
    Plateau& plateau;
    bool modelDirty = true;
};
