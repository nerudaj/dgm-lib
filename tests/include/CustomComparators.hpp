#pragma once

#define COMPARE_UNSIGNED_VECTORS(v1, v2)                                       \
    REQUIRE(v1.x == v2.x);                                                     \
    REQUIRE(v1.y == v2.y);

#define COMPARE_SIGNED_VECTORS(v1, v2)                                         \
    REQUIRE(v1.x == v2.x);                                                     \
    REQUIRE(v1.y == v2.y);

#define COMPARE_INTRECTS(r1, r2)                                               \
    REQUIRE(r1.left == r2.left);                                               \
    REQUIRE(r1.top == r2.top);                                                 \
    REQUIRE(r1.width == r2.width);                                             \
    REQUIRE(r1.height == r2.height);