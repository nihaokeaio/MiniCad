//
// Created by ZQD on 2026/6/28.
//

#include "GizmoManager.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <Precision.hxx>
#include <QDebug>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Vec.hxx>

#include "Controller/Interaction/InteractionManager.h"
#include "Controller/Interaction/Picking/ScenePicker.h"
#include "ElementMesh/ElementMesh.h"
#include "GeomCalculator.h"
#include "Presentation/ViewState/TransformGuideAdaptor.h"
#include "Presentation/ViewState/ViewStateAdaptor.h"
#include "Scene/Scene.h"

using namespace TransformElementSpace;

namespace {
    constexpr double GizmoAxisLength = 180.0;
    constexpr double GizmoRotateRadius = 120.0;
    constexpr double GizmoPlaneOffset = 52.0;
    constexpr double GizmoPlaneSize = 32.0;
    constexpr double GizmoPickTolerance = 8.0;
    constexpr double GizmoCenterPickTolerance = 12.0;
    constexpr uint32_t GizmoRingSegments = 96;

    gp_Dir AxisDirection(TransformConstraint constraint) {
        switch (constraint) {
            case TransformConstraint::XAxis:
                return gp::DX();
            case TransformConstraint::YAxis:
                return gp::DY();
            case TransformConstraint::ZAxis:
                return gp::DZ();
            case TransformConstraint::XYPlane:
                return gp::DZ();
            case TransformConstraint::YZPlane:
                return gp::DX();
            case TransformConstraint::ZXPlane:
                return gp::DY();
            case TransformConstraint::Free:
            default:
                return gp::DZ();
        }
    }

    TransformMode ToTransformMode(GizmoMode mode) {
        switch (mode) {
            case GizmoMode::Rotate:
                return TransformMode::Rotate;
            case GizmoMode::Scale:
                return TransformMode::Scale;
            case GizmoMode::Move:
            default:
                return TransformMode::Move;
        }
    }

    const char *ConstraintName(TransformConstraint constraint) {
        switch (constraint) {
            case TransformConstraint::XAxis:
                return "X";
            case TransformConstraint::YAxis:
                return "Y";
            case TransformConstraint::ZAxis:
                return "Z";
            case TransformConstraint::XYPlane:
                return "XY";
            case TransformConstraint::YZPlane:
                return "YZ";
            case TransformConstraint::ZXPlane:
                return "ZX";
            case TransformConstraint::Free:
            default:
                return "Free";
        }
    }

    const char *HandleTypeName(GizmoHandleType type) {
        switch (type) {
            case GizmoHandleType::Axis:
                return "axis";
            case GizmoHandleType::Ring:
                return "ring";
            case GizmoHandleType::Plane:
                return "plane";
            case GizmoHandleType::Center:
            default:
                return "center";
        }
    }

    ElementMesh BuildAxisWidgetMesh(const gp_Pnt &pivot, TransformConstraint axis) {
        ElementMesh mesh;
        const gp_Pnt end = pivot.Translated(gp_Vec(AxisDirection(axis)) * GizmoAxisLength);
        mesh.vertices.push_back(MeshVertex{pivot.XYZ(), {}});
        mesh.vertices.push_back(MeshVertex{end.XYZ(), {}});
        mesh.segments.push_back(SegmentIndex{0, 1});
        return mesh;
    }

    ElementMesh BuildCenterWidgetMesh(const gp_Pnt &pivot) {
        ElementMesh mesh;
        mesh.vertices.push_back(MeshVertex{pivot.XYZ(), {}});
        mesh.points.push_back(PointIndex{0});
        return mesh;
    }

    void PlaneBasis(TransformConstraint plane, gp_Dir &u, gp_Dir &v) {
        switch (plane) {
            case TransformConstraint::XYPlane:
                u = gp::DX();
                v = gp::DY();
                break;
            case TransformConstraint::YZPlane:
                u = gp::DY();
                v = gp::DZ();
                break;
            case TransformConstraint::ZXPlane:
                u = gp::DZ();
                v = gp::DX();
                break;
            default:
                u = gp::DX();
                v = gp::DY();
                break;
        }
    }

    ElementMesh BuildPlaneWidgetMesh(const gp_Pnt &pivot, TransformConstraint plane) {
        ElementMesh mesh;
        gp_Dir u;
        gp_Dir v;
        PlaneBasis(plane, u, v);

        const gp_Vec uOffset = gp_Vec(u) * GizmoPlaneOffset;
        const gp_Vec vOffset = gp_Vec(v) * GizmoPlaneOffset;
        const gp_Vec uSize = gp_Vec(u) * GizmoPlaneSize;
        const gp_Vec vSize = gp_Vec(v) * GizmoPlaneSize;

        const gp_Pnt p0 = pivot.Translated(uOffset + vOffset);
        const gp_Pnt p1 = p0.Translated(uSize);
        const gp_Pnt p2 = p0.Translated(uSize + vSize);
        const gp_Pnt p3 = p0.Translated(vSize);

        mesh.vertices.push_back(MeshVertex{p0.XYZ(), {}});
        mesh.vertices.push_back(MeshVertex{p1.XYZ(), {}});
        mesh.vertices.push_back(MeshVertex{p2.XYZ(), {}});
        mesh.vertices.push_back(MeshVertex{p3.XYZ(), {}});
        mesh.triangles.push_back(TriangleIndex{0, 1, 2});
        mesh.triangles.push_back(TriangleIndex{0, 2, 3});
        return mesh;
    }

    void RingBasis(TransformConstraint axis, gp_Dir &u, gp_Dir &v) {
        switch (axis) {
            case TransformConstraint::XAxis:
                u = gp::DY();
                v = gp::DZ();
                break;
            case TransformConstraint::YAxis:
                u = gp::DX();
                v = gp::DZ();
                break;
            case TransformConstraint::ZAxis:
            case TransformConstraint::Free:
            default:
                u = gp::DX();
                v = gp::DY();
                break;
        }
    }

    ElementMesh BuildRingWidgetMesh(const gp_Pnt &pivot, TransformConstraint axis) {
        ElementMesh mesh;
        gp_Dir u;
        gp_Dir v;
        RingBasis(axis, u, v);

        mesh.vertices.reserve(GizmoRingSegments);
        mesh.segments.reserve(GizmoRingSegments);
        for (uint32_t index = 0; index < GizmoRingSegments; ++index) {
            const double angle = 2.0 * std::numbers::pi * static_cast<double>(index) /
                                 static_cast<double>(GizmoRingSegments);
            const gp_Vec offset = gp_Vec(u) * (std::cos(angle) * GizmoRotateRadius) +
                                  gp_Vec(v) * (std::sin(angle) * GizmoRotateRadius);
            mesh.vertices.push_back(MeshVertex{pivot.Translated(offset).XYZ(), {}});
        }

        for (uint32_t index = 0; index < GizmoRingSegments; ++index) {
            mesh.segments.push_back(SegmentIndex{index, (index + 1) % GizmoRingSegments});
        }
        return mesh;
    }

    constexpr double GizmoTargetPixelLength = 100.0;
}

GizmoManager::GizmoManager(InteractionContext *context) : m_Context(context) {
}

void GizmoManager::Show(const gp_Pnt &pivot,
                        GizmoMode mode,
                        TransformConstraint constraint,
                        std::optional<gp_Pnt> constraintPivot) {
    const bool shouldRebuildWidgets =
        !m_State.visible ||
        m_State.mode != mode ||
        m_State.constraint != constraint;

    m_State.visible = true;
    m_State.pivot = pivot;
    m_State.constraintPivot = std::move(constraintPivot);
    m_State.mode = mode;
    m_State.constraint = constraint;
    SyncView();
    if (shouldRebuildWidgets) {
        SyncSceneWidgets();
    } else {
        SyncSceneWidgetTransforms();
    }
}

void GizmoManager::Hide() {
    m_State.visible = false;
    m_State.constraintPivot.reset();
    m_State.hovered.reset();
    m_State.active.reset();
    ClearSceneWidgets();
    SyncView();
}

void GizmoManager::SetMode(GizmoMode mode) {
    m_State.mode = mode;
    SyncView();
    SyncSceneWidgets();
}

void GizmoManager::SetConstraint(TransformConstraint constraint) {
    m_State.constraint = constraint;
    SyncView();
    SyncSceneWidgets();
}

void GizmoManager::UpdatePose(const gp_Pnt &pivot, std::optional<gp_Pnt> constraintPivot) {
    if (!m_State.visible) {
        return;
    }

    m_State.pivot = pivot;
    m_State.constraintPivot = std::move(constraintPivot);
    SyncView();
    SyncSceneWidgetTransforms();
}

void GizmoManager::RefreshViewState() {
    if (!m_State.visible) {
        return;
    }

    SyncView();
    SyncSceneWidgetTransforms();
}

std::optional<GizmoPickTarget> GizmoManager::Pick(int mouseX, int mouseY) const {
    if (m_Context == nullptr || m_Context->m_View.IsNull() || m_Context->m_Scene == nullptr || !m_State.visible) {
        return std::nullopt;
    }

    PickSettings settings;
    settings.pickMask = PickMask::Point | PickMask::Segment | PickMask::Triangle;
    settings.pointTolerance = GizmoCenterPickTolerance;
    settings.segmentTolerance = GizmoPickTolerance;
    settings.broadPhaseTolerance = std::max(GizmoCenterPickTolerance, GizmoPickTolerance);

    const gp_Ax1 mouseRay = GeomCalculator::GetMouseScreenRay(mouseX, mouseY, m_Context->m_View);
    ScenePicker picker(m_Context->m_Scene);
    const auto pick = picker.Pick(PickQuery{gp_Lin(mouseRay.Location(), mouseRay.Direction()), settings});
    if (!pick.has_value()) {
        return std::nullopt;
    }

    const auto gizmoTarget = std::get_if<GizmoPickTarget>(&pick->pickTarget);
    if (gizmoTarget == nullptr) {
        return std::nullopt;
    }

    return *gizmoTarget;
}

bool GizmoManager::UpdateHover(int mouseX, int mouseY) {
    const auto pick = Pick(mouseX, mouseY);
    const std::optional<GizmoHandleId> handle = pick.has_value()
                                                    ? std::optional<GizmoHandleId>(pick->handle)
                                                    : std::nullopt;
    if (handle == m_State.hovered) {
        return handle.has_value();
    }

    SetHovered(handle);
    if (handle.has_value()) {
        qDebug() << "[GizmoPick] hover" << HandleTypeName(handle->type) << ConstraintName(handle->constraint);
    }
    return handle.has_value();
}

void GizmoManager::SetHovered(std::optional<GizmoHandleId> handle) {
    m_State.hovered = std::move(handle);
}

void GizmoManager::SetActive(std::optional<GizmoHandleId> handle) {
    m_State.active = std::move(handle);
}

const GizmoState &GizmoManager::State() const {
    return m_State;
}

void GizmoManager::SyncView() const {
    if (m_Context == nullptr || m_Context->m_ViewStateAdaptor == nullptr) {
        return;
    }

    const auto guideState = std::make_shared<TransformGuideState>();
    guideState->visible = m_State.visible;
    guideState->pivot = m_State.pivot;
    guideState->constraintPivot = m_State.constraintPivot;
    guideState->constraint = m_State.constraint;
    guideState->mode = ToTransformMode(m_State.mode);
    guideState->transform = BuildGizmoWorldTransform();
    m_Context->m_ViewStateAdaptor->ShowTransformGuide(guideState);
}

void GizmoManager::SyncSceneWidgets() const {
    if (m_Context == nullptr || m_Context->m_Scene == nullptr) {
        return;
    }

    m_Context->m_Scene->ClearWidgets();
    if (!m_State.visible) {
        return;
    }

    const auto addAxisWidget = [this](TransformConstraint axis) {
        m_Context->m_Scene->AddWidget(
            BuildAxisWidgetMesh(gp::Origin(), axis),
            GizmoHandleId{m_State.mode, axis, GizmoHandleType::Axis});
    };

    const auto addCenterWidget = [this]() {
        m_Context->m_Scene->AddWidget(
            BuildCenterWidgetMesh(gp::Origin()),
            GizmoHandleId{m_State.mode, TransformConstraint::Free, GizmoHandleType::Center});
    };

    const auto addPlaneWidget = [this](TransformConstraint plane) {
        m_Context->m_Scene->AddWidget(
            BuildPlaneWidgetMesh(gp::Origin(), plane),
            GizmoHandleId{m_State.mode, plane, GizmoHandleType::Plane});
    };

    const auto addRingWidget = [this](TransformConstraint axis) {
        m_Context->m_Scene->AddWidget(
            BuildRingWidgetMesh(gp::Origin(), axis),
            GizmoHandleId{m_State.mode, axis, GizmoHandleType::Ring});
    };

    if (m_State.mode == GizmoMode::Rotate) {
        if (m_State.constraint == TransformConstraint::Free) {
            addRingWidget(TransformConstraint::XAxis);
            addRingWidget(TransformConstraint::YAxis);
            addRingWidget(TransformConstraint::ZAxis);
            SyncSceneWidgetTransforms();
            return;
        }

        addRingWidget(m_State.constraint);
        SyncSceneWidgetTransforms();
        return;
    }

    if (m_State.mode != GizmoMode::Move && m_State.mode != GizmoMode::Scale) {
        return;
    }

    if (m_State.constraint == TransformConstraint::Free) {
        if (m_State.mode == GizmoMode::Move) {
            addCenterWidget();
            addPlaneWidget(TransformConstraint::XYPlane);
            addPlaneWidget(TransformConstraint::YZPlane);
            addPlaneWidget(TransformConstraint::ZXPlane);
        }
        addAxisWidget(TransformConstraint::XAxis);
        addAxisWidget(TransformConstraint::YAxis);
        addAxisWidget(TransformConstraint::ZAxis);
        SyncSceneWidgetTransforms();
        return;
    }

    if (m_State.constraint == TransformConstraint::XYPlane ||
        m_State.constraint == TransformConstraint::YZPlane ||
        m_State.constraint == TransformConstraint::ZXPlane) {
        addPlaneWidget(m_State.constraint);
        SyncSceneWidgetTransforms();
        return;
    }

    addAxisWidget(m_State.constraint);
    SyncSceneWidgetTransforms();
}

void GizmoManager::SyncSceneWidgetTransforms() const {
    if (m_Context != nullptr && m_Context->m_Scene != nullptr) {
        m_Context->m_Scene->SetWidgetTransform(BuildGizmoWorldTransform());
    }
}

GeometryTypes::RTransform GizmoManager::BuildGizmoWorldTransform() const {
    Handle(V3d_View) view;
    if (m_Context != nullptr) {
        view = m_Context->m_View;
    }

    const double targetLength = GeomCalculator::GetPersistenceScreenSize(
        view,
        static_cast<int>(GizmoTargetPixelLength),
        m_State.pivot.XYZ());
    const double scale = std::max(targetLength / GizmoAxisLength, Precision::Confusion());

    GeometryTypes::RTransform transform;
    transform.SetScale(gp::Origin(), scale);
    transform.SetTranslationPart(gp_Vec(gp::Origin(), m_State.pivot));
    return transform;
}

void GizmoManager::ClearSceneWidgets() const {
    if (m_Context != nullptr && m_Context->m_Scene != nullptr) {
        m_Context->m_Scene->ClearWidgets();
    }
}
