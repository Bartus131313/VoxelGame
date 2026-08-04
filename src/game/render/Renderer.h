#pragma once

#include <source_location>
#include <string_view>
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @brief Global OpenGL state manager and rendering pipeline coordinator.
 *
 * Centralizes OpenGL global state configuration.
 */
class Renderer {
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
     * @brief Sets the depth comparison function.
     *
     * @param func Depth function (e.g., GL_LESS, GL_LEQUAL).
     */
    static void setDepthFunc(GLenum func);

    /**
     * @brief Enables or disables face culling.
     *
     * @param enable True to cull back faces, false to render both sides.
     */
    static void setFaceCulling(bool enable);

    /**
     * @brief Enables or disables alpha blending.
     *
     * @param enable True to enable blending, false to disable.
     */
    static void setBlending(bool enable);

    /**
     * @brief Sets the alpha blending function factors.
     *
     * @param sourceFactor Source blending factor (e.g., GL_SRC_ALPHA).
     * @param destFactor Destination blending factor (e.g., GL_ONE_MINUS_SRC_ALPHA).
     */
    static void setBlendFunc(GLenum sourceFactor, GLenum destFactor);

    /**
     * @brief Checks and logs any pending OpenGL errors.
     * @param context Optional description of the operation being checked.
     * @param location Caller source location (auto-filled).
     */
    static void checkError(std::string_view context = "",
                           std::source_location location = std::source_location::current());
};