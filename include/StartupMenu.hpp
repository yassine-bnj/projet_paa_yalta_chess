#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <string>

class StartupMenu {
public:
    enum class Mode {
        Human3,
        HumanVsAi,
        Ai3
    };

    StartupMenu();

    std::array<bool, 3> run(sf::RenderWindow& window);

private:
    sf::Font font;
    std::optional<sf::Text> titleText;
    std::optional<sf::Text> subtitleText;
    std::optional<sf::Text> helpText;
    std::array<std::optional<sf::Text>, 3> modeTexts;
    sf::RectangleShape modeCards[3];
    sf::RectangleShape startButton;
    std::optional<sf::Text> startText;

    Mode selectedMode;
    bool readyToStart;

    void setupLayout();
    void updateSelectionVisuals();
    void draw(sf::RenderWindow& window) const;
    void handleClick(sf::Vector2f position);
    bool isPointInside(const sf::RectangleShape& shape, sf::Vector2f position) const;
    std::array<bool, 3> selectedAiPlayers() const;
};
