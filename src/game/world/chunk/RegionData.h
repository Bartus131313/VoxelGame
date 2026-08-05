#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "ChunkData.h"
#include "../WorldConfig.h"

/**
 * @brief Represents a region containing a grid of ChunkData instances.
 *
 * Owned by World. Manages ChunkData instances in a horizontal area.
 */
class RegionData {
public:
    RegionData(const int32_t regionX, const int32_t regionZ)
        : m_regionX(regionX), m_regionZ(regionZ) {}

    /**
     * @brief Retrieves a chunk using local region coordinates.
     *
     * @return Pointer to ChunkData or nullptr if not generated/loaded yet.
     */
    [[nodiscard]] ChunkData* getChunkLocal(const uint8_t localX, const uint8_t localZ) {
        if (localX >= WorldConfig::REGION_SIZE || localZ >= WorldConfig::REGION_SIZE)
            return nullptr;

        // TODO: If somehow in the future the subchunk size will change, this below NEEDS to be changed too.
        return m_chunks[(localZ << 4) | localX].get();
    }

    /**
     * @brief Retrieves a chunk using global world chunk coordinates.
     *
     * Automatically converts world chunk coordinates to local 0..15 region indices.
     */
    [[nodiscard]] ChunkData* getChunk(const int32_t chunkX, const int32_t chunkZ) {
        const uint8_t localX = getLocalChunkCoord(chunkX);
        const uint8_t localZ = getLocalChunkCoord(chunkZ);
        return getChunkLocal(localX, localZ);
    }

    /** @brief Retrieves an existing chunk or lazily creates a new one at local coordinates. */
    ChunkData* getOrCreateChunkLocal(const uint8_t localX, const uint8_t localZ) {
        if (localX >= WorldConfig::REGION_SIZE || localZ >= WorldConfig::REGION_SIZE)
            return nullptr;

        // TODO: If somehow in the future the subchunk size will change, this below NEEDS to be changed too.
        const size_t index = (localZ << 4) | localX;
        if (!m_chunks[index]) {
            const int32_t worldChunkX = (m_regionX * WorldConfig::REGION_SIZE) + localX;
            const int32_t worldChunkZ = (m_regionZ * WorldConfig::REGION_SIZE) + localZ;
            m_chunks[index] = std::make_unique<ChunkData>(worldChunkX, worldChunkZ);
        }

        return m_chunks[index].get();
    }

    /** @brief Returns region X coordinate in world region units. */
    [[nodiscard]] int32_t getRegionX() const { return m_regionX; }

    /** @brief Returns region Z coordinate in world region units. */
    [[nodiscard]] int32_t getRegionZ() const { return m_regionZ; }

    /** @brief Helper to convert any world chunk coordinate to a 0..15 local region coordinate. */
    [[nodiscard]] static uint8_t getLocalChunkCoord(const int32_t worldChunkCoord) {
        const int32_t mod = worldChunkCoord % WorldConfig::REGION_SIZE;
        return static_cast<uint8_t>(mod < 0 ? mod + WorldConfig::REGION_SIZE : mod);
    }

private:
    int32_t m_regionX{0}; ///< World region X coordinate.
    int32_t m_regionZ{0}; ///< World region Z coordinate.

    /// Grid of ChunkData instances.
    std::array<std::unique_ptr<ChunkData>, WorldConfig::REGION_SIZE> m_chunks{};
};