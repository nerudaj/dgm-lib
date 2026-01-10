#pragma once

#include <SFML/System/Vector2.hpp>
#include <concepts>
#include <memory>

namespace dgm
{
    namespace traits
    {
        template<class T>
        struct is_unique_ptr : std::false_type
        {
        };

        template<class T, class D>
        struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type
        {
        };

        template<class T>
        struct is_shared_ptr : std::false_type
        {
        };

        template<class T>
        struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
        {
        };

        template<class T>
        struct is_weak_ptr : std::false_type
        {
        };

        template<class T>
        struct is_weak_ptr<std::weak_ptr<T>> : std::true_type
        {
        };

        template<class T>
        struct is_sf_vector : std::false_type
        {
        };

        template<class T>
        struct is_sf_vector<sf::Vector2<T>> : std::true_type
        {
        };
    } // namespace traits

    template<class T>
    concept SmartPtrType =
        traits::is_unique_ptr<T>::value || traits::is_shared_ptr<T>::value
        || traits::is_weak_ptr<T>::value;

    template<class T>
    concept SfmlVectorType = traits::is_sf_vector<T>::value;

    /**
     *  Default constructible and swappable
     */
    template<class T>
    concept TrivialType =
        std::is_default_constructible_v<T> && std::is_swappable_v<T>;

    template<class T, class Base>
    concept UniversalReference =
        std::is_same_v<
            typename std::remove_const<
                typename std::remove_reference<T>::type>::type,
            Base>
        || std::convertible_to<T, Base>;

    template<class T>
    concept EnumType = std::is_enum_v<T>;

} // namespace dgm
