//
// Created by ZQD on 2026/6/17.
//

#pragma once

#include "Command.h"

#include <vector>

#include "Element/ElementId.h"
#include "Geometry/GeometryTypes.h"

struct ElementTransformChange {
    ElementId elementId{ElementId::InvalidId};
    GeometryTypes::RTransform oldTransform;
    GeometryTypes::RTransform newTransform;
};

class TransformElementsCommand final : public Command {
public:
    TransformElementsCommand(Document *document, std::vector<ElementTransformChange> changes);

    void Execute() override;

    void Undo() override;

    void Redo() override;

private:
    void Apply(bool useNewTransform);

    std::vector<ElementTransformChange> m_Changes;
};
