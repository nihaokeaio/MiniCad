//
// Created by ZQD on 2026/6/11.
//

#pragma once

/// OCCT
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Vec2f.hxx>

/// QT
#include <gp_GTrsf.hxx>
#include <QVector3D>
#include <QVector2D>
#include <QMatrix4x4>

namespace Types {
    using Vec3f = gp_Vec;
    using Point3f = gp_Pnt;
    using Vec2f = gp_Vec2f;
    using Vec2d = gp_Vec2d;
    using Point2d = gp_Pnt2d;
    using Bound3d = Bnd_Box;

    inline QVector3D ConvertFromOcct3D(const gp_XYZ &xyz) {
        return QVector3D(xyz.X(), xyz.Y(), xyz.Z());
    }

    inline QVector2D ConvertFromOcct2D(const gp_XY &xy) {
        return QVector2D(xy.X(), xy.Y());
    }

    inline QMatrix4x4 ConvertFromOcctMat4x4(const gp_GTrsf &mat) {
        NCollection_Mat4<float> theMat;
        mat.GetMat4<float>(theMat);
        return QMatrix4x4(theMat.GetData());
    }
}





