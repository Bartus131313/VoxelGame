#pragma once

#include <string>
#include <string_view>
#include <functional>
/**
 * @brief Represents a resource location (domain:path).
 */
class ResourceLocation {
public:
    static constexpr std::string_view DEFAULT_DOMAIN = "voxelgame";

    /**
     * @brief Parses a resource location from a single string.
     *
     * If no ':' is found, the domain defaults to @c DEFAULT_DOMAIN string.
     *
     * @param resourceName The formatted string (e.g., "DEFAULT_DOMAIN:stone" or just "stone").
     */
    ResourceLocation(std::string_view resourceName) {
        if (const size_t colonPos = resourceName.find(':'); colonPos != std::string_view::npos) {
            m_resourceDomain = resourceName.substr(0, colonPos);
            m_resourcePath = resourceName.substr(colonPos + 1);
        } else {
            m_resourceDomain = DEFAULT_DOMAIN;
            m_resourcePath = resourceName;
        }
        m_fullString = m_resourceDomain + ":" + m_resourcePath;
    }

    /**
     * @brief Constructs a resource location explicitly from a domain and path.
     */
    ResourceLocation(const std::string_view resourceDomain, const std::string_view resourcePath)
        : m_resourceDomain(resourceDomain), m_resourcePath(resourcePath) {
        m_fullString = m_resourceDomain + ":" + m_resourcePath;
    }

    /** @brief Returns full string that connects domain with path. */
    [[nodiscard]] const std::string& toString() const { return m_fullString; }

    /** @brief Returns resource domain. */
    [[nodiscard]] const std::string& getResourceDomain() const { return m_resourceDomain; }

    /** @brief Returns resource path. */
    [[nodiscard]] const std::string& getResourcePath() const { return m_resourcePath; }

    /** @brief Returns full path to the asset resource. */
    [[nodiscard]] std::string resolveAssetPath() const {
        return "assets/" + m_resourceDomain + "/" + m_resourcePath;
    }

    /** @brief Returns full path to the data resource. */
    [[nodiscard]] std::string resolveDataPath() const {
        return "data/" + m_resourceDomain + "/" + m_resourcePath;
    }

    /**
     * @brief Calculates Hash Code of the Resource Location using hash code of domain and path.
     *
     * @return Hash Code of the Resource Location
     */
    [[nodiscard]] std::size_t getHashCode() const {
        return 31 * std::hash<std::string>{}(m_resourceDomain) + std::hash<std::string>{}(m_resourcePath);
    }

    bool operator==(const ResourceLocation& other) const {
        return m_fullString == other.m_fullString;
    }

    bool operator!=(const ResourceLocation& other) const {
        return !(*this == other);
    }

private:
    std::string m_resourceDomain;       ///< Namespace of the resource.
    std::string m_resourcePath;         ///< Path of the resource.
    std::string m_fullString;           ///< Full string that has namespace connected with path.
};

/** @brief This allows Identifier to be used in @c std::unordered_map as a key. */
template <>
struct std::hash<ResourceLocation> {
    std::size_t operator()(const ResourceLocation& id) const noexcept {
        // We can just hash the cached full string for maximum speed
        return std::hash<std::string>{}(id.toString());
    }
};