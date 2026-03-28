#include <SFML/Graphics.hpp>

#include "GameController.hpp"
#include "GameView.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1200u, 900u}), "YaltaChess SFML 3");
    window.setFramerateLimit(60);

    Plateau plateau;
    GameController controller(window, plateau);
    GameView view(plateau);

    while (window.isOpen()) {
        controller.handleEvents();

        view.render(window);
    }

    return 0;
}
