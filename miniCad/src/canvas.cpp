#include "canvas.h"
#include <AIS_Shape.hxx>
#include <Quantity_NameOfColor.hxx>
#include <TCollection_ExtendedString.hxx>

Canvas::Canvas() {
    m_graphicDriver = new OpenGl_GraphicDriver(new Aspect_DisplayConnection());
    m_viewer = new V3d_Viewer(m_graphicDriver);
    m_viewer->SetDefaultLights();
    m_context = new AIS_InteractiveContext(m_viewer);
}

bool Canvas::isReady() const {
    return !m_context.IsNull();
}

void Canvas::clear() {
    if (!isReady()) {
        return;
    }
    m_context->RemoveAll(false);
    m_shapes.clear();
    m_context->UpdateCurrentViewer();
}

void Canvas::draw(const Shape& shape) {
    if (!isReady()) {
        return;
    }
    TopoDS_Shape occShape = shape.toTopoDS();
    Handle(AIS_Shape) aisShape = new AIS_Shape(occShape);
    m_shapes.push_back(aisShape);
    m_context->Display(aisShape, false);
    m_context->UpdateCurrentViewer();
}

int Canvas::shapeCount() const {
    return static_cast<int>(m_shapes.size());
}