#include <SFML/Graphics.hpp>

#include <array>
#include <iostream>

#include "GameController.hpp"
#include "GameView.hpp"
#include "RuntimeConfig.hpp"
#include "StartupMenu.hpp"
#include <exception>

int main() {
    std::freopen("run_stdout.txt", "w", stdout);
    std::freopen("run_stderr.txt", "w", stderr);

    std::set_terminate([]() {
        std::fprintf(stderr, "[FATAL] terminate called\n");
        std::fflush(stderr);
        std::abort();
    });

    Plateau::setDebugLoggingEnabled(false);

    try {
    const RuntimeConfig runtimeConfig = loadRuntimeConfig("yalta_config.ini");

    sf::RenderWindow window(sf::VideoMode({1200u, 900u}), "YaltaChess SFML 3");
    window.setFramerateLimit(60);

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
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "[EXCEPTION] %s\n", ex.what());
        std::fflush(stderr);
        return 1;
    } catch (...) {
        std::fprintf(stderr, "[EXCEPTION] unknown\n");
        std::fflush(stderr);
        return 1;
    }
}