#pragma once

#include <format>
#include <source_location>
#include <string_view>
#include <glad/glad.h>
#include <glm/glm.hpp>

/** @brief Represents polygon mode used by OpenGL. */
enum RenderMode : uint8_t {
    Fill = 0, Line = 1, Point = 2
};

template <>
struct std::formatter<RenderMode> : std::formatter<std::string_view> {
    auto format(const RenderMode mode, std::format_context& ctx) const {
        std::string_view name = "UNKNOWN";
        switch (mode) {
            case Fill:  name = "FILL"; break;
            case Line:  name = "LINE"; break;
            case Point: name = "POINT"; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

/** @brief Face culling targets. */
enum class CullFace : uint8_t {
    Front, Back, FrontAndBack
};

/** @brief Triangle vertex winding orders. */
enum class WindingOrder : uint8_t {
    Clockwise, CounterClockwise
};

/** @brief Depth buffer comparison algorithms. */
enum class DepthFunc : uint8_t {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

/** @brief Source and destination alpha blending calculation factors. */
enum class BlendFactor : uint8_t {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha
};

/** @brief Target texture bind targets. */
enum class TextureType : uint8_t {
    Texture2D,
    Texture2DArray,
    TextureCubeMap,
    Texture3D
};

/**
 * @brief Global OpenGL state manager and rendering pipeline coordinator.
 *
 * Centralizes OpenGL global state configuration.
 */
class RenderSystem {
public:
    /**
     * @brief Configures initial global OpenGL pipeline states.
     *
     * @note Must be called after an active OpenGL context is created by the Window class.
     */
    static void init();

    /**
     * @brief Clears color and depth buffers for the current frame using vec3.
     *
     * @param color Background clear color (RGB).
     */
    static void clear(const glm::vec3& color = glm::vec3(0.1f, 0.1f, 0.1f));

    /**
     * @brief Clears color and depth buffers for the current frame using 3 floats.
     *
     * @param r Red channel.
     * @param g Green channel.
     * @param b Blue channel.
     */
    static void clear(float r, float g, float b);

    /**
     * @brief Updates the OpenGL viewport dimensions on window resize events.
     *
     * @param width New framebuffer width in pixels.
     * @param height New framebuffer height in pixels.
     */
    static void setViewport(int width, int height);

    /**
     * @brief Enables of disables depth test.
     *
     * @param enable @c true to enable depth test, @c false to disable.
     */
    static void setDepthTest(bool enable);

    /**
     * @brief Sets the depth buffer comparison operation.
     *
     * @param func Comparison algorithm used to pass or fail incoming depth pixels.
     */
    static void setDepthFunc(DepthFunc func);

    /**
     * @brief Enables or disables face culling.
     *
     * @param enable @c true to cull back faces, @c false to render both sides.
     */
    static void setFaceCulling(bool enable);

    /**
    * @brief Sets which polygon face is culled (Front, Back, or FrontAndBack).
    *
    * @param face Target polygon face (Front, Back, or FrontAndBack) to discard during rasterization.
    */
    static void setCullFace(CullFace face);

    /**
    * @brief Sets the winding order that determines front-facing polygons (CCW is OpenGL default).
    *
    * @param order Vertex winding direction (Clockwise or CounterClockwise) used to identify front-facing polygons.
    */
    static void setWindingOrder(WindingOrder order);

    /**
     * @brief Enables or disables alpha blending.
     *
     * @param enable @c true to enable blending, @c false to disable.
     */
    static void setBlending(bool enable);

    /**
     * @brief Sets the pixel blending arithmetic factors for source and destination colors.
     *
     * @param sourceFactor Specifies how the incoming RGBA color factor is calculated.
     * @param destFactor Specifies how the existing frame buffer color factor is calculated.
     */
    static void setBlendFunc(BlendFactor sourceFactor, BlendFactor destFactor);

    /**
     * @brief Sets the active GL texture unit slot.
     *
     * @param slot Texture unit index (0 for slot 0 / GL_TEXTURE0, 1 for slot 1, etc.).
     */
    static void setActiveTextureSlot(uint32_t slot);

    /**
     * @brief Binds a texture object to the currently active texture unit slot.
     *
     * @param type Target texture type.
     * @param textureId Native graphics driver texture handle ID.
     */
    static void bindTexture(TextureType type, GLuint textureId);

    /**
     * @brief Convenience function that sets the active slot and binds a texture in one call.
     *
     * @param slot Texture unit index.
     * @param type Target texture type.
     * @param textureId Native graphics driver texture handle ID.
     */
    static void bindTexture(uint32_t slot, TextureType type, GLuint textureId);

    /**
     * @brief Convenience overload for 2D textures on a specific slot.
     *
     * @param slot Texture unit index.
     * @param textureId Native graphics driver texture handle ID.
     */
    static void bindTexture(uint32_t slot, GLuint textureId);

    /**
     * @brief Checks and logs any pending OpenGL errors.
     * @param context Optional description of the operation being checked.
     * @param location Caller source location (auto-filled).
     */
    static void checkError(std::string_view context = "",
                           std::source_location location = std::source_location::current());

    /**
     * @brief Enters flat 2D/UI rendering mode (disables depth test + face culling).
     */
    static void enter2D();

    /**
     * @brief Restores global GL state for 3D world rendering (depth testing + back-face culling).
     *
     * Call this after a 2D/UI pass to bring GL state back to what the 3D world pass expects
     * on the next frame.
     */
    static void enter3D();

    /**
     * @brief Sets render mode to the new one. Useful for debug wireframe rendering.
     *
     * @param mode New render mode that will be applied.
     */
    static void setRenderMode(RenderMode mode);

    /**
     * @brief Returns current render mode used by OpenGL.
     *
     * @return Current render mode.
     */
    static RenderMode getRenderMode() { return s_renderMode; }

private:
    static RenderMode s_renderMode;
};