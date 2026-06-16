//
// Created by ZQD on 2026/6/16.
//

#pragma once

#include <Standard_Handle.hxx>
#include <gp_Trsf.hxx>

class AIS_InteractiveContext;
class AIS_Shape;
class SelectionManager;
class TopoDS_Shape;
class ViewObjectRegistry;

class ViewStateAdaptor {
public:
    ViewStateAdaptor(const Handle(AIS_InteractiveContext) &context, ViewObjectRegistry *registry);

    void ShowPreviewShape(const TopoDS_Shape &shape);

    void UpdatePreviewShape(const TopoDS_Shape &shape);

    void UpdatePreviewTransform(const gp_Trsf &transform);

    void ClearPreview();

    void ApplySelection(const SelectionManager &selectionManager) const;

    void ClearSelection() const;

private:
    void ClearSelection(bool updateViewer) const;

    Handle(AIS_InteractiveContext) m_Context;
    ViewObjectRegistry *m_Registry = nullptr;
    Handle(AIS_Shape) m_PreviewAis;
};
