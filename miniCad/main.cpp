#include <QApplication>
#include <QVBoxLayout>

#include "AppContext.h"
#include "CadView.h"
#include "UI/MainWindow.h"

int main(int argc,char *argv[])
{
    QApplication app(argc, argv);

    AppContext context;
    MainWindow window(&context);


    auto* central =new QWidget();
    auto* layout =new QVBoxLayout();
    auto *view = context.m_View;

    layout->addWidget(view,1);
    central->setLayout(layout);
    window.setCentralWidget(central);
    window.resize(1200,800);
    window.show();

    return app.exec();
}
