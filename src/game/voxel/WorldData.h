#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "../entity/Entity.h"
#include "../entity/impl/Player.h"
#include "chunk/RegionData.h"

/** @brief Represents all data that belongs to the world. Only updates everything, does not render. */
class WorldData {
public:
    WorldData();
    ~WorldData();

    // Prevent accidental copying (WorldData owns unique resources)
    WorldData(const WorldData&) = delete;
    WorldData& operator=(const WorldData&) = delete;

    // Allow moving
    WorldData(WorldData&&) noexcept = default;
    WorldData& operator=(WorldData&&) noexcept = default;

    /**
     * @brief Updates all entities and processes fixed game ticks.
     *
     * @param deltaTime Time elapsed since the previous frame.
     */
    void update(float deltaTime);

    /**
     * @brief Retrieves a block state at global world block coordinates (x, y, z).
     *
     * @return BlockState at location or air if the region/chunk is ungenerated or out of bounds.
     */
    [[nodiscard]] BlockState getBlockState(const int32_t x, const int32_t y, const int32_t z) {
        if (y < 0 || y >= WorldConfig::WORLD_HEIGHT) return BlockState{};

        const int32_t chunkX = blockToChunk(x);
        const int32_t chunkZ = blockToChunk(z);

        const ChunkData* chunk = getChunkAt(chunkX, chunkZ);
        if (!chunk) return BlockState{};

        const auto localX = posToLocal(x);
        const auto localZ = posToLocal(z);

        return chunk->getBlockState(localX, static_cast<uint16_t>(y), localZ);
    }

    /**
     * @brief Sets a block state at global world block coordinates (x, y, z).
     *
     * Automatically lazily creates regions/chunks on placement if they don't exist.
     */
    void setBlockState(const int32_t x, const int32_t y, const int32_t z, const BlockState state) {
        if (y < 0 || y >= WorldConfig::WORLD_HEIGHT) return;

        const int32_t chunkX = blockToChunk(x);
        const int32_t chunkZ = blockToChunk(z);

        ChunkData* chunk = getOrCreateChunk(chunkX, chunkZ);
        if (!chunk) return;

        const auto localX = posToLocal(x);
        const auto localY = posToLocal(y);
        const auto localZ = posToLocal(z);

        chunk->setBlockState(localX, static_cast<uint16_t>(y), localZ, state);

        const auto subChunkY = y / WorldConfig::SUBCHUNK_SIZE;

        // Check X boundaries and mark neighbor chunks dirty
        if (localX == 0) {
            if (ChunkData* neighbor = getChunkAt(blockToChunk(x - 1), chunkZ)) {
                neighbor->markSubChunkDirty(subChunkY);
            }
        } else if (localX == WorldConfig::SUBCHUNK_SIZE - 1) {
            if (ChunkData* neighbor = getChunkAt(blockToChunk(x + 1), chunkZ)) {
                neighbor->markSubChunkDirty(subChunkY);
            }
        }

        // Check Z boundaries and mark neighbor chunks dirty
        if (localZ == 0) {
            if (ChunkData* neighbor = getChunkAt(chunkX, blockToChunk(z - 1))) {
                neighbor->markSubChunkDirty(subChunkY);
            }
        } else if (localZ == WorldConfig::SUBCHUNK_SIZE - 1) {
            if (ChunkData* neighbor = getChunkAt(chunkX, blockToChunk(z + 1))) {
                neighbor->markSubChunkDirty(subChunkY);
            }
        }

        // Check Y boundaries and mark subchunks dirty within the SAME chunk column
        if (localY == 0 && subChunkY > 0) {
            chunk->markSubChunkDirty(subChunkY - 1);
        } else if (localY == WorldConfig::SUBCHUNK_SIZE - 1 && subChunkY < WorldConfig::CHUNK_HEIGHT - 1) {
            chunk->markSubChunkDirty(subChunkY + 1);
        }
    }

    /** @brief Retrieves a loaded region by region world coordinates. Returns nullptr if not loaded. */
    [[nodiscard]] RegionData* getRegion(const int32_t regionX, const int32_t regionZ) {
        const uint64_t key = packKey(regionX, regionZ);
        const auto it = m_regions.find(key);
        return (it != m_regions.end()) ? it->second.get() : nullptr;
    }

    /** @brief Gets an existing region or allocates a new one if it doesn't exist. */
    RegionData* getOrCreateRegion(const int32_t regionX, const int32_t regionZ) {
        const uint64_t key = packKey(regionX, regionZ);
        auto& regionPtr = m_regions[key];
        if (!regionPtr) {
            regionPtr = std::make_unique<RegionData>(regionX, regionZ);
        }
        return regionPtr.get();
    }

    /** @brief Retrieves a loaded chunk by chunk world coordinates. Returns nullptr if not loaded. */
    [[nodiscard]] ChunkData* getChunkAt(const int32_t chunkX, const int32_t chunkZ) {
        const int32_t rx = chunkToRegion(chunkX);
        const int32_t rz = chunkToRegion(chunkZ);

        const RegionData* region = getRegion(rx, rz);
        if (!region) return nullptr;

        return region->getChunk(chunkX, chunkZ);
    }

    /** @brief Retrieves or allocates a chunk by global chunk coordinates. */
    ChunkData* getOrCreateChunk(const int32_t chunkX, const int32_t chunkZ) {
        const int32_t rx = chunkToRegion(chunkX);
        const int32_t rz = chunkToRegion(chunkZ);

        RegionData* region = getOrCreateRegion(rx, rz);

        const auto localChunkX = chunkToLocalRegion(chunkX);
        const auto localChunkZ = chunkToLocalRegion(chunkZ);

        return region->getOrCreateChunkLocal(localChunkX, localChunkZ);
    }

    /** @brief Maps world block position to world chunk position. */
    [[nodiscard]] static int32_t blockToChunk(const int32_t val) {
        return (val < 0) ? ((val - (WorldConfig::SUBCHUNK_SIZE - 1)) /
            WorldConfig::SUBCHUNK_SIZE) : (val / WorldConfig::SUBCHUNK_SIZE);
    }

    /** @brief Maps world chunk position to world region position. */
    [[nodiscard]] static int32_t chunkToRegion(const int32_t val) {
        return (val < 0) ? ((val - (WorldConfig::REGION_SIZE - 1)) /
            WorldConfig::REGION_SIZE) : (val / WorldConfig::REGION_SIZE);
    }

    /** @brief Maps world block position to local 0..15 block coordinate within a chunk. */
    [[nodiscard]] static uint8_t posToLocal(const int32_t val) {
        const int32_t mod = val % WorldConfig::SUBCHUNK_SIZE;
        return static_cast<uint8_t>(mod < 0 ? mod + WorldConfig::SUBCHUNK_SIZE : mod);
    }

    /** @brief Maps world chunk position to local 0..31 chunk coordinate within a region. */
    [[nodiscard]] static uint8_t chunkToLocalRegion(const int32_t val) {
        const int32_t mod = val % WorldConfig::REGION_SIZE;
        return static_cast<uint8_t>(mod < 0 ? mod + WorldConfig::REGION_SIZE : mod);
    }

    /** @brief Bit-packs 2D 32-bit coordinates (X, Z) into a single 64-bit map key. */
    [[nodiscard]] static uint64_t packKey(const int32_t x, const int32_t z) {
        return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(z);
    }

    /**
     * @brief Spawns a new entity in the world.
     *
     * @tparam T Entity type deriving from Entity.
     * @tparam Args Constructor arguments for T.
     *
     * @return Raw pointer to the newly created entity.
     */
    template<typename T, typename... Args>
    T* spawnEntity(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = entity.get();
        m_entities[entity->getId()] = std::move(entity);
        return ptr;
    }

    /**
     * @brief Removes an entity by its unique identifier.
     */
    void removeEntity(const uint64_t entityId) {
        if (m_localPlayer && m_localPlayer->getId() == entityId) {
            m_localPlayer = nullptr; // Prevent dangling pointer
        }
        m_entities.erase(entityId);
    }

    /**
    * @brief Returns a read-only reference to all entities in the world.
    */
    [[nodiscard]] const std::unordered_map<uint64_t, std::unique_ptr<Entity>>& getEntities() const {
        return m_entities;
    }

    /**
     * @brief Gets the Local Player reference without taking ownership.
     *
     * @return Non-owning pointer to Player, or nullptr if not spawned.
     */
    [[nodiscard]] Player* getLocalPlayer() const { return m_localPlayer; }

    /** @brief Returns world ticks since the start. */
    [[nodiscard]] int64_t getWorldTicks() const { return m_worldTicks; }

private:
    /** @brief Executes fixed game ticks (physics, logic, world simulation). */
    void tick();

    /// Map of packed 64-bit region coordinates (RegionX, RegionZ) -> RegionData instance.
    std::unordered_map<uint64_t, std::unique_ptr<RegionData>> m_regions;

    /// Map of unique Entity ID -> Owning Entity Pointer
    std::unordered_map<uint64_t, std::unique_ptr<Entity>> m_entities;

    Player* m_localPlayer = nullptr;    ///< Non-owning raw pointer targeting the local player in m_entities.

    int64_t m_worldTicks{0};            ///< Total game ticks executed.
    float m_tickAccumulator{0.0f};      ///< Accumulated time for fixed timestep updates.
};
