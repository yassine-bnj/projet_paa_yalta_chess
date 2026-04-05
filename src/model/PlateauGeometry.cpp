#include "Plateau.hpp"

#include <cmath>
#include <queue>
#include <stdexcept>
#include <vector>

namespace {
constexpr float kPi = 3.14159265359f;

sf::Vector2f midpoint(const sf::Vector2f& a, const sf::Vector2f& b) {
    return (a + b) / 2.0f;
}

sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) {
    return a + (b - a) * t;
}

bool nearlyEqual(float a, float b, float epsilon = 0.01f) {
    return std::abs(a - b) <= epsilon;
}

bool samePoint(const sf::Vector2f& a, const sf::Vector2f& b) {
    return nearlyEqual(a.x, b.x) && nearlyEqual(a.y, b.y);
}

bool shareEdge(const std::vector<sf::Vector2f>& a, const std::vector<sf::Vector2f>& b) {
    int sharedVertices = 0;
    for (const auto& pa : a) {
        for (const auto& pb : b) {
            if (samePoint(pa, pb)) {
                ++sharedVertices;
                break;
            }
        }
    }

    return sharedVertices >= 2;
}

std::vector<sf::Vector2f> createMatrixLines(
    const sf::Vector2f& center,
    const sf::Vector2f& mid1,
    const sf::Vector2f& mid2,
    const sf::Vector2f& p1,
    const sf::Vector2f& p2,
    const sf::Vector2f& p3
) {
    std::vector<sf::Vector2f> lines;
    lines.reserve(12);

    for (float ratio : {0.25f, 0.50f, 0.75f}) {
        lines.push_back(lerp(center, mid1, ratio));
        lines.push_back(lerp(p1, p2, (1.0f - ratio) / 2.0f));
    }

    for (float ratio : {0.25f, 0.50f, 0.75f}) {
        lines.push_back(lerp(center, mid2, ratio));
        lines.push_back(lerp(p3, p1, (1.0f + ratio) / 2.0f));
    }

    return lines;
}

std::vector<std::vector<sf::Vector2f>> createMatrixLosanges(
    const sf::Vector2f& center,
    const std::vector<sf::Vector2f>& l,
    const sf::Vector2f& point,
    const sf::Vector2f& mid1,
    const sf::Vector2f& mid2
) {
    if (l.size() != 12) {
        throw std::runtime_error("Invalid matrix line count");
    }

    std::vector<std::vector<sf::Vector2f>> shapes;
    shapes.reserve(16);

    shapes.push_back({center, l[0], lerp(l[6], l[7], 0.25f), l[6]});
    shapes.push_back({l[0], l[2], lerp(l[6], l[7], 0.50f), lerp(l[6], l[7], 0.25f)});
    shapes.push_back({l[2], l[4], lerp(l[6], l[7], 0.75f), lerp(l[6], l[7], 0.50f)});
    shapes.push_back({l[4], mid1, l[7], lerp(l[6], l[7], 0.75f)});

    shapes.push_back({l[6], lerp(l[6], l[7], 0.25f), lerp(l[8], l[9], 0.25f), l[8]});
    shapes.push_back({lerp(l[6], l[7], 0.25f), lerp(l[6], l[7], 0.50f), lerp(l[8], l[9], 0.50f), lerp(l[8], l[9], 0.25f)});
    shapes.push_back({lerp(l[6], l[7], 0.50f), lerp(l[6], l[7], 0.75f), lerp(l[8], l[9], 0.75f), lerp(l[8], l[9], 0.50f)});
    shapes.push_back({lerp(l[6], l[7], 0.75f), l[7], l[9], lerp(l[8], l[9], 0.75f)});

    shapes.push_back({l[8], lerp(l[8], l[9], 0.25f), lerp(l[10], l[11], 0.25f), l[10]});
    shapes.push_back({lerp(l[8], l[9], 0.25f), lerp(l[8], l[9], 0.50f), lerp(l[10], l[11], 0.50f), lerp(l[10], l[11], 0.25f)});
    shapes.push_back({lerp(l[8], l[9], 0.50f), lerp(l[8], l[9], 0.75f), lerp(l[10], l[11], 0.75f), lerp(l[10], l[11], 0.50f)});
    shapes.push_back({lerp(l[8], l[9], 0.75f), l[9], l[11], lerp(l[10], l[11], 0.75f)});

    shapes.push_back({l[10], lerp(l[10], l[11], 0.25f), l[1], mid2});
    shapes.push_back({lerp(l[10], l[11], 0.25f), lerp(l[10], l[11], 0.50f), l[3], l[1]});
    shapes.push_back({lerp(l[10], l[11], 0.50f), lerp(l[10], l[11], 0.75f), l[5], l[3]});
    shapes.push_back({lerp(l[10], l[11], 0.75f), l[5], point, l[11]});

    return shapes;
}
}

void Plateau::buildBoard() {
    cells.clear();
    cellCoords.clear();

    std::vector<float> sideLengths = {360.f, 368.f, 368.f, 360.f, 368.f, 368.f};
    std::vector<sf::Vector2f> points;
    points.reserve(6);

    float angle = 0.f;
    sf::Vector2f origin(originX, originY);
    points.push_back(origin + sf::Vector2f(sideLengths[0], 0.f));
    for (int i = 1; i < 6; ++i) {
        angle += kPi / 3.f;
        points.push_back(points.back() + sf::Vector2f(std::cos(angle), std::sin(angle)) * sideLengths[i]);
    }

    sf::Vector2f center(0.f, 0.f);
    for (const auto& p : points) {
        center += p;
    }
    center /= static_cast<float>(points.size());

    std::vector<sf::Vector2f> mids = {
        midpoint(points[4], points[3]),
        midpoint(points[3], points[2]),
        midpoint(points[0], points[1]),
        midpoint(points[0], points[5]),
        midpoint(points[5], points[4]),
        midpoint(points[1], points[2])
    };

    std::vector<std::vector<sf::Vector2f>> matrixLines(6);
    matrixLines[0] = createMatrixLines(center, mids[1], mids[0], points[3], points[4], points[2]);
    matrixLines[1] = createMatrixLines(center, mids[5], mids[1], points[2], points[3], points[1]);
    matrixLines[2] = createMatrixLines(center, mids[0], mids[4], points[4], points[5], points[3]);
    matrixLines[3] = createMatrixLines(center, mids[4], mids[3], points[5], points[0], points[4]);
    matrixLines[4] = createMatrixLines(center, mids[2], mids[5], points[1], points[2], points[0]);
    matrixLines[5] = createMatrixLines(center, mids[3], mids[2], points[0], points[1], points[5]);

    std::vector<std::vector<std::vector<sf::Vector2f>>> matrices(6);
    matrices[0] = createMatrixLosanges(center, matrixLines[0], points[3], mids[1], mids[0]);
    matrices[1] = createMatrixLosanges(center, matrixLines[1], points[2], mids[5], mids[1]);
    matrices[2] = createMatrixLosanges(center, matrixLines[2], points[4], mids[0], mids[4]);
    matrices[3] = createMatrixLosanges(center, matrixLines[3], points[5], mids[4], mids[3]);
    matrices[4] = createMatrixLosanges(center, matrixLines[4], points[1], mids[2], mids[5]);
    matrices[5] = createMatrixLosanges(center, matrixLines[5], points[0], mids[3], mids[2]);

    const sf::Color light(0xFE, 0xF7, 0xE5);
    const sf::Color dark(0xEE, 0xCF, 0xA1);

    const auto indexToLegacy = [](int index, int matrixId) -> sf::Vector2i {
        int x = 0;
        int y = 0;
        if (matrixId == 1) {
            y = 3 - (index / 4);
            x = 3 - (index % 4);
        } else if (matrixId == 2) {
            x = 3 - index / 4;
            y = (index % 4) + 4;
        } else if (matrixId == 3) {
            x = index / 4 + 4;
            y = 3 - index % 4;
        } else if (matrixId == 4) {
            y = index / 4 + 8;
            x = index % 4 + 4;
        } else if (matrixId == 5) {
            y = index / 4 + 4;
            x = index % 4 + 8;
        } else if (matrixId == 6) {
            x = index / 4 + 8;
            y = index % 4 + 8;
        }
        return sf::Vector2i{x, y};
    };

    std::vector<std::vector<sf::Vector2f>> allShapes;
    allShapes.reserve(matrixCount * matrixCellCount);
    std::vector<sf::Vector2i> allLegacyCoords;
    allLegacyCoords.reserve(matrixCount * matrixCellCount);

    for (int matrixId = 0; matrixId < matrixCount; ++matrixId) {
        for (int index = 0; index < matrixCellCount; ++index) {
            allShapes.push_back(matrices[matrixId][index]);
            allLegacyCoords.push_back(indexToLegacy(index, matrixId + 1));
        }
    }

    const std::size_t n = allShapes.size();
    std::vector<std::vector<std::size_t>> adjacency(n);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            if (shareEdge(allShapes[i], allShapes[j])) {
                adjacency[i].push_back(j);
                adjacency[j].push_back(i);
            }
        }
    }

    std::vector<int> parity(n, -1);
    for (std::size_t start = 0; start < n; ++start) {
        if (parity[start] != -1) {
            continue;
        }

        std::queue<std::size_t> q;
        parity[start] = 0;
        q.push(start);

        while (!q.empty()) {
            const std::size_t current = q.front();
            q.pop();

            for (const std::size_t neighbor : adjacency[current]) {
                if (parity[neighbor] == -1) {
                    parity[neighbor] = 1 - parity[current];
                    q.push(neighbor);
                }
            }
        }
    }

    for (std::size_t i = 0; i < n; ++i) {
        const sf::Color color = parity[i] == 0 ? light : dark;
        cells.emplace_back(allShapes[i], color);
        cellCoords.push_back(allLegacyCoords[i]);
    }
}

sf::Color Plateau::ownerColor(PlayerId owner) const {
    switch (owner) {
        case PlayerId::Red:
            return sf::Color(205, 70, 70);
        case PlayerId::White:
            return sf::Color(235, 235, 235);
        case PlayerId::Black:
        default:
            return sf::Color(55, 55, 70);
    }
}

sf::Vector2f Plateau::cellCenter(sf::Vector2i cell) const {
    for (std::size_t i = 0; i < cellCoords.size(); ++i) {
        if (cellCoords[i] == cell) {
            return cells[i].getCenter();
        }
    }

    return sf::Vector2f(0.f, 0.f);
}

sf::ConvexShape Plateau::makeHexMarker(sf::Vector2i cell, sf::Color fill, sf::Color outline, float outlineThickness) const {
    sf::ConvexShape marker;
    marker.setPointCount(6);
    const float markerRadius = hexRadius * 0.45f;
    for (std::size_t i = 0; i < 6; ++i) {
        const float angle = -90.f + static_cast<float>(i) * 60.f;
        const float radians = angle * 3.14159265f / 180.f;
        marker.setPoint(i, sf::Vector2f{std::cos(radians) * markerRadius, std::sin(radians) * markerRadius});
    }
    marker.setPosition(cellCenter(cell));
    marker.setFillColor(fill);
    marker.setOutlineColor(outline);
    marker.setOutlineThickness(outlineThickness);
    return marker;
}
