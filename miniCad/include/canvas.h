#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Ais_Shape.hxx>
#include "shapes.h"

class Canvas {
public:
    Canvas();

    bool isReady() const;
    void clear();
    void draw(const Shape& shape);
    int shapeCount() const;

private:
    Handle(Graphic3d_GraphicDriver) m_graphicDriver;
    Handle(V3d_Viewer) m_viewer;
    Handle(AIS_InteractiveContext) m_context;
    std::vector<Handle(AIS_Shape)> m_shapes;
};

#endif // CANVAS_H