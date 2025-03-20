#include <DGM/classes/Camera.hpp>
#include <DGM/classes/Time.hpp>
#include <algorithm>
#include <array>

static const std::array<sf::Vector2f, 20> SHAKE_POSITIONS = {
    // Predefined set of directions where shake should go
    sf::Vector2f(0.2f, -0.6f),
    { -1.f, 0.5f },
    { -0.3f, -1.f },
    { 0.5f, -1.f },
    { -0.8f, -0.7f },
    { -0.2f, 0.3f },
    { -0.6f, -0.6f },
    { 0.2f, 0.5f },
    { 0.5f, -0.2f },
    { 1.f, -0.6f },
    { 0.4f, 0.6f },
    { 0.5f, 0.7f },
    { -1.f, -1.f },
    { 0.6f, 0.5f },
    { 0.3f, 0.7f },
    { 0.3f, -0.7f },
    { -0.2f, -0.7f },
    { -0.1f, -0.3f },
    { -0.2f, 0.1f },
    { -0.1f, 0.6f },
}; // we loop over 20 predefined positions

template<typename T>
inline float dgm::Camera::Effect<T>::update(const dgm::Time& time)
{
    elapsed += time.getDeltaTime();
    const float f = elapsed / duration;
    return easing(std::clamp(f, 0.f, 1.f));
}

template<typename T>
inline void
dgm::Camera::Effect<T>::init(T s, T a, const sf::Time& d, EasingFunc f)
{
    elapsed = 0.f;
    duration = d.asSeconds();
    start = s;
    amount = a;
    easing = f;
}

void dgm::Camera::update(const dgm::Time& time)
{
    if (isMoving())
    {
        setPosition(
            moveEffect.start + moveEffect.update(time) * moveEffect.amount);

        if (isShaking()) shakeEffect.start = view.getCenter();
    }

    if (isZooming())
    {
        setZoom(zoomEffect.start + zoomEffect.update(time) * zoomEffect.amount);
    }

    if (isRotating())
    {
        setRotation(
            rotationEffect.start
            + rotationEffect.update(time) * rotationEffect.amount);
    }

    if (isShaking())
    {
        const float f = shakeEffect.update(time);
        const unsigned vecIndex =
            unsigned(shakeEffect.elapsed / shakeEffect.hold) % 20;
        setPosition(
            shakeEffect.start
            + f * SHAKE_POSITIONS[vecIndex] * shakeEffect.amount.x);
    }
}

void dgm::Camera::moveGradually(
    const sf::Vector2f& position, const sf::Time& duration, EasingFunc f)
{
    if (isMoving()) return;

    moveEffect.init(view.getCenter(), position - view.getCenter(), duration, f);
}

void dgm::Camera::zoomGradually(
    float level, const sf::Time& duration, EasingFunc f)
{
    if (isZooming()) return;

    const float start = view.getSize().x / defaultView.getSize().x;
    zoomEffect.init(start, level - start, duration, f);
}

void dgm::Camera::rotateGradually(
    sf::Angle amount, const sf::Time& duration, EasingFunc f)
{
    if (isRotating()) return;

    rotationEffect.init(view.getRotation(), amount, duration, f);
}

void dgm::Camera::shake(
    const sf::Time& duration, float amount, EasingFunc f, const sf::Time& hold)
{
    if (isShaking()) return;

    shakeEffect.init(view.getCenter(), { amount, amount }, duration, f);
    shakeEffect.hold = hold.asSeconds();
}
