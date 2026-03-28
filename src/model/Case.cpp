#include "Case.hpp"

#include <cmath>
#include <vector>

namespace {
bool pointInPolygon(const std::vector<sf::Vector2f>& polygon, sf::Vector2f point) {
    bool inside = false;
    if (polygon.size() < 3) {
        return false;
    }

    for (std::size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        const bool intersects = ((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
            (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y + 1e-6f) + polygon[i].x);
        if (intersects) {
            inside = !inside;
        }
    }
    return inside;
}
}

Case::Case(sf::Vector2f centerPosition, float hexRadius, sf::Color color)
    : center(centerPosition), radius(hexRadius) {
    shape.setPointCount(6);
    for (std::size_t i = 0; i < 6; ++i) {
        const float angle = -90.f + static_cast<float>(i) * 60.f;
        const float rad = angle * 3.14159265f / 180.f;
        shape.setPoint(i, sf::Vector2f{std::cos(rad) * radius, std::sin(rad) * radius});
    }

    shape.setPosition(center);
    shape.setFillColor(color);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(30, 30, 30));
}

Case::Case(const std::vector<sf::Vector2f>& points, sf::Color color)
    : center(0.f, 0.f), radius(0.f) {
    shape.setPointCount(points.size());
    for (std::size_t i = 0; i < points.size(); ++i) {
        shape.setPoint(i, points[i]);
        center += points[i];
    }

    if (!points.empty()) {
        center /= static_cast<float>(points.size());
    }

    shape.setFillColor(color);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(30, 30, 30));
}

void Case::draw(sf::RenderTarget& target) const {
    target.draw(shape);
}

bool Case::contains(sf::Vector2f point) const {
    std::vector<sf::Vector2f> worldPoints;
    worldPoints.reserve(shape.getPointCount());
    for (std::size_t i = 0; i < shape.getPointCount(); ++i) {
        worldPoints.push_back(shape.getTransform().transformPoint(shape.getPoint(i)));
    }

    return pointInPolygon(worldPoints, point);
}

sf::Vector2f Case::getCenter() const {
    return center;
}

float Case::getRadius() const {
    return radius;
}
