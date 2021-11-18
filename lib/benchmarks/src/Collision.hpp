#pragma once

#include <DGM/dgm.hpp>

bool OldCollisionBasic(const dgm::Rect& rect, const sf::Vector2i& point);
bool NewCollisionBasic(const dgm::Rect& rect, const sf::Vector2i& point);
bool OldCollisionBasic(const dgm::Circle& circle, const sf::Vector2i& point);
bool NewCollisionBasic(const dgm::Circle& circle, const sf::Vector2i& point);
bool OldCollisionBasic(const dgm::Circle& A, const dgm::Circle& B);
bool NewCollisionBasic(const dgm::Circle& A, const dgm::Circle& B);
bool OldCollisionBasic(const dgm::Rect& A, const dgm::Circle& B);
bool NewCollisionBasic(const dgm::Rect& A, const dgm::Circle& B);
bool OldCollisionBasic(const dgm::Rect& A, const dgm::Rect& B);
bool NewCollisionBasic(const dgm::Rect& A, const dgm::Rect& B);
