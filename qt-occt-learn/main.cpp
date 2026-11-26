#include <QString>
#include <QtGlobal>
#include <iostream>

int main()
{
    const QString text = QStringLiteral("Qt dependency ready");

    std::cout << "Qt version: " << qVersion() << '\n';
    std::cout << text.toStdString() << '\n';
    return 0;
}