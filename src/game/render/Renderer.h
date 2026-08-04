#pragma once

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
};