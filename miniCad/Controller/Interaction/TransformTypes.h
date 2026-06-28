//
// Created by ZQD on 2026/6/28.
//

#pragma once

namespace TransformElementSpace {
    enum class TransformState {
        Idle,
        Dragging
    };

    enum class TransformMode {
        Move,
        Rotate,
        Scale
    };

    enum class TransformConstraint {
        XAxis,
        YAxis,
        ZAxis,
        XYPlane,
        YZPlane,
        ZXPlane,
        Free
    };
}
