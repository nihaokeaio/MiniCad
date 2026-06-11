//
// Created by ZQD on 2026/6/10.
//

#include "ElementMesh.h"

#include <utility>

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

namespace {
    GeometryTypes::Vector3D ComputeTriangleNormal(const gp_Pnt &p0, const gp_Pnt &p1, const gp_Pnt &p2) {
        gp_Vec edge0(p0, p1);
        gp_Vec edge1(p0, p2);
        gp_Vec normal = edge0.Crossed(edge1);
        if (normal.SquareMagnitude() <= gp::Resolution()) {
            return {};
        }

        normal.Normalize();
        return normal.XYZ();
    }

    uint32_t AddVertex(ElementMesh &mesh, const gp_Pnt &point, const GeometryTypes::Vector3D &normal) {
        const auto index = static_cast<uint32_t>(mesh.vertices.size());
        mesh.vertices.push_back(MeshVertex{point.XYZ(), normal});
        return index;
    }

    void AddPointPrimitive(ElementMesh &mesh, const gp_Pnt &point) {
        const uint32_t vertex = AddVertex(mesh, point, {});
        mesh.points.push_back(PointIndex{vertex});
    }

    void AddSegmentPrimitive(ElementMesh &mesh, const gp_Pnt &start, const gp_Pnt &end) {
        if (start.SquareDistance(end) <= gp::Resolution()) {
            return;
        }

        const uint32_t v0 = AddVertex(mesh, start, {});
        const uint32_t v1 = AddVertex(mesh, end, {});
        mesh.segments.push_back(SegmentIndex{v0, v1});
    }

    void AppendShapeTriangles(ElementMesh &mesh, const TopoDS_Shape &shape) {
        for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
            const TopoDS_Face face = TopoDS::Face(explorer.Current());
            TopLoc_Location location;
            const Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
            if (triangulation.IsNull()) {
                continue;
            }

            const gp_Trsf faceTransform = location.Transformation();
            for (int triangleIndex = 1; triangleIndex <= triangulation->NbTriangles(); ++triangleIndex) {
                int node0 = 0;
                int node1 = 0;
                int node2 = 0;
                triangulation->Triangle(triangleIndex).Get(node0, node1, node2);

                if (face.Orientation() == TopAbs_REVERSED) {
                    std::swap(node1, node2);
                }

                gp_Pnt point0 = triangulation->Node(node0).Transformed(faceTransform);
                gp_Pnt point1 = triangulation->Node(node1).Transformed(faceTransform);
                gp_Pnt point2 = triangulation->Node(node2).Transformed(faceTransform);
                const auto normal = ComputeTriangleNormal(point0, point1, point2);

                const uint32_t v0 = AddVertex(mesh, point0, normal);
                const uint32_t v1 = AddVertex(mesh, point1, normal);
                const uint32_t v2 = AddVertex(mesh, point2, normal);
                mesh.triangles.push_back(TriangleIndex{v0, v1, v2});
            }
        }
    }

    void AppendShapePoints(ElementMesh &mesh, const TopoDS_Shape &shape) {
        for (TopExp_Explorer explorer(shape, TopAbs_VERTEX); explorer.More(); explorer.Next()) {
            const TopoDS_Vertex vertex = TopoDS::Vertex(explorer.Current());
            AddPointPrimitive(mesh, BRep_Tool::Pnt(vertex));
        }
    }

    void AppendShapeSegments(ElementMesh &mesh, const TopoDS_Shape &shape, double deflection) {
        for (TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()) {
            const TopoDS_Edge edge = TopoDS::Edge(explorer.Current());
            BRepAdaptor_Curve curve(edge);
            GCPnts_UniformDeflection sampler(curve, deflection);
            if (!sampler.IsDone() || sampler.NbPoints() < 2) {
                AddSegmentPrimitive(mesh, curve.Value(curve.FirstParameter()), curve.Value(curve.LastParameter()));
                continue;
            }

            gp_Pnt previous = sampler.Value(1);
            for (int index = 2; index <= sampler.NbPoints(); ++index) {
                const gp_Pnt current = sampler.Value(index);
                AddSegmentPrimitive(mesh, previous, current);
                previous = current;
            }
        }
    }
}

ElementMesh ElementMeshBuilder::BuildFromShape(const TopoDS_Shape &shape, const ElementMeshBuildOptions &options) {
    ElementMesh mesh;
    if (shape.IsNull()) {
        return mesh;
    }

    TopoDS_Shape triangulatedShape = shape;
    if (options.cleanBeforeBuild) {
        BRepTools::Clean(triangulatedShape);
    }

    bool triangulationReady = false;
    try {
        BRepMesh_IncrementalMesh mesher(
            triangulatedShape,
            options.deflection,
            options.relative,
            options.angleDeflection,
            options.parallel);
        mesher.Perform();
        triangulationReady = mesher.IsDone();
    } catch (const Standard_Failure &) {
        triangulationReady = false;
    }

    if (triangulationReady) {
        AppendShapeTriangles(mesh, triangulatedShape);
    }
    AppendShapeSegments(mesh, triangulatedShape, options.deflection);
    AppendShapePoints(mesh, triangulatedShape);

    return mesh;
}
