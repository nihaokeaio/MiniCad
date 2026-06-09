//
// Created by ZQD on 2026/6/9.
//

#include "ReferenceOverlay.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>

namespace {
Handle(AIS_Shape) MakeLineObject(const gp_Pnt &from, const gp_Pnt &to, const Quantity_Color &color,
                                 double width = 1.0) {
    const TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(from, to);
    Handle(AIS_Shape) ais = new AIS_Shape(edge);
    ais->SetColor(color);
    ais->Attributes()->SetLineAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
    ais->Attributes()->SetWireAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
    return ais;
}
}

ReferenceOverlay::ReferenceOverlay(const Handle(AIS_InteractiveContext) &context) : m_Context(context) {
}

void ReferenceOverlay::ShowGrid(bool visible) {
    m_ShowGrid = visible;
    RebuildGrid();
}

void ReferenceOverlay::ShowAxes(bool visible) {
    m_ShowAxes = visible;
    RebuildAxes();
}

void ReferenceOverlay::SetGridSize(double size) {
    if (size <= 0.0) {
        return;
    }
    m_GridSize = size;
    RebuildGrid();
}

void ReferenceOverlay::SetGridStep(double step) {
    if (step <= 0.0) {
        return;
    }
    m_GridStep = step;
    RebuildGrid();
}

void ReferenceOverlay::RebuildGrid() {
    ClearObjects(m_GridObjects);
    if (!m_ShowGrid || m_Context.IsNull()) {
        return;
    }

    const Quantity_Color minorColor(0.32, 0.32, 0.32, Quantity_TOC_RGB);
    const Quantity_Color majorColor(0.45, 0.45, 0.45, Quantity_TOC_RGB);
    const int lineCount = static_cast<int>(m_GridSize / m_GridStep);

    for (int i = -lineCount; i <= lineCount; ++i) {
        const double value = i * m_GridStep;
        const bool isMajor = i % 5 == 0;
        const auto &color = isMajor ? majorColor : minorColor;

        auto xLine = MakeLineObject(gp_Pnt(-m_GridSize, value, 0.0), gp_Pnt(m_GridSize, value, 0.0), color);
        auto yLine = MakeLineObject(gp_Pnt(value, -m_GridSize, 0.0), gp_Pnt(value, m_GridSize, 0.0), color);

        m_Context->Display(xLine, 0, -1, Standard_False);
        m_Context->Display(yLine, 0, -1, Standard_False);
        m_GridObjects.push_back(xLine);
        m_GridObjects.push_back(yLine);
    }

    m_Context->UpdateCurrentViewer();
}

void ReferenceOverlay::RebuildAxes() {
    ClearObjects(m_AxisObjects);
    if (!m_ShowAxes || m_Context.IsNull()) {
        return;
    }

    const double axisSize = m_GridSize * 1.1;
    auto xAxis = MakeLineObject(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(axisSize, 0.0, 0.0),
                                Quantity_Color(0.85, 0.15, 0.12, Quantity_TOC_RGB), 2.0);
    auto yAxis = MakeLineObject(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, axisSize, 0.0),
                                Quantity_Color(0.20, 0.75, 0.20, Quantity_TOC_RGB), 2.0);
    auto zAxis = MakeLineObject(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, axisSize),
                                Quantity_Color(0.20, 0.40, 0.90, Quantity_TOC_RGB), 2.0);

    m_Context->Display(xAxis, 0, -1, Standard_False);
    m_Context->Display(yAxis, 0, -1, Standard_False);
    m_Context->Display(zAxis, 0, -1, Standard_False);

    m_AxisObjects.push_back(xAxis);
    m_AxisObjects.push_back(yAxis);
    m_AxisObjects.push_back(zAxis);
    m_Context->UpdateCurrentViewer();
}

void ReferenceOverlay::ClearObjects(std::vector<Handle(AIS_InteractiveObject)> &objects) {
    if (m_Context.IsNull()) {
        objects.clear();
        return;
    }

    for (const auto &object: objects) {
        if (!object.IsNull()) {
            m_Context->Remove(object, Standard_False);
        }
    }
    objects.clear();
    m_Context->UpdateCurrentViewer();
}
