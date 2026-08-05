#include "Canvas.h"

#include <iostream>
#include <algorithm>

#include "../render/RenderSystem.h"
#include "glm/ext/matrix_transform.hpp"

Canvas::Canvas(const int screenWidth, const int screenHeight) {
    setSize(screenWidth, screenHeight);
}

void Canvas::setSize(const int width, const int height) {
    m_width = static_cast<float>(width);
    m_height = static_cast<float>(height);
    m_projection = glm::ortho(0.0f, m_width, m_height, 0.0f, -1.0f, 1.0f);
}

void Canvas::render() const {
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

    // Enter 2D rendering
    RenderSystem::enter2D();

    // Draw all visible UI elements in a single pass based on z-index
    for (const auto& element : sortedElements) {
        element->render(m_projection);
    }
}