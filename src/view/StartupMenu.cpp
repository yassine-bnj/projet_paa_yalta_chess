#include "StartupMenu.hpp"

#include <iostream>

namespace {
sf::Vector2f cardPosition(std::size_t index) {
    return sf::Vector2f(110.f + static_cast<float>(index) * 330.f, 260.f);
}
}

StartupMenu::StartupMenu()
    : selectedMode(Mode::HumanVsAi),
      readyToStart(false) {
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
                std::cout << "[Menu] Impossible de charger la police d'accueil\n";
    }

    titleText.emplace(font, "YaltaChess", 42);
    titleText->setPosition(sf::Vector2f(390.f, 70.f));
    titleText->setFillColor(sf::Color(250, 245, 235));

    subtitleText.emplace(font, "Choisis un mode de jeu avant de lancer la partie", 22);
    subtitleText->setPosition(sf::Vector2f(300.f, 135.f));
    subtitleText->setFillColor(sf::Color(210, 215, 225));

    helpText.emplace(font, "Clique sur un mode puis sur Lancer la partie", 18);
    helpText->setPosition(sf::Vector2f(360.f, 780.f));
    helpText->setFillColor(sf::Color(190, 195, 205));

    const char* labels[3] = {
        "3 joueurs humains",
        "Humain contre IA",
        "3 joueurs IA"
    };

    const char* descriptions[3] = {
        "Aucune IA active.\nTous les coups sont joues manuellement.",
        "Rouge et Noir controles par l'IA.\nBlanc reste jouable a la souris.",
        "Les trois joueurs sont controles par l'IA.\nMode demo automatique.",
    };

    for (std::size_t index = 0; index < 3; ++index) {
        modeCards[index].setSize(sf::Vector2f(280.f, 320.f));
        modeCards[index].setPosition(cardPosition(index));
        modeCards[index].setOutlineThickness(3.f);
        modeCards[index].setOutlineColor(sf::Color(255, 255, 255, 60));

        modeTexts[index].emplace(font, std::string(labels[index]) + "\n\n" + descriptions[index], 20);
        modeTexts[index]->setPosition(cardPosition(index) + sf::Vector2f(18.f, 20.f));
        modeTexts[index]->setFillColor(sf::Color(240, 240, 240));
        modeTexts[index]->setLineSpacing(1.2f);
    }

    startButton.setSize(sf::Vector2f(280.f, 64.f));
    startButton.setPosition(sf::Vector2f(460.f, 670.f));
    startButton.setOutlineThickness(2.f);
    startButton.setOutlineColor(sf::Color(255, 255, 255, 100));

    startText.emplace(font, "Lancer la partie", 24);
    startText->setPosition(sf::Vector2f(500.f, 684.f));
    startText->setFillColor(sf::Color::White);

    setupLayout();
}

void StartupMenu::setupLayout() {
    updateSelectionVisuals();
}

std::array<bool, 3> StartupMenu::selectedAiPlayers() const {
    switch (selectedMode) {
        case Mode::Human3:
            return {false, false, false};
        case Mode::Ai3:
            return {true, true, true};
        case Mode::HumanVsAi:
        default:
            return {true, false, true};
    }
}

void StartupMenu::updateSelectionVisuals() {
    const std::array<bool, 3> selected = selectedAiPlayers();
    for (std::size_t index = 0; index < 3; ++index) {
        if ((selectedMode == Mode::Human3 && index == 0) ||
            (selectedMode == Mode::HumanVsAi && index == 1) ||
            (selectedMode == Mode::Ai3 && index == 2)) {
            modeCards[index].setFillColor(sf::Color(60, 120, 180, 210));
            modeCards[index].setOutlineColor(sf::Color(255, 215, 120, 220));
        } else {
            modeCards[index].setFillColor(sf::Color(30, 34, 46, 210));
            modeCards[index].setOutlineColor(sf::Color(255, 255, 255, 60));
        }
    }

    if (readyToStart) {
        startButton.setFillColor(sf::Color(80, 160, 100, 230));
    } else {
        startButton.setFillColor(sf::Color(45, 55, 70, 230));
    }
}

bool StartupMenu::isPointInside(const sf::RectangleShape& shape, sf::Vector2f position) const {
    const sf::FloatRect bounds = shape.getGlobalBounds();
    return bounds.contains(position);
}

void StartupMenu::handleClick(sf::Vector2f position) {
    for (std::size_t index = 0; index < 3; ++index) {
        if (isPointInside(modeCards[index], position)) {
            if (index == 0) {
                selectedMode = Mode::Human3;
            } else if (index == 1) {
                selectedMode = Mode::HumanVsAi;
            } else {
                selectedMode = Mode::Ai3;
            }
            updateSelectionVisuals();
            return;
        }
    }

    readyToStart = isPointInside(startButton, position);
    updateSelectionVisuals();
}

void StartupMenu::draw(sf::RenderWindow& window) const {
    window.clear(sf::Color(18, 22, 30));

    sf::RectangleShape background(sf::Vector2f(1200.f, 900.f));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    sf::CircleShape glow(180.f);
    glow.setFillColor(sf::Color(255, 215, 120, 28));
    glow.setPosition(sf::Vector2f(40.f, 20.f));
    window.draw(glow);

    glow.setRadius(140.f);
    glow.setPosition(sf::Vector2f(860.f, 560.f));
    glow.setFillColor(sf::Color(90, 150, 255, 24));
    window.draw(glow);

    if (titleText.has_value()) {
        window.draw(*titleText);
    }
    if (subtitleText.has_value()) {
        window.draw(*subtitleText);
    }

    for (const auto& card : modeCards) {
        window.draw(card);
    }
    for (const auto& text : modeTexts) {
        if (text.has_value()) {
            window.draw(*text);
        }
    }

    window.draw(startButton);
    if (startText.has_value()) {
        window.draw(*startText);
    }
    if (helpText.has_value()) {
        window.draw(*helpText);
    }

    window.display();
}

std::array<bool, 3> StartupMenu::run(sf::RenderWindow& window) {
    while (window.isOpen() && !readyToStart) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            if (event->is<sf::Event::MouseButtonPressed>()) {
                handleClick(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
            }

            if (event->is<sf::Event::KeyPressed>()) {
                const auto* key = event->getIf<sf::Event::KeyPressed>();
                if (key != nullptr) {
                    if (key->code == sf::Keyboard::Key::Num1) {
                        selectedMode = Mode::Human3;
                        updateSelectionVisuals();
                    } else if (key->code == sf::Keyboard::Key::Num2) {
                        selectedMode = Mode::HumanVsAi;
                        updateSelectionVisuals();
                    } else if (key->code == sf::Keyboard::Key::Num3) {
                        selectedMode = Mode::Ai3;
                        updateSelectionVisuals();
                    } else if (key->code == sf::Keyboard::Key::Enter || key->code == sf::Keyboard::Key::Space) {
                        readyToStart = true;
                        updateSelectionVisuals();
                    }
                }
            }
        }

        draw(window);
    }

    return selectedAiPlayers();
}
