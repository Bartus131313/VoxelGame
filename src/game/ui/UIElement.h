#pragma once

#include <glm/glm.hpp>
#include "../render/Shader.h"

/// Represents 2D element that will be rendered on UI.
class UIElement {
public:
    virtual ~UIElement() = default;

    /// Updates the UI Element, called on update pass.
    /// @param deltaTime Time elapsed since the previous frame.
    virtual void update(float deltaTime) {}

    /// Renders the whole UI Element, called on render pass.
    /// @param shader Shader that will be used to render it.
    virtual void render(const Shader& shader) = 0;

    /// Sets position of UI Element to given one.
    /// @param x X-coord of the element's position.
    /// @param y Y-coord of the element's position.
    void setPosition(const int x, const int y) { m_position = glm::ivec2(x, y); }

    /// Sets scale of UI Element to new one.
    /// @param scale New scale of the element.
    void setScale(const float scale) { m_scale = scale; }

    /// Sets rotation of UI Element to new one.
    /// @param rotation New rotation of the element.
    void setRotation(const float rotation) { m_rotation = rotation; }

    /// Set element's visibility to new one.
    /// @param visible Should the element be visible or not.
    void setVisible(const bool visible) { m_isVisible = visible; }

    /// Set element's z-index to new one.
    /// @param zIndex New z-index of the element. (Lower = rendered first, higher = rendered last)
    void setZIndex(const int zIndex) { m_zIndex = zIndex; }

    /// Gets element-s z-index.
    /// @return Z-index of this element.
    [[nodiscard]] int getZIndex() const { return m_zIndex; }

    /// Checks if UI Element is visible and should be rendered.
    /// @return @c true if element is visible, @c false if not.
    [[nodiscard]] bool isVisible() const { return m_isVisible; }

protected:
    glm::ivec2 m_position{};    ///< Position of the element.
    float m_scale = 1.0f;       ///< Scale of the element.
    float m_rotation = 0.0f;    ///< Rotation of the element in radians.
    bool m_isVisible = true;    ///< Should the element be rendered?
    int m_zIndex = 0;           ///< Z-index of the element. (Lower = rendered first, higher = rendered last)
};