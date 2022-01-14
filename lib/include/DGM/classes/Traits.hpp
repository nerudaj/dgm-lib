#pragma once

#include <memory>
#include <SFML/System/Vector2.hpp>

namespace dgm {

template <class T>
struct is_unique_ptr : std::false_type {};

template <class T, class D>
struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type {};

template<class T>
struct is_shared_ptr : std::false_type {};

template<class T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<class T>
struct is_weak_ptr : std::false_type {};

template<class T>
struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {};

template<class T>
concept IsSmartPtr = is_unique_ptr<T>::value || is_shared_ptr<T>::value || is_weak_ptr<T>::value;



template<class T>
struct is_sf_vector : std::false_type {};

template<class T>
struct is_sf_vector<sf::Vector2<T>> : std::true_type {};

template<class T>
concept IsSfVector = is_sf_vector<T>::value;

}
