#pragma once

#include <memory>

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

}
