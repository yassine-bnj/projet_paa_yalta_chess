#include <SFML/Graphics.hpp>
#include <iostream>
int main() {
    std::cout << "SFML Test Start..." << std::endl;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    std::cout << "Window created. Close it to exit." << std::endl;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();
        window.clear(sf::Color::Blue);
        window.display();
    }
    return 0;
}