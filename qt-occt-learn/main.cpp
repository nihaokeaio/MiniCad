#include <QString>
#include <QtGlobal>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <cmath>
#include <iostream>

int main()
{
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

    return std::abs(distance - 5.0) < 1e-9
        && std::abs(scaledDistance - 10.0) < 1e-9 ? 0 : 1;
}