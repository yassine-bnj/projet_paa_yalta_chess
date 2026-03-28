#include "Plateau.hpp"

int Plateau::sideOf(PlayerId owner) const {
    if (owner == PlayerId::White) {
        return 1;
    }
    if (owner == PlayerId::Red) {
        return 2;
    }
    return 3;
}

int Plateau::determineSubMatrix(int x, int y) const {
    if (x < 4 && y < 4) return 1;
    if (x < 4 && y >= 4 && y < 8) return 2;
    if (y < 4 && x >= 4 && x < 8) return 3;
    if (y >= 7 && x >= 4 && x < 8) return 4;
    if (x >= 7 && y >= 4 && y < 8) return 5;
    if (x >= 7 && y >= 7) return 6;
    return 0;
}

bool Plateau::isOutOfBoard(int x, int y) const {
    return x < 0 || x >= 12 || y < 0 || y >= 12;
}

bool Plateau::isSquareExist(int x, int y) const {
    return (x < 4 && y > 7) || (x > 7 && y < 4);
}

bool Plateau::isWallInMatrix5(int xStart, int yStart, int xMove, int yMove) const {
    return (yStart == 7 && (yStart + yMove == 8)) && ((xMove == 0 && yMove == 1) || (xMove == -1 && yMove == 1) || (xMove == 1 && yMove == 1));
}

bool Plateau::isWallInMatrix4(int xStart, int yStart, int xMove, int yMove) const {
    return (xStart == 7 && (xStart + xMove == 8)) && ((xMove == 1 && yMove == -1) || (xMove == 1 && yMove == 0) || (xMove == 1 && yMove == 1));
}

void Plateau::adjustCoordinates(int xStart, int yStart, int& xDestination, int& yDestination, int& xMove, int& yMove) const {
    const int matrixStart = determineSubMatrix(xStart, yStart);
    const int matrixDestination = determineSubMatrix(xDestination, yDestination);

    if ((xDestination > 3 && xDestination < 8) && (yDestination > 3 && yDestination < 8)) {
        if (xMove == -1 && yMove == 0) {
            xDestination -= 4;
        } else if (xMove == 0 && yMove == 1) {
            yDestination += 4;
        } else if (xMove == 1 && yMove == 0) {
            xDestination += 4;
        } else if (xMove == 0 && yMove == -1) {
            yDestination -= 4;
        } else if (xMove == -1 && yMove == -1) {
            if (matrixStart == 4) {
                yDestination -= 4;
            } else if (matrixStart == 5) {
                xDestination -= 4;
            }
        } else if (xMove == -1 && yMove == 1) {
            if (matrixStart == 3) {
                yDestination += 4;
            } else if (matrixStart == 5) {
                xDestination -= 4;
            }
        } else if (xMove == 1 && yMove == 1) {
            if (matrixStart == 2) {
                xDestination += 4;
            } else if (matrixStart == 3) {
                yDestination += 4;
            }
        } else if (xMove == 1 && yMove == -1) {
            if (matrixStart == 2) {
                xDestination += 4;
            } else if (matrixStart == 4) {
                yDestination -= 4;
            }
        }
    } else if ((matrixStart == 6 && matrixDestination == 5) || (matrixStart == 5 && yDestination == 3)) {
        yDestination = 11 - yDestination;
        if ((xMove == 0 || xMove == 1 || xMove == -1) && yMove == -1) {
            yMove = 1;
        }
    } else if ((matrixStart == 6 && matrixDestination == 4) || (matrixStart == 4 && xDestination == 3)) {
        xDestination = 11 - xDestination;
        if (xMove == -1 && (yMove == 0 || yMove == -1 || yMove == 1)) {
            xMove = 1;
        }
    }
}

bool Plateau::isCenterDiagonalTransition(int xStart, int yStart, int xMove, int yMove) const {
    return (xMove == 1 && yMove == 1 && xStart == 3 && yStart == 3) ||
           (xMove == 1 && yMove == -1 && xStart == 3 && yStart == 4) ||
           (xMove == -1 && yMove == 1 && xStart == 4 && yStart == 3) ||
           (xMove == -1 && yMove == -1 && xStart == 8 && yStart == 8) ||
           (xMove == -1 && yMove == -1 && xStart == 8 && yStart == 4) ||
           (xMove == -1 && yMove == -1 && xStart == 4 && yStart == 8);
}

std::vector<sf::Vector2i> Plateau::centerTransitionDestinations(int xStart, int yStart, int xMove, int yMove) const {
    if (xMove == 1 && yMove == 1 && xStart == 3 && yStart == 3) {
        return {{4, 8}, {8, 4}};
    }
    if (xMove == 1 && yMove == -1 && xStart == 3 && yStart == 4) {
        return {{8, 8}, {4, 3}};
    }
    if (xMove == -1 && yMove == 1 && xStart == 4 && yStart == 3) {
        return {{8, 8}, {3, 4}};
    }
    if (xMove == -1 && yMove == -1 && xStart == 8 && yStart == 8) {
        return {{3, 4}, {4, 3}};
    }
    if (xMove == -1 && yMove == -1 && xStart == 8 && yStart == 4) {
        return {{4, 8}, {3, 3}};
    }
    if (xMove == -1 && yMove == -1 && xStart == 4 && yStart == 8) {
        return {{8, 4}, {3, 3}};
    }

    return {};
}

std::vector<sf::Vector2i> Plateau::centerCaptureTargets(int x, int y) const {
    if (x == 3 && y == 3) {
        return {{4, 8}, {8, 4}};
    }
    if ((x == 3 && y == 4) || (x == 4 && y == 3)) {
        return {{8, 8}};
    }
    if (x == 8 && y == 8) {
        return {{3, 4}, {4, 3}};
    }
    if (x == 8 && y == 4) {
        return {{4, 8}, {3, 3}};
    }
    if (x == 4 && y == 8) {
        return {{8, 4}, {3, 3}};
    }

    return {};
}

std::vector<sf::Vector2i> Plateau::knightCenterTransitions(int x, int y) const {
    if (x == 3 && y == 3) {
        return {{8, 8}};
    }
    if (x == 3 && y == 4) {
        return {{4, 8}};
    }
    if (x == 4 && y == 3) {
        return {{8, 4}};
    }
    if (x == 8 && y == 8) {
        return {{3, 3}};
    }
    if (x == 8 && y == 4) {
        return {{4, 3}};
    }
    if (x == 4 && y == 8) {
        return {{3, 4}};
    }

    return {};
}
