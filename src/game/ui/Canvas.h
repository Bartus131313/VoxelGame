#pragma once

#include <vector>
#include <memory>

#include "UIElement.h"
#include "../render/shader/Shader.h"
#include "glm/ext/matrix_clip_space.hpp"

/**
 * @brief Manages a collection of UI elements, handles Z-index sorting, and renders them
 * using a responsive, scaled orthographic projection.
 */
class Canvas {
public:
    /** @brief Constructs a Canvas instance with initial screen dimensions.
     *
     * @param screenWidth Initial width of the rendering viewport in pixels.
     * @param screenHeight Initial height of the rendering viewport in pixels.
     */
    Canvas(int screenWidth, int screenHeight);

    /**
     * @brief Instantiates and adds a new UI element of type T to the canvas collection.
     *
     * @tparam T The UI element class type (must derive from UIElement).
     * @tparam Args Forwarded argument types for the element's constructor.
     * @param args Arguments forwarded directly to the constructor of type T.
     *
     * @return A shared pointer to the newly created and added UI element.
     */
    template<typename T, typename... Args>
    std::shared_ptr<T> addElement(Args&&... args) {
        auto element = std::make_shared<T>(std::forward<Args>(args)...);
        m_elements.push_back(element);
        return element;
    }

    /**
     * @brief Updates the canvas viewport dimensions and recalculates its base metrics.
     *
     * @param width New viewport width in pixels.
     * @param height New viewport height in pixels.
     */
    void setSize(int width, int height);

    /**
     * @brief Sorts all active elements by Z-index, computes the discrete UI scale factor,
     * and renders them in a single render pass.
     */
    void render() const;

private:
    float m_width{};            ///< Current canvas viewport width in pixels.
    float m_height{};           ///< Current canvas viewport height in pixels.
    glm::mat4 m_projection{};   ///< Standard unscaled orthographic projection matrix.

    /// Collection tracking all active UI elements owned by this canvas.
    std::vector<std::shared_ptr<UIElement>> m_elements;
};