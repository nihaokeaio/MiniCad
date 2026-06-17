//
// Created by ZQD on 2026/6/1.
//

#pragma once
#include <string>
#include <vector>

#include "ElementCreateParams.h"
#include "../Data/Element/ElementId.h"
#include "../Data/Element/ElementType.h"
#include "Property/PropertyValue.h"

class CommandManager;
class Document;
struct ElementTransformChange;

class CadController {
public:
    CadController(Document *document, CommandManager *commandManager);

    void CreateElement(ElementType elementType) const;

    void CreateElement(const ElementCreateParams &params) const;

    void ChangeElementProperty(ElementId id, const std::string &key, const PropertyValue &value);

    void TransformElements(const std::vector<ElementTransformChange> &changes) const;

    void Undo() const;

    void Redo() const;

private:
    Document *m_Document = nullptr;
    CommandManager *m_CommandManager = nullptr;
};
