#include <DGM/classes/Traits.hpp>
#include <catch2/catch_all.hpp>
#include <memory>

class A
{
};

class B
{
public:
    A operator*()
    {
        return A();
    }
};

TEST_CASE("SmartPtrType", "[Traits]")
{
    static_assert(!dgm::SmartPtrType<int>);
    static_assert(!dgm::SmartPtrType<A>);
    static_assert(!dgm::SmartPtrType<B>);
    static_assert(!dgm::SmartPtrType<B*>);

    static_assert(dgm::SmartPtrType<std::unique_ptr<int>>);
    static_assert(dgm::SmartPtrType<std::unique_ptr<A>>);
    static_assert(dgm::SmartPtrType<std::unique_ptr<B>>);
    static_assert(dgm::SmartPtrType<std::unique_ptr<B*>>);

    static_assert(dgm::SmartPtrType<std::shared_ptr<int>>);
    static_assert(dgm::SmartPtrType<std::shared_ptr<A>>);
    static_assert(dgm::SmartPtrType<std::shared_ptr<B>>);
    static_assert(dgm::SmartPtrType<std::shared_ptr<B*>>);

    static_assert(dgm::SmartPtrType<std::weak_ptr<int>>);
    static_assert(dgm::SmartPtrType<std::weak_ptr<A>>);
    static_assert(dgm::SmartPtrType<std::weak_ptr<B>>);
    static_assert(dgm::SmartPtrType<std::weak_ptr<B*>>);
}

TEST_CASE("SfmlVectorType", "[Traits]")
{
    static_assert(!dgm::SfmlVectorType<int>);
    static_assert(!dgm::SfmlVectorType<A>);
    static_assert(!dgm::SfmlVectorType<std::pair<int, int>>);

    static_assert(dgm::SfmlVectorType<sf::Vector2<int>>);
    static_assert(dgm::SfmlVectorType<sf::Vector2u>);
    static_assert(dgm::SfmlVectorType<sf::Vector2f>);
}
