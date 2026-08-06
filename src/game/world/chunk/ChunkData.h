#pragma once

#include <array>
#include <memory>

#include "SubChunkData.h"
#include "../WorldConfig.h"

/** @brief Stores X, Z coords for chunk. */
struct ChunkPos {
    int32_t x{0};      ///< World chunk X coordinate.
    int32_t z{0};      ///< World chunk Z coordinate.

    bool operator==(const ChunkPos& other) const noexcept {
        return x == other.x && z == other.z;
    }
};

/** @brief This allows ChunkPos to be used in @c std::unordered_map as a key. */
template <>
struct std::hash<ChunkPos> {
    std::size_t operator()(const ChunkPos& pos) const noexcept {
        const auto x = static_cast<uint64_t>(static_cast<uint32_t>(pos.x));
        const auto z = static_cast<uint64_t>(static_cast<uint32_t>(pos.z));

        return std::hash<uint64_t>{}((x << 32) | z);
    }
};

/**
 * @brief Represents a vertical column of subchunks.
 *
 * Owned by Region. Manages SubChunkData instances lazily.
 */
class ChunkData {
public:
    ChunkData(const int32_t chunkX, const int32_t chunkZ)
        : m_chunkPos({chunkX, chunkZ}) {}

    ChunkData(const ChunkPos& chunkPos)
        : m_chunkPos(chunkPos) {}

    /**
     * @brief Retrieves the block state at global vertical coordinates within this column.
     *
     * @param x X position of block in local space.
     * @param y Y position of block in local space.
     * @param z Z position of block in local space.
     *
     * @return BlockState at given 3D local position or default BlockState (air) if out of bounds.
     */
    [[nodiscard]] BlockState getBlockState(const uint8_t x, const uint16_t y, const uint8_t z) const {
        if (y >= WorldConfig::WORLD_HEIGHT || x >= WorldConfig::SUBCHUNK_SIZE || z >= WorldConfig::SUBCHUNK_SIZE)
            return BlockState{};

        const uint8_t subChunkY = y / WorldConfig::SUBCHUNK_SIZE;
        const uint8_t localY = y % WorldConfig::SUBCHUNK_SIZE;

        const auto& subChunk = m_subChunks[subChunkY];
        if (!subChunk) return BlockState{}; // Empty/Unallocated sky returns default Air state

        return subChunk->getBlockState(x, localY, z);
    }

    /**
     * @brief Sets a block state, allocating the subchunk if it doesn't exist yet.
     *
     * @param x X position of block in local space.
     * @param y Y position of block in local space.
     * @param z Z position of block in local space.
     * @param state New block state that will be set.
     */
    void setBlockState(const uint8_t x, const uint16_t y, const uint8_t z, const BlockState state) {
        if (y >= WorldConfig::WORLD_HEIGHT || x >= WorldConfig::SUBCHUNK_SIZE || z >= WorldConfig::SUBCHUNK_SIZE)
            return;

        const uint8_t subChunkY = y / WorldConfig::SUBCHUNK_SIZE;
        const uint8_t localY = y % WorldConfig::SUBCHUNK_SIZE;

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
     * @param subChunkY Vertical position of subchunk inside chunk.
     *
     * @return Raw pointer to subchunk or @c nullptr if unallocated.
     */
    [[nodiscard]] const SubChunkData* getSubChunk(const uint8_t subChunkY) const {
        if (subChunkY >= m_subChunks.size()) return nullptr;
        return m_subChunks[subChunkY].get();
    }

    /** @brief Returns chunk position in world space. */
    [[nodiscard]] ChunkPos getChunkPos() const { return m_chunkPos; }

    /** @brief Returns chunk X position in world space. */
    [[nodiscard]] int32_t getChunkPosX() const { return m_chunkPos.x; }

    /** @brief Returns chunk Z position in world space. */
    [[nodiscard]] int32_t getChunkPosZ() const { return m_chunkPos.z; }

    /**
     * @brief Checks if chunk is dirty.
     *
     * @return @c true if this chunk's mesh need to be rebuilt, @c false if not.
     */
    [[nodiscard]] bool isDirty() const { return m_isDirty; }

    /** @brief Makes the chunk non-dirty, meaning there is no need to rebuild the mesh. */
    void clearDirty() { m_isDirty = false; }

private:
    ChunkPos m_chunkPos;    ///< World chunk position.
    bool m_isDirty{true};   ///< Flag indicating whether GPU mesh needs rebuilding.

    /// Array storing all vertical subchunks.
    std::array<std::unique_ptr<SubChunkData>, WorldConfig::CHUNK_HEIGHT> m_subChunks{};
};