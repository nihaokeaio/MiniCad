#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QToolBar>

#include "View/CadView.h"

int main(int argc,char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;

    auto* central =new QWidget();
    auto* layout =new QVBoxLayout();
    auto* view =new CadView();

    layout->addWidget(view,1);
    central->setLayout(layout);
    window.setCentralWidget(central);
    auto toolbar= window.addToolBar("Main Toolbar");
    window.resize(1200,800);
    window.show();

    auto action =  toolbar->addAction("Print Selection");
    QObject::connect(action,&QAction::triggered, view,&CadView::PrintSelection);

    return app.exec();
}