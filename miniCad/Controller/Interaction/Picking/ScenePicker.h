//
// Created by ZQD on 2026/6/10.
//

#ifndef MINICAD_SCENEPICKER_H
#define MINICAD_SCENEPICKER_H
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <optional>

#include "ElementMesh/ElementMesh.h"
#include "Element/ElementId.h"


class Scene;
struct SceneObject;

struct PickResult {
    ElementId elementId{ElementId::InvalidId};
    MeshPrimitiveType primitiveType{MeshPrimitiveType::None};
    uint32_t primitiveIndex{InvalidPrimitiveIndex};
    double distance{std::numeric_limits<double>::max()};
    gp_Pnt hitPoint;
};


class ScenePicker {
public:
    explicit ScenePicker(const Scene *scene);

    std::optional<PickResult> Pick(const gp_Lin &ray) const;

private:
    std::optional<PickResult> PickElementMesh(const gp_Lin &localRay,
                                              const SceneObject &object,
                                              double pointTolerance,
                                              double segmentTolerance) const;

    std::optional<PickResult> PickMeshPoints(const gp_Lin &localRay,
                                             const SceneObject &object,
                                             double pointTolerance) const;

    std::optional<PickResult> PickMeshSegments(const gp_Lin &localRay,
                                               const SceneObject &object,
                                               double segmentTolerance) const;

    std::optional<PickResult> PickMeshTriangles(const gp_Lin &localRay,
                                                const SceneObject &object) const;

    [[nodiscard]] static PickResult MakeObjectPick(ElementId elementId,
                                                   const gp_Lin &ray,
                                                   double distance);

    [[nodiscard]] static bool IsBetterPick(const PickResult &candidate, const PickResult &current);

    static void UpdateBestPick(std::optional<PickResult> &best, const PickResult &candidate);

    const Scene *m_Scene;
};


#endif //MINICAD_SCENEPICKER_H
