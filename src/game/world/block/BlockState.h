#pragma once
#include "../Direction.h"

/** @brief Stores all important data (id, direction, etc.) about one single block. */
struct BlockState {
    uint16_t blockId : 12 {0}; ///< ID of the block (for now there can be max 4 095 blocks, should be enough).

    /// Direction which the block face.
    Direction blockDirection : 3 {None};

    bool isWatered : 1 {false};  ///< Does the block have water in it.

    /** @brief Convert struct directly to raw 16-bit integer. */
    [[nodiscard]] uint16_t toRaw() const {
        return (blockId & 0x0FFF) |
               ((static_cast<uint16_t>(blockDirection) & 0x07) << 12) |
               ((isWatered ? 1u : 0u) << 15);
    }

    /** @brief Construct struct from raw 16-bit integer. */
    [[nodiscard]] static BlockState fromRaw(const uint16_t raw) {
        BlockState state;
        state.blockId = raw & 0x0FFF;
        state.blockDirection = static_cast<Direction>((raw >> 12) & 0x07);
        state.isWatered = (raw >> 15) & 0x01;
        return state;
    }
};