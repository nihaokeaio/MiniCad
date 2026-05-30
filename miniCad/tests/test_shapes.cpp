#include <gtest/gtest.h>
#include "shapes.h"

TEST(CircleTest, Draw) {
    Circle circle;
    circle.setPosition(5, 5);
    // Assuming draw() returns a string representation for testing
    EXPECT_EQ(circle.draw(), "Drawing Circle at (5, 5)");
}

TEST(RectangleTest, Draw) {
    Rectangle rectangle;
    rectangle.setPosition(10, 10);
    // Assuming draw() returns a string representation for testing
    EXPECT_EQ(rectangle.draw(), "Drawing Rectangle at (10, 10)");
}

TEST(CircleTest, Area) {
    Circle circle;
    circle.setRadius(3);
    EXPECT_EQ(calculateArea(circle), 28.27); // Assuming π is approximated as 3.14
}

TEST(RectangleTest, Area) {
    Rectangle rectangle;
    rectangle.setDimensions(4, 5);
    EXPECT_EQ(calculateArea(rectangle), 20);
}