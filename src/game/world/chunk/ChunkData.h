#pragma once

#include <array>
#include <memory>
#include <cstdint>

#include "SubChunkData.h"
#include "../WorldConfig.h"

/**
 * @brief Represents a vertical column of subchunks.
 *
 * Owned by Region. Manages SubChunkData instances lazily.
 */
class ChunkData {
public:
    ChunkData(const int32_t chunkX, const int32_t chunkZ) 
        : m_chunkX(chunkX), m_chunkZ(chunkZ) {}

    /**
     * @brief Retrieves the block state at global vertical coordinates within this column.
     *
     * @return BlockState at given 3D local position or default BlockState (air) if out of bounds.
     */
    [[nodiscard]] BlockState getBlockState(const uint8_t x, const uint16_t y, const uint8_t z) const {
        if (y >= WorldConfig::WORLD_HEIGHT || x >= WorldConfig::SUBCHUNK_SIZE || z >= WorldConfig::SUBCHUNK_SIZE)
            return BlockState{};

        // TODO: If somehow in the future the subchunk size will change, this below NEEDS to be changed too.
        const uint8_t subChunkY = y >> 4; // y / 16
        const uint8_t localY = y & 0x0F;  // y % 16

        const auto& subChunk = m_subChunks[subChunkY];
        if (!subChunk) return BlockState{}; // Empty/Unallocated sky returns default Air state

        return subChunk->getBlockState(x, localY, z);
    }

    /** @brief Sets a block state, allocating the subchunk if it doesn't exist yet. */
    void setBlockState(const uint8_t x, const uint16_t y, const uint8_t z, const BlockState state) {
        if (y >= WorldConfig::WORLD_HEIGHT || x >= WorldConfig::SUBCHUNK_SIZE || z >= WorldConfig::SUBCHUNK_SIZE)
            return;

        // TODO: If somehow in the future the subchunk size will change, this below NEEDS to be changed too.
        const uint8_t subChunkY = y >> 4; // y / 16
        const uint8_t localY = y & 0x0F;  // y % 16

        // Lazily allocate subchunk on demand
        if (!m_subChunks[subChunkY]) {
            // Don't allocate a new subchunk just to place air
            if (state.blockId == 0) return; 
            m_subChunks[subChunkY] = std::make_unique<SubChunkData>(subChunkY);
        }

        m_subChunks[subChunkY]->setBlockState(x, localY, z, state);
        m_isDirty = true; // Mark chunk for mesh rebuild
    }

    /**
     * @brief Retrieves a raw pointer to a subchunk.
     *
     * @return Raw pointer to subchunk or @c nullptr if unallocated.
     */
    [[nodiscard]] const SubChunkData* getSubChunk(const uint8_t subChunkY) const {
        if (subChunkY >= m_subChunks.size()) return nullptr;
        return m_subChunks[subChunkY].get();
    }

    /** @brief Returns chunk X position in world space. */
    [[nodiscard]] int32_t getChunkX() const { return m_chunkX; }

    /** @brief Returns chunk Z position in world space. */
    [[nodiscard]] int32_t getChunkZ() const { return m_chunkZ; }

    /**
     * @brief Checks if chunk is dirty.
     *
     * @return @c true if this chunk's mesh need to be rebuilt, @c false if not.
     */
    [[nodiscard]] bool isDirty() const { return m_isDirty; }

    /** @brief Makes the chunk non-dirty, meaning there is no need to rebuild the mesh. */
    void clearDirty() { m_isDirty = false; }

private:
    int32_t m_chunkX{0}; ///< World chunk X coordinate.
    int32_t m_chunkZ{0}; ///< World chunk Z coordinate.
    bool m_isDirty{true}; ///< Flag indicating whether GPU mesh needs rebuilding.

    /// Array storing all vertical subchunks.
    std::array<std::unique_ptr<SubChunkData>, WorldConfig::CHUNK_HEIGHT> m_subChunks{};
};