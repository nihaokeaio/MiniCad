//
// Created by ZQD on 2026/6/7.
//

#pragma once
#include <Standard_Handle.hxx>
#include <gp_Trsf.hxx>


class AIS_InteractiveContext;
class AIS_Shape;
class TopoDS_Shape;

class PreviewAdaptor {
public:
    explicit PreviewAdaptor(const Handle(AIS_InteractiveContext) &context);

    void ShowShape(const TopoDS_Shape &shape);

    void UpdateShape(const TopoDS_Shape &shape);

    void UpdateTransform(const gp_Trsf &transform);

    void Clear();

private:
    Handle(AIS_InteractiveContext) m_Context;
    Handle(AIS_Shape) m_PreviewAis;
};


