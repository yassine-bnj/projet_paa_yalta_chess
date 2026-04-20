#include <SFML/Graphics.hpp>

#include "GameController.hpp"
#include "GameView.hpp"
#include "RuntimeConfig.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1200u, 900u}), "YaltaChess SFML 3");
    window.setFramerateLimit(60);

    Plateau plateau;
    const RuntimeConfig runtimeConfig = loadRuntimeConfig("yalta_config.ini");
    GameController controller(window, plateau, runtimeConfig.aiPlayers, runtimeConfig.aiSearch);
    GameView view(plateau);

    while (window.isOpen()) {
        controller.handleEvents();

        view.render(window);
    }

    return 0;
}
