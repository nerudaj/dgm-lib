#pragma once

#define COMPARE_UNSIGNED_VECTORS(v1, v2)                                       \
    REQUIRE(v1.x == v2.x);                                                     \
    REQUIRE(v1.y == v2.y);

#define COMPARE_SIGNED_VECTORS(v1, v2)                                         \
    REQUIRE(v1.x == v2.x);                                                     \
    REQUIRE(v1.y == v2.y);

#define COMPARE_INTRECTS(r1, r2)                                               \
    COMPARE_SIGNED_VECTORS(r1.position, r2.position)                           \
    COMPARE_SIGNED_VECTORS(r1.size, r2.size)
