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

TEST_CASE("IsSmartPtr", "[Traits]")
{
    static_assert(!dgm::IsSmartPtr<int>);
    static_assert(!dgm::IsSmartPtr<A>);
    static_assert(!dgm::IsSmartPtr<B>);
    static_assert(!dgm::IsSmartPtr<B*>);

    static_assert(dgm::IsSmartPtr<std::unique_ptr<int>>);
    static_assert(dgm::IsSmartPtr<std::unique_ptr<A>>);
    static_assert(dgm::IsSmartPtr<std::unique_ptr<B>>);
    static_assert(dgm::IsSmartPtr<std::unique_ptr<B*>>);

    static_assert(dgm::IsSmartPtr<std::shared_ptr<int>>);
    static_assert(dgm::IsSmartPtr<std::shared_ptr<A>>);
    static_assert(dgm::IsSmartPtr<std::shared_ptr<B>>);
    static_assert(dgm::IsSmartPtr<std::shared_ptr<B*>>);

    static_assert(dgm::IsSmartPtr<std::weak_ptr<int>>);
    static_assert(dgm::IsSmartPtr<std::weak_ptr<A>>);
    static_assert(dgm::IsSmartPtr<std::weak_ptr<B>>);
    static_assert(dgm::IsSmartPtr<std::weak_ptr<B*>>);
}

TEST_CASE("IsSfVector", "[Traits]")
{
    static_assert(!dgm::IsSfVector<int>);
    static_assert(!dgm::IsSfVector<A>);
    static_assert(!dgm::IsSfVector<std::pair<int, int>>);

    static_assert(dgm::IsSfVector<sf::Vector2<int>>);
    static_assert(dgm::IsSfVector<sf::Vector2u>);
    static_assert(dgm::IsSfVector<sf::Vector2f>);
}
