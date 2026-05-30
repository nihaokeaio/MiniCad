#include "utils.h"
#include <cmath>

double calculateArea(const Shape& shape) {
    // Assuming Shape has a method to get the type and dimensions
    if (shape.getType() == "Circle") {
        double radius = shape.getRadius();
        return M_PI * radius * radius;
    } else if (shape.getType() == "Rectangle") {
        double width = shape.getWidth();
        double height = shape.getHeight();
        return width * height;
    }
    return 0.0; // Default case for unsupported shapes
}

double calculatePerimeter(const Shape& shape) {
    if (shape.getType() == "Circle") {
        double radius = shape.getRadius();
        return 2 * M_PI * radius;
    } else if (shape.getType() == "Rectangle") {
        double width = shape.getWidth();
        double height = shape.getHeight();
        return 2 * (width + height);
    }
    return 0.0; // Default case for unsupported shapes
}