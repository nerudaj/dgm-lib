#pragma once

#include <DGM\dgm.hpp>

template<typename T>
inline float dgm::Camera::Effect<T>::update(const dgm::Time& time) {
	elapsed += time.getDeltaTime();
	float f = elapsed / duration;
	return easing(dgm::Math::clamp(f, f, 1.f));
}

template<typename T>
inline void dgm::Camera::Effect<T>::init(T s, T a, const sf::Time& d, EasingFunc f) {
	elapsed = 0.f;
	duration = d.asSeconds();
	start = s;
	amount = a;
	easing = f;
}

void dgm::Camera::update(const dgm::Time& time) {
	if (isMoving()) {
		setPosition(moveEffect.start + moveEffect.update(time) * moveEffect.amount);

		if (isShaking()) shakeEffect.start = view.getCenter();
	}

	if (isZooming()) {
		setZoom(zoomEffect.start + zoomEffect.update(time) * zoomEffect.amount);
	}

	if (isRotating()) {
		setRotation(rotationEffect.start + rotationEffect.update(time) * rotationEffect.amount);
	}

	if (isShaking()) {
		float f = shakeEffect.update(time);
		unsigned vecIndex = unsigned(shakeEffect.elapsed / shakeEffect.hold) % 20;
		setPosition(shakeEffect.start + f * shakeEffect.positions[vecIndex] * shakeEffect.amount.x);
	}
}

void dgm::Camera::moveGradually(const sf::Vector2f& position, const sf::Time& duration, EasingFunc f) {
	if (isMoving()) return;

	moveEffect.init(view.getCenter(), position - view.getCenter(), duration, f);
}

void dgm::Camera::zoomGradually(float level, const sf::Time& duration, EasingFunc f) {
	if (isZooming()) return;

	float start = view.getSize().x / defaultZoomLevel.x;
	zoomEffect.init(start, level - start, duration, f);
}

void dgm::Camera::rotateGradually(float amount, const sf::Time& duration, EasingFunc f) {
	if (isRotating()) return;

	rotationEffect.init(view.getRotation(), amount, duration, f);
}

void dgm::Camera::shake(const sf::Time& duration, float amount, EasingFunc f, const sf::Time& hold) {
	if (isShaking()) return;

	shakeEffect.init(view.getCenter(), { amount, amount }, duration, f);
	shakeEffect.hold = hold.asSeconds();
}
