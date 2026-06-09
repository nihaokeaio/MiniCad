//
// Created by ZQD on 2026/6/9.
//

#pragma once
#include <Standard_Handle.hxx>

#include "ElementId.h"


struct InteractionContext;
class V3d_View;

class ViewController {
public:
    ViewController(InteractionContext *interactionContext);

public:
    void FitAll() const;

    void FocusSelection() const;

    void FocusElement(ElementId id) const;

    void SetOrthographic() const;

    void SetPerspective() const;

    void SetAxoView() const;

    void SetTopView() const;

private:
    InteractionContext *m_InteractionContext;
};


