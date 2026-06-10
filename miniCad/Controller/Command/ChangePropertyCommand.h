//
// Created by ZQD on 2026/6/4.
//

#pragma once
#include "Command.h"
#include "../../Data/Element/ElementId.h"
#include "Property/PropertyValue.h"


class ChangePropertyCommand : public Command {
public:
    ChangePropertyCommand(Document *document, ElementId elementId, const std::string &propertyKey,
                          const PropertyValue &newValue);

    void Execute() override;

    void Undo() override;

    void Redo() override;

private:
    ElementId m_ElementId;
    std::string m_PropertyKey;
    PropertyValue m_OldValue;
    PropertyValue m_NewValue;
    bool m_HasOldValue = false;
};


