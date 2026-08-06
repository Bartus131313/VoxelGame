#pragma once

/** @brief Contains all important world configurations. */
class WorldConfig {
public:
    static constexpr int  SUBCHUNK_SIZE   = 16;   ///< How many blocks does subchunk have in any direction?
    static constexpr int  SUBCHUNK_AREA   = SUBCHUNK_SIZE * SUBCHUNK_SIZE;
    static constexpr int  SUBCHUNK_VOLUME = SUBCHUNK_AREA * SUBCHUNK_SIZE;
    static constexpr int  CHUNK_HEIGHT    = 16;    ///< How many subchunks are in one chunk?

    static constexpr int  REGION_SIZE     = 32;    ///< How many chunks does region have in any direction?
    static constexpr int  REGION_AREA     = REGION_SIZE * REGION_SIZE;

    static constexpr int  WORLD_HEIGHT    = SUBCHUNK_SIZE * CHUNK_HEIGHT;
};