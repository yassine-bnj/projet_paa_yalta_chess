#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Case {
public:
    Case(sf::Vector2f center, float radius, sf::Color color);
    Case(const std::vector<sf::Vector2f>& points, sf::Color color);

    void draw(sf::RenderTarget& target) const;
    bool contains(sf::Vector2f point) const;
    sf::Vector2f getCenter() const;
    float getRadius() const;
    sf::Color getFillColor() const;
    std::vector<sf::Vector2f> getWorldPoints() const;

private:
    sf::ConvexShape shape;
    sf::Vector2f center;
    float radius;
};
