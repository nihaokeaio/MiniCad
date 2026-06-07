//
// Created by ZQD on 2026/6/1.
//

#pragma once
#include <string>

#include "ElementCreateParams.h"
#include "ElementId.h"
#include "ElementType.h"
#include "Property/PropertyValue.h"

class CommandManager;
class Document;

class CadController {
public:
    CadController(Document *document, CommandManager *commandManager);

    void CreateBox() const;

    void CreateCylinder() const;

    void CreateElement(const ElementCreateParams &params) const;

    void ChangeElementProperty(ElementId id, const std::string &key, const PropertyValue &value);

    void Undo() const;

    void Redo() const;

private:
    Document *m_Document = nullptr;
    CommandManager *m_CommandManager = nullptr;
};
