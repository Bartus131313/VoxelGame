#pragma once

#include <array>

#include "../WorldConfig.h"
#include "../block/BlockState.h"

/**
 * @brief Subchunk stores block states in the smallest volume possible.
 *
 * It belongs to Chunk which then belongs to Region.
 */
class SubChunkData {
public:
    explicit SubChunkData(const uint16_t yPos) : m_yPos(yPos) {};

    /**
     * @brief Fast 3D local coordinate lookup (0-15 range for x, y, z).
     *
     * @return BlockState at given 3D local position.
     */
    [[nodiscard]] BlockState getBlockState(const uint8_t x, const uint8_t y, const uint8_t z) const {
        return m_blockData[(y << 8) | (z << 4) | x];
    }

    /**
     * @brief Safe 1D index getter.
     *
     * @return BlockState at given index or default BlockState (air) if out of bounds.
     */
    [[nodiscard]] BlockState getBlockStateAt(const size_t index) const {
        if (index >= m_blockData.size()) return BlockState{};
        return m_blockData[index];
    }

    /** @brief Fast 3D local coordinate setter. */
    void setBlockState(const uint8_t x, const uint8_t y, const uint8_t z, const BlockState state) {
        m_blockData[(y << 8) | (z << 4) | x] = state;
    }

    /** @brief Safe 1D index setter (does nothing if index is out of bounds). */
    void setBlockStateAt(const size_t index, const BlockState state) {
        if (index >= m_blockData.size()) return;
        m_blockData[index] = state;
    }

    /**
     * @brief Gets Y-pos (vertical index) of this chunk within the chunk column.
     *
     * @return Y-pos of this subchunk (vertical index).
     */
    [[nodiscard]] uint16_t getYPos() const { return m_yPos; }

private:
    uint16_t m_yPos{0}; ///< Vertical subchunk index within the chunk column.

    /// Array storing all block states inside this subchunk (16x16x16 = 4096 blocks).
    std::array<BlockState, WorldConfig::SUBCHUNK_VOLUME> m_blockData{};
};
