#include "canvas.h"
#include <gtest/gtest.h>

class CanvasTest : public ::testing::Test {
protected:
    Canvas* canvas;

    void SetUp() override {
        canvas = new Canvas();
    }

    void TearDown() override {
        delete canvas;
    }
};

TEST_F(CanvasTest, ClearCanvas) {
    canvas->draw(); // Assume draw modifies the canvas
    canvas->clear();
    // Add assertions to verify the canvas is cleared
}

TEST_F(CanvasTest, DrawOnCanvas) {
    // Add shapes to the canvas and verify they are drawn correctly
    // Example: canvas->addShape(new Circle());
    // Add assertions to verify the shapes are drawn
}