#pragma once

#include <glm/glm.hpp>

/**
 * @brief Manages camera positioning, rotation, and transformation matrices for 3D rendering.
 *
 * Computes View (@c glm::lookAt) and Perspective Projection (@c glm::perspective) matrices.
 * Uses Euler angles (<b>Yaw</b> and <b>Pitch</b>) to calculate direction vectors,
 * with built-in pitch clamping to prevent gimbal lock.
 */
class Camera3D {
public:
    static constexpr float NEAR_PLANE = 0.01f;
    static constexpr float FAR_PLANE  = 1000.0f;
    static constexpr auto  WORLD_UP   = glm::vec3{0.0f, 1.0f, 0.0f};

    /**
     * @brief Constructs a new Camera3D instance with position and projection parameters.
     *
     * @param position Initial position vector in 3D world space (default: [0, 0, 0]).
     * @param yaw Initial horizontal rotation angle in degrees (default: -90.0f, facing -Z).
     * @param pitch Initial vertical rotation angle in degrees (default: 0.0f, horizon level).
     * @param fov Vertical field of view in degrees (default: 60.0f).
     * @param aspectRatio Initial viewport width divided by height (default: 16.0 / 9.0).
     */
    Camera3D(glm::vec3 position = glm::vec3(0.0f), float yaw = -90.0f, float pitch = 0.0f,
             float fov = 60.0f, float aspectRatio = 16.0f / 9.0f);

    /**
     * @brief Safely updates the aspect ratio when the viewport or window resizes.
     *
     * Ignores updates if height is <= 0 to prevent division-by-zero errors when minimized.
     *
     * @param width The new window or viewport width in pixels.
     * @param height The new window or viewport height in pixels.
     */
    void updateAspect(int width, int height);

    /**
     * @brief Recalculates the internal <b>Front</b>, <b>Right</b>, and <b>Up</b> directional vectors.
     *
     * Must be called whenever @c yaw or @c pitch values are modified.
     */
    void updateVectors();

    /**
     * @brief Computes and returns the 3D View matrix.
     *
     * @return @c glm::mat4 View matrix transformation (@c glm::lookAt).
     */
    [[nodiscard]] glm::mat4 getViewMatrix() const;

    /**
     * @brief Computes and returns the 3D Perspective Projection matrix.
     *
     * @return @c glm::mat4 Perspective projection matrix (@c glm::perspective).
     */
    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

    /** @brief Sets the world space position of the camera. */
    void setPosition(const glm::vec3& position) { m_position = position; }

    /**
     * @brief Returns the current world space position of the camera.
     *
     * @return Position of the camera in world space.
     */
    [[nodiscard]] const glm::vec3& getPosition() const { return m_position; }

    /**
     * @brief Returns the normalized forward direction vector of the camera.
     *
     * @return Normalized front vector of the camera.
     */
    [[nodiscard]] const glm::vec3& getFront() const { return m_front; }

    /**
     * @brief Returns the normalized right direction vector of the camera.
     *
     * @return Normalized right vector of the camera.
     */
    [[nodiscard]] const glm::vec3& getRight() const { return m_right; }

     /**
      * @brief Returns the normalized up direction vector of the camera.
      *
      * @return Normalized up vector of the camera.
      */
    [[nodiscard]] const glm::vec3& getUp() const { return m_up; }

    /** @brief Sets the vertical field of view in degrees. */
    void setFov(const float fov) { m_fov = fov; }

    /**
     * @brief Returns the current field of view in degrees.
     *
     * @return Camera's field of view (FOV).
     */
    [[nodiscard]] float getFov() const { return m_fov; }

    float yaw{-90.0f};    ///< Rotation of the camera around vertical axis (left, right).
    float pitch{0.0f};    ///< Rotation of the camera around lateral axis (up, down).

private:
    glm::vec3 m_position{0.0f};             ///< Position of the camera in 3D world.
    glm::vec3 m_front{0.0f, 0.0f, -1.0f};   ///< Front vector of the camera.
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};       ///< Up vector of the camera.
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};    ///< Right vector of the camera.

    float m_fov{60.0f};                 ///< Field of view of the camera.
    float m_aspectRatio{16.0f / 9.0f};  ///< Aspect ratio of the camera's viewport.
};