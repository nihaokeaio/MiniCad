#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "canvas.h"
#include "shapes.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Canvas canvas;

    QMainWindow window;
    QWidget* central = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(central);
    QLabel* statusLabel = new QLabel("点击“创建图形”按钮，生成 OpenCascade 几何体。");
    QPushButton* createButton = new QPushButton("创建图形");

    layout->addWidget(statusLabel);
    layout->addWidget(createButton);
    central->setLayout(layout);
    window.setCentralWidget(central);
    window.setWindowTitle("miniCad 最小演示");
    window.resize(420, 150);

    QObject::connect(createButton, &QPushButton::clicked, [&canvas, statusLabel]() {
        Circle circle(15.0);
        circle.setPosition(0, 0);
        canvas.draw(circle);

        Rectangle rectangle(60.0, 30.0);
        rectangle.setPosition(40, 10);
        canvas.draw(rectangle);

        statusLabel->setText(QString("已创建 %1 个 OpenCascade 图元").arg(canvas.shapeCount()));
    });

    window.show();
    return app.exec();
}