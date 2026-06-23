//
// Created by ZQD on 2026/6/17.
//

#include "TransformElementsCommand.h"

#include "Document.h"
#include "Element/Element.h"
#include "Property/PropertyValue.h"

TransformElementsCommand::TransformElementsCommand(Document *document, std::vector<ElementTransformChange> changes)
    : Command(document), m_Changes(std::move(changes)) {
}

void TransformElementsCommand::Execute() {
    Apply(true);
}

void TransformElementsCommand::Undo() {
    Apply(false);
}

void TransformElementsCommand::Redo() {
    Apply(true);
}

void TransformElementsCommand::Apply(bool useNewTransform) const {
    for (const auto &change: m_Changes) {
        if (!change.elementId.IsValid()) {
            continue;
        }

        if (const auto element = m_Document->FindElement(change.elementId)) {
            const auto &transform = useNewTransform ? change.newTransform : change.oldTransform;
            element->SetProperty("LocalTransform", PropertyValue(transform));
        }
    }
}
