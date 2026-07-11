//
// Created by ZQD on 2026/7/11.
//

#pragma once
#include <memory>
#include <QMatrix4x4>
#include <QVector3D>

struct OrbitCamera {
    QVector3D pivot; // 观察中心，也就是 orbit pivot
    float distance; // 相机到 pivot 的距离
    float yaw;
    float pitch;
    float fovY;
    float aspect;
    float nearPlane;
    float farPlane;
    bool orthographic;
    float orthoHeight;
};


class MiniCamera {
public:
    MiniCamera(int width, int height);

    ~MiniCamera() = default;

public:
    void Resize(int width, int height);

    void Orbit(float dx, float dy);

    void Pan(float dx, float dy);

    void Zoom(float delta);

    QMatrix4x4 ViewMatrix() const;

    QMatrix4x4 ProjectionMatrix() const;

private:
    [[nodiscard]] QVector3D CameraPosition() const;

    std::unique_ptr<OrbitCamera> m_OrbitCamera;
    int m_Width = 1;
    int m_Height = 1;
};
