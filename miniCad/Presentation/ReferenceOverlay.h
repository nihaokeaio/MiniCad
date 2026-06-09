//
// Created by ZQD on 2026/6/9.
//

#pragma once

#include <Standard_Handle.hxx>
#include <vector>

class AIS_InteractiveContext;
class AIS_InteractiveObject;

class ReferenceOverlay {
public:
    explicit ReferenceOverlay(const Handle(AIS_InteractiveContext) &context);

    void ShowGrid(bool visible);

    void ShowAxes(bool visible);

    void SetGridSize(double size);

    void SetGridStep(double step);

private:
    void RebuildGrid();

    void RebuildAxes();

    void ClearObjects(std::vector<Handle(AIS_InteractiveObject)> &objects);

private:
    Handle(AIS_InteractiveContext) m_Context;
    std::vector<Handle(AIS_InteractiveObject)> m_GridObjects;
    std::vector<Handle(AIS_InteractiveObject)> m_AxisObjects;
    double m_GridSize = 1000.0;
    double m_GridStep = 100.0;
    bool m_ShowGrid = false;
    bool m_ShowAxes = false;
};
