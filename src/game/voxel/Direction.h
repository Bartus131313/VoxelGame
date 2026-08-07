#pragma once

#include <cstdint>

/** @brief Represents direction of the world. */
enum Direction : uint8_t {
    Up = 0, Down = 1, North = 2, South = 3, East = 4, West = 5, None = 6
};