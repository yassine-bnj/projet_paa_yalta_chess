#include <SFML/Graphics.hpp>

#include <array>

#include "GameController.hpp"
#include "GameView.hpp"
#include "RuntimeConfig.hpp"
#include "StartupMenu.hpp"

int main() {
    const RuntimeConfig runtimeConfig = loadRuntimeConfig("yalta_config.ini");

    sf::RenderWindow window(sf::VideoMode({1200u, 900u}), "YaltaChess SFML 3");
    window.setFramerateLimit(60);

    // Always show startup menu for user to choose game mode
    StartupMenu startupMenu;
    const std::array<bool, 3> selectedPlayers = startupMenu.run(window);

    if (!window.isOpen()) {
        return 0;
    }

    Plateau plateau;
    GameView view(plateau);
    GameController controller(window, plateau, selectedPlayers, runtimeConfig.aiSearch, &view);

    while (window.isOpen()) {
        controller.handleEvents();

        view.render(window);
    }

    return 0;
}
