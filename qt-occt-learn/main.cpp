#include <QString>
#include <QtGlobal>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <cmath>
#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    std::cout << "Qt version: " << qVersion() << '\n';
    std::cout << QStringLiteral("Qt + OCCT ready").toStdString() << '\n';

    const gp_Pnt origin(0, 0, 0);
    const gp_Pnt point(3, 4, 0);
    const double distance = origin.Distance(point);

    gp_Trsf scale;
    scale.SetScale(origin, 2.0);
    const double scaledDistance = origin.Distance(point.Transformed(scale));

    std::cout << "Distance: " << distance << '\n';
    std::cout << "Scaled distance: " << scaledDistance << '\n';

    const bool correct = std::abs(distance - 5.0) < 1e-9
    && std::abs(scaledDistance - 10.0) < 1e-9;

    if (!correct)
        return 1;

    QLabel window(QStringLiteral("Distance: %1\nScaled distance: %2")
                      .arg(distance)
                      .arg(scaledDistance));
    window.setWindowTitle(QStringLiteral("Qt + OCCT Learning"));
    window.resize(360, 140);
    window.show();

    if (app.arguments().contains(QStringLiteral("--smoke-test")))
    {
        QTimer::singleShot(1000, &app, [&app] {
            std::cout << "GUI smoke check finished\n";
            app.quit();
        });
    }

    return app.exec();
}