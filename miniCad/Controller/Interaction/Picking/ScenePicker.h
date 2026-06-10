//
// Created by ZQD on 2026/6/10.
//

#ifndef MINICAD_SCENEPICKER_H
#define MINICAD_SCENEPICKER_H
#include <gp_Lin.hxx>
#include <optional>

#include "Element/ElementId.h"


class Scene;

struct PickResult {
    ElementId elementId{ElementId::InvalidId};
    double distance{std::numeric_limits<double>::max()};
};


class ScenePicker {
public:
    explicit ScenePicker(const Scene *scene);

    std::optional<PickResult> PickByBoundingBox(const gp_Lin &ray) const;

private:
    const Scene *m_Scene;
};


#endif //MINICAD_SCENEPICKER_H
