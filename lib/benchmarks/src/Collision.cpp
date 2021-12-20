#include "Collision.hpp"

bool OldCollisionBasic(const dgm::Rect& rect, const sf::Vector2i& point) {
    sf::Vector2f pos = rect.getPosition();
    sf::Vector2f sze = rect.getSize();

    return (pos.x <= point.x && point.x <= (pos.x + sze.x) && pos.y <= point.y && point.y <= (pos.y + sze.y));
}

bool NewCollisionBasic(const dgm::Rect& rect, const sf::Vector2i& point) {
    const sf::Vector2f pos = rect.getPosition();
    const sf::Vector2f sze = rect.getSize();
    return (pos.x <= point.x && point.x <= (pos.x + sze.x) && pos.y <= point.y && point.y <= (pos.y + sze.y));
}

bool OldCollisionBasic(const dgm::Circle& circle, const sf::Vector2i& point) {
    sf::Vector2f cpos = circle.getPosition();

    float dX = abs(point.x - cpos.x);
    float dY = abs(point.y - cpos.y);

    return (dX * dX + dY * dY) < (circle.getRadius() * circle.getRadius());
}

bool NewCollisionBasic(const dgm::Circle& circle, const sf::Vector2i& point) {
    const sf::Vector2f cpos = circle.getPosition();
    const float dX = point.x - cpos.x;
    const float dY = point.y - cpos.y;

    return (dX * dX + dY * dY) < (circle.getRadius() * circle.getRadius());
}

bool OldCollisionBasic(const dgm::Circle& A, const dgm::Circle& B) {
    sf::Vector2f cpos1 = A.getPosition();
    sf::Vector2f cpos2 = B.getPosition();

    float dX = abs(cpos1.x - cpos2.x);
    float dY = abs(cpos1.y - cpos2.y);

    if ((dX * dX + dY * dY) < ((A.getRadius() + B.getRadius()) * (A.getRadius() + B.getRadius()))) {
        return true;
    }

    return false;
}

bool NewCollisionBasic(const dgm::Circle& A, const dgm::Circle& B) {
    const sf::Vector2f diff = A.getPosition() - B.getPosition();
    const float rSum = A.getRadius() + B.getRadius();
    return (diff.x * diff.x + diff.y * diff.y) < (rSum * rSum);
}

bool OldCollisionBasic(const dgm::Rect& A, const dgm::Circle& B) {
    sf::Vector2f apos = A.getPosition();
    sf::Vector2f size = A.getSize();
    sf::Vector2f bpos = B.getPosition();
    // Closest coordinates to circle on border of the rect
    // Courtesy of lazyfoo.net tutorials
    float cX = 0.f, cY = 0.f;

    if (bpos.x < apos.x) {
        cX = apos.x;
    }
    else if (bpos.x > (apos.x + size.x)) {
        cX = apos.x + size.x;
    }
    else {
        cX = bpos.x;
    }

    if (bpos.y < apos.y) {
        cY = apos.y;
    }
    else if (bpos.y > (apos.y + size.y)) {
        cY = apos.y + size.y;
    }
    else {
        cY = bpos.y;
    }

    cX = bpos.x - cX;
    cY = bpos.y - cY;

    if (sqrt(cX * cX + cY * cY) < B.getRadius()) {
        return true;
    }

    return false;
}

// I would expect this one to be faster, but it breaks even at best
bool _NewCollisionBasic(const dgm::Rect& A, const dgm::Circle& B) {
    // Adopted algorithm from lazyfoo.net
    const sf::Vector2f apos = A.getPosition();
    const sf::Vector2f size = apos + A.getSize();
    const sf::Vector2f bpos = B.getPosition();

    const float cX = bpos.x - ((bpos.x < apos.x) ? apos.x : ((bpos.x > size.x) ? size.y : bpos.x));
    const float cY = bpos.y - ((bpos.y < apos.y) ? apos.y : ((bpos.y > size.y) ? size.y : bpos.y));

    return (cX * cX + cY * cY) < B.getRadius() * B.getRadius();
}

bool NewCollisionBasic(const dgm::Rect& A, const dgm::Circle& B) {
    // Adopted algorithm from lazyfoo.net

    const sf::Vector2f apos = A.getPosition();
    const sf::Vector2f size = A.getSize();
    const sf::Vector2f bpos = B.getPosition();

    // Do not try to optimize this, benchmarks show no real benefit
    float cX = 0.f, cY = 0.f;
    if (bpos.x < apos.x) {
        cX = apos.x;
    }
    else if (bpos.x > (apos.x + size.x)) {
        cX = apos.x + size.x;
    }
    else {
        cX = bpos.x;
    }

    if (bpos.y < apos.y) {
        cY = apos.y;
    }
    else if (bpos.y > (apos.y + size.y)) {
        cY = apos.y + size.y;
    }
    else {
        cY = bpos.y;
    }

    cX = bpos.x - cX;
    cY = bpos.y - cY;

    return (cX * cX + cY * cY) < B.getRadius() * B.getRadius();
}

bool OldCollisionBasic(const dgm::Rect& A, const dgm::Rect& B) {
    sf::Vector2f cA = A.getPosition() + A.getSize() / 2.f;
    sf::Vector2f cB = B.getPosition() + B.getSize() / 2.f;

    float dX = abs(cA.x - cB.x) * 2.f;
    float dY = abs(cA.y - cB.y) * 2.f;

    if (dX <= (A.getSize().x + B.getSize().x) && dY <= (A.getSize().y + B.getSize().y)) {
        return true;
    }

    return false;
}

bool NewCollisionBasic(const dgm::Rect& A, const dgm::Rect& B) {
    const auto aHalfSize = A.getSize() / 2.f;
    const auto bHalfSize = B.getSize() / 2.f;
    const sf::Vector2f cA = A.getPosition() + aHalfSize;
    const sf::Vector2f cB = B.getPosition() + bHalfSize;

    const float dX = abs(cA.x - cB.x);
    const float dY = abs(cA.y - cB.y);

    return (dX <= (aHalfSize.x + bHalfSize.x) && dY <= (aHalfSize.y + bHalfSize.y));
}