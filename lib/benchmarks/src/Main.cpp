#include <benchmark/benchmark.h>
#include "Collision.hpp"

constexpr unsigned ITEM_COUNT = 256;

auto generateRandomPoints(unsigned num) {
    std::vector<sf::Vector2i> points(num);
    for (auto& p : points) {
        p.x = rand() % 20;
        p.y = rand() % 20;
    }
    return points;
}

auto generateRandomCircles(unsigned num) {
    std::vector<dgm::Circle> circles(num);
    for (auto& c : circles) {
        c.setPosition(float((rand() % 20)), float(rand() % 20));
        c.setRadius(float(rand() % 5));
    }
    return circles;
}

auto generateRandomRects(unsigned num) {
    std::vector<dgm::Rect> rects(num);
    for (auto& r : rects) {
        r.setPosition(float((rand() % 20)), float(rand() % 20));
        r.setSize(float(rand() % 5), float(rand() % 5));
    }
    return rects;
}

static void BenchOldCollisionBasicRectPoint(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Rect& rect = dgm::Rect(sf::Vector2f(5.f, 5.f), sf::Vector2f(15.f, 15.f));
    const auto points = generateRandomPoints(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        OldCollisionBasic(rect, points[i]);
        ++i;
    }
}

static void BenchNewCollisionBasicRectPoint(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Rect& rect = dgm::Rect(sf::Vector2f(5.f, 5.f), sf::Vector2f(15.f, 15.f));
    const auto points = generateRandomPoints(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        NewCollisionBasic(rect, points[i]);
        ++i;
    }
}

static void BenchOldCollisionBasicCirclePoint(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Circle& circ = dgm::Circle(sf::Vector2f(10.f, 10.f), 6.f);
    const auto points = generateRandomPoints(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        OldCollisionBasic(circ, points[i]);
        ++i;
    }
}

static void BenchNewCollisionBasicCirclePoint(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Circle& circ = dgm::Circle(sf::Vector2f(10.f, 10.f), 6.f);
    const auto points = generateRandomPoints(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        NewCollisionBasic(circ, points[i]);
        ++i;
    }
}

static void BenchOldCollisionBasicCircleCircle(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Circle& circ = dgm::Circle(sf::Vector2f(10.f, 10.f), 3.f);
    const auto circles = generateRandomCircles(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        OldCollisionBasic(circ, circles[i]);
        ++i;
    }
}

static void BenchNewCollisionBasicCircleCircle(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Circle& circ = dgm::Circle(sf::Vector2f(10.f, 10.f), 3.f);
    const auto circles = generateRandomCircles(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        NewCollisionBasic(circ, circles[i]);
        ++i;
    }
}

static void BenchOldCollisionBasicRectCircle(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Rect rect = dgm::Rect(sf::Vector2f(7.f, 7.f), sf::Vector2f(7.f, 7.f));
    auto circles = generateRandomCircles(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        OldCollisionBasic(rect, circles[i]);
        ++i;
    }
}

static void BenchNewCollisionBasicRectCircle(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Rect rect = dgm::Rect(sf::Vector2f(7.f, 7.f), sf::Vector2f(7.f, 7.f));
    auto circles = generateRandomCircles(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        NewCollisionBasic(rect, circles[i]);
        ++i;
    }
}

static void BenchOldCollisionBasicRectRect(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Rect rect = dgm::Rect({7.f, 7.f}, {7.f, 7.f});
    const auto rects = generateRandomRects(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        OldCollisionBasic(rect, rects[i]);
        ++i;
    }
}

static void BenchNewCollisionBasicRectRect(benchmark::State& state) {
    srand(time(nullptr));
    const dgm::Rect rect = dgm::Rect({ 7.f, 7.f }, { 7.f, 7.f });
    const auto rects = generateRandomRects(ITEM_COUNT);
    uint8_t i = 0;
    for (auto _ : state) {
        NewCollisionBasic(rect, rects[i]);
        ++i;
    }
}

BENCHMARK(BenchOldCollisionBasicRectPoint);
BENCHMARK(BenchNewCollisionBasicRectPoint);
BENCHMARK(BenchOldCollisionBasicCirclePoint);
BENCHMARK(BenchNewCollisionBasicCirclePoint);
BENCHMARK(BenchOldCollisionBasicCircleCircle);
BENCHMARK(BenchNewCollisionBasicCircleCircle);
BENCHMARK(BenchOldCollisionBasicRectCircle);
BENCHMARK(BenchNewCollisionBasicRectCircle);
BENCHMARK(BenchOldCollisionBasicRectRect);
BENCHMARK(BenchNewCollisionBasicRectRect);

// Run the benchmark
BENCHMARK_MAIN();