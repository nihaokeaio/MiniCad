//
// Created by ZQD on 2026/6/3.
//

#include "ElementFactory.h"

#include "BoxElement.h"
#include "CylinderElement.h"
#include "PointElement.h"
#include "SegmentElement.h"

std::unique_ptr<Element> ElementFactory::Create(ElementType type) {
    // TODO: Replace this manual registration switch with a type registry or macro-assisted registration
    // once element creation points grow beyond simple built-in primitives.
    switch (type) {
        case ElementType::Box:
            return std::make_unique<BoxElement>();
        case ElementType::Cylinder:
            return std::make_unique<CylinderElement>();
        case ElementType::Point:
            return std::make_unique<PointElement>();
        case ElementType::Segment:
            return std::make_unique<SegmentElement>();
        default:
            return nullptr;
    }
}
