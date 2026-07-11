//
// Created by ZQD on 2026/7/11.
//

#include "MiniCamera.h"

#include <algorithm>
#include <cmath>
#include <QtMath>

MiniCamera::MiniCamera(int width, int height) : m_Width(width), m_Height(height) {
    m_OrbitCamera = std::make_unique<OrbitCamera>();
    m_OrbitCamera->pivot = QVector3D(0.0f, 0.0f, 0.0f);
    m_OrbitCamera->distance = 3.0f;
    m_OrbitCamera->yaw = 35.0f;
    m_OrbitCamera->pitch = 25.0f;
    m_OrbitCamera->fovY = 45.0f;
    m_OrbitCamera->aspect = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    m_OrbitCamera->nearPlane = 0.1f;
    m_OrbitCamera->farPlane = 100.0f;
    m_OrbitCamera->orthographic = true;
    m_OrbitCamera->orthoHeight = 4.0f;
}

void MiniCamera::Resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    m_OrbitCamera->aspect = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
}

void MiniCamera::Orbit(float dx, float dy) {
    constexpr float OrbitSensitivity = 0.35f;
    m_OrbitCamera->yaw += -dx * OrbitSensitivity;
    m_OrbitCamera->pitch += dy * OrbitSensitivity;
    m_OrbitCamera->pitch = std::clamp(m_OrbitCamera->pitch, -89.0f, 89.0f);
}

void MiniCamera::Pan(float dx, float dy) {
    const QVector3D cameraPosition = CameraPosition();
    const QVector3D forward = (m_OrbitCamera->pivot - cameraPosition).normalized();
    const QVector3D worldUp(0.0f, 1.0f, 0.0f);
    const QVector3D right = QVector3D::crossProduct(forward, worldUp).normalized();
    const QVector3D up = QVector3D::crossProduct(right, forward).normalized();

    const float viewportHeight = std::max(1, m_Height);
    const float visibleHeight = m_OrbitCamera->orthographic
                                    ? m_OrbitCamera->orthoHeight
                                    : 2.0f * m_OrbitCamera->distance *
                                      std::tan(qDegreesToRadians(m_OrbitCamera->fovY * 0.5f));
    const float worldUnitsPerPixel = visibleHeight / viewportHeight;

    m_OrbitCamera->pivot -= right * (dx * worldUnitsPerPixel);
    m_OrbitCamera->pivot += up * (dy * worldUnitsPerPixel);
}

void MiniCamera::Zoom(float delta) {
    if (m_OrbitCamera->orthographic) {
        m_OrbitCamera->orthoHeight *= std::pow(0.85f, delta);
        m_OrbitCamera->orthoHeight = std::clamp(m_OrbitCamera->orthoHeight, 0.05f, 1000.0f);
        return;
    }

    m_OrbitCamera->distance *= std::pow(0.85f, delta);
    m_OrbitCamera->distance = std::clamp(m_OrbitCamera->distance, 0.05f, 1000.0f);
}

QMatrix4x4 MiniCamera::ViewMatrix() const {
    QMatrix4x4 view;
    view.lookAt(CameraPosition(), m_OrbitCamera->pivot, QVector3D(0.0f, 1.0f, 0.0f));
    return view;
}

QMatrix4x4 MiniCamera::ProjectionMatrix() const {
    QMatrix4x4 projection;
    if (m_OrbitCamera->orthographic) {
        const float halfHeight = m_OrbitCamera->orthoHeight * 0.5f;
        const float halfWidth = halfHeight * m_OrbitCamera->aspect;
        projection.ortho(
            -halfWidth,
            halfWidth,
            -halfHeight,
            halfHeight,
            m_OrbitCamera->nearPlane,
            m_OrbitCamera->farPlane);
        return projection;
    }

    projection.perspective(
        m_OrbitCamera->fovY,
        m_OrbitCamera->aspect,
        m_OrbitCamera->nearPlane,
        m_OrbitCamera->farPlane);
    return projection;
}

QVector3D MiniCamera::CameraPosition() const {
    const float yawRadians = qDegreesToRadians(m_OrbitCamera->yaw);
    const float pitchRadians = qDegreesToRadians(m_OrbitCamera->pitch);

    QVector3D direction;
    direction.setX(std::cos(pitchRadians) * std::sin(yawRadians));
    direction.setY(std::sin(pitchRadians));
    direction.setZ(std::cos(pitchRadians) * std::cos(yawRadians));
    direction.normalize();

    return m_OrbitCamera->pivot + direction * m_OrbitCamera->distance;
}
