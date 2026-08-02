#include "Canvas.h"

#include <iostream>
#include <algorithm>
#include "glm/ext/matrix_transform.hpp"

Canvas::Canvas(const int screenWidth, const int screenHeight) {
    setSize(screenWidth, screenHeight);

    // TODO: Make ShaderManager that will cache all shaders.
    // Load UI/Text shader once
    if (!m_shader.loadFromFile("assets/shaders/font.vsh", "assets/shaders/font.fsh")) {
        std::cerr << "[Canvas] Failed to load font shader." << std::endl;
    }
}

void Canvas::setSize(const int width, const int height) {
    m_width = static_cast<float>(width);
    m_height = static_cast<float>(height);
    m_projection = glm::ortho(0.0f, m_width, m_height, 0.0f, -1.0f, 1.0f);
}

void Canvas::render() const {
    if (!m_shader.isValid()) return;

    // Create a temporary vector of raw pointers for sorting
    std::vector<UIElement*> sortedElements;
    sortedElements.reserve(m_elements.size());
    for (const auto& el : m_elements) {
        if (el->isVisible()) {
            sortedElements.push_back(el.get());
        }
    }

    // Sort the raw pointers by Z-Index (safe, fast, no copying of unique_ptr)
    std::ranges::stable_sort(sortedElements, [](const UIElement* a, const UIElement* b) {
        return a->getZIndex() < b->getZIndex();
    });
    // Use font shader and set projection uniform
    m_shader.use();
    m_shader.setMat4("projection", m_projection);

    // Draw all visible UI elements in a single pass based on z-index
    for (const auto& element : sortedElements) {
        if (element->isVisible()) element->render(m_shader);
    }
}