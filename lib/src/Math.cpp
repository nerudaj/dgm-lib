#include <DGM/classes/Math.hpp>

namespace dgm
{

    sf::Vector2f Math::getClosestPointOnLine(
        const Math::Line& line, const sf::Vector2f& point) noexcept
    {
        if (line.a == 0.f)
            return { point.x, 0.f };
        else if (line.b == 0.f)
            return { 0.f, point.y };

        const float c2 = line.b * point.x - line.a * point.y;
        const float y = -(line.a * c2 + line.b * line.c)
                        / (line.a * line.a + line.b * line.b);
        const auto&& result =
            sf::Vector2f { (-line.b * y - line.c) / line.a, y };
        return result;
    }

    std::optional<sf::Vector2f> Math::getIntersection(
        const Math::Line& line1, const Math::Line& line2) noexcept
    {
        // Parallel lines
        if ((line1.a == 0.f && line2.a == 0.f)
            || (line1.b == 0.f && line2.b == 0.f)
            || (line1.a / line2.a == line1.b / line2.b))
            return {};

        if (line1.a == 0.f)
        {
            // line1.b is not zero because that violates precondition
            // that a,b cannot be zero at the same time
            // line2.a cannot be zero because they would be parallel
            // and processed by an earlier condition
            return sf::Vector2f { (line2.b * line1.c - line1.b * line2.c)
                                      / (line2.a * line1.b),
                                  -line1.c / line1.b };
        }
        else if (line1.b == 0.f)
        {
            // Same justification for nonzero as in previous condition
            return sf::Vector2f { -line1.c / line1.a,
                                  (line1.c * line2.a - line1.a * line2.c)
                                      / (line1.a * line2.b) };
        }

        // line1.a and line1.b are guaranteed to not be zero
        // at least one of line2.a/b are non-zero
        const float y = (line1.c * line2.a - line1.a * line2.c)
                        / (line1.a * line2.b - line1.b * line2.a);
        return sf::Vector2f { (-line1.b * y - line1.c) / line1.a, y };
    }

    std::optional<std::pair<sf::Vector2f, sf::Vector2f>>
    Math::getIntersection(const Line& l, const dgm::Circle& c)
    {

        const float m = c.getPosition().x;
        const float n = c.getPosition().y;
        const float r = c.getRadius();

        if (l.a == 0.f)
        {
            const float b2 = l.b * l.b; // A
            const float B = -2 * b2 * m;
            const float C = b2 * m * m + l.c * l.c + 2 * l.b * l.c * n
                            + b2 * n * n - b2 * r * r;
            const float D = B * B - 4 * b2 * C;

            if (D < 0.f) return std::nullopt;

            const float x1 = (-B + std::sqrt(D)) / (2 * b2);
            const float x2 = (-B - std::sqrt(D)) / (2 * b2);
            const float y = -l.c / l.b;
            return std::pair { sf::Vector2f { x1, y }, sf::Vector2f { x2, y } };
        }

        const float a2 = l.a * l.a;
        const float A = a2 + l.b * l.b;
        const float B = 2 * (l.b * l.c + l.a * l.b * m - a2 * n);
        const float C =
            l.c * l.c + 2 * l.a * l.c * m + a2 * (m * m + n * n - r * r);
        const float D = B * B - 4 * A * C;

        if (D < 0.f) return std::nullopt;

        const float y1 = (-B + std::sqrt(D)) / (2 * A);
        const float y2 = (-B - std::sqrt(D)) / (2 * A);
        const float x1 = -(l.b * y1 + l.c) / l.a;
        const float x2 = -(l.b * y2 + l.c) / l.a;

        return std::pair { sf::Vector2f { x1, y1 }, sf::Vector2f { x2, y2 } };
    }

    bool Math::hasIntersection(const Line& line, const dgm::Circle& circle)
    {
        return getSize(
                   getClosestPointOnLine(line, circle.getPosition())
                   - circle.getPosition())
               < circle.getRadius();
    }

    float Math::getAngle(const float x, const float y) {
        if (x == 0.f && y < 0.f)
            return 3.f * std::numbers::pi_v<float> / 2.f;
        else if (x == 0.f && y >= 0.f)
            return std::numbers::pi_v<float> / 2.f;
        else if (y == 0.f && x < 0.f)
            return std::numbers::pi_v<float>;
        else if (y == 0.f && x >= 0.f)
            return 0.f;
        return std::atan2(y, x);
    }

} // namespace dgm
