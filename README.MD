# VoxelGame

A modular, high-performance rendering engine core built from scratch in modern **C++20** using **OpenGL**.

This project features a complete component-based UI canvas with Z-index depth sorting, automated GPU resource
management via RAII, runtime font atlas generation using `stb_truetype`, and an efficient shader caching pipeline.

---

## Key Features
>* **Modern 2D UI System (`Canvas` & `UIElement`)**
>    * **Component-Based Architecture:** Easily extendable `UIElement` base class for elements like `UILabel`.
>    * **Z-Index Layering:** Dynamic, stable sorting (`std::ranges::stable_sort`) ensuring predictable UI draw orders.
>    * **Responsive Orthographic Projection:** Screen-space canvas scaling with automatic viewport recalculations.

>* **Dynamic Font & Text Rendering (`FontManager` & `FontRenderer`)**
>    * **On-the-Fly Packing:** Real-time font glyph atlas packing using `stb_truetype`.
>    * **Smart Asset Caching:** Deduplicated font caching keyed by font family and pixel height.
>    * **Ascent & Metrics Processing:** Top-left anchoring support using precise character metrics.

> * **Shader Pipeline (`Shader` & `ShaderManager`)**
>     * **Pipeline Flexibility:** Support for Vertex, Fragment, and optional Geometry shader stages.
>     * **Smart Asset Caching:** Centralized cache preventing duplicate compilation of GLSL source files.
>     * **Hot Asset Tracking:** Custom CMake integration monitoring `.vsh`, `.fsh`, and `.gsh` modifications for continuous builds.

>* **Modern C++20 & RAII Memory Management**
>    * **Strict Resource Lifecycles:** Zero manual GPU memory leaks; textures, VAOs, VBOs, and shader programs clean up automatically upon destruction.
>    * **Clear Ownership Boundaries:** Shared ownership for engine assets (`std::shared_ptr`) combined with zero-overhead borrowing (`const raw pointers`) for render loops.

---

## Tech Stack & Dependencies

| Library             | Role                                                |
|:--------------------|:----------------------------------------------------|
| **C++20**           | Core programming language standard                  |
| **CMake (>= 3.20)** | Build system & asset file dependency tracking       |
| **OpenGL**          | Core graphics API                                   |
| **GLFW**            | Window creation, OpenGL context, and platform input |
| **GLAD**            | Modern OpenGL function loader                       |
| **GLM**             | OpenGL Mathematics (matrices, vectors, projections) |
| **stb_truetype**    | TrueType font loading and bitmap packing            |

---

## Building the Project

### Prerequisites
> * C++20 Compliant Compiler (GCC 10+, Clang 11+, or MSVC 2019+)
> * CMake 3.20 or higher
> * Git

### Clone & Build
```shell
# 1. Clone the repository
git clone https://github.com/Bartus131313/VoxelGame.git
cd VoxelGame

# 2. Generate build system files
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 3. Build executable
cmake --build build --parallel
```

## License
This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.