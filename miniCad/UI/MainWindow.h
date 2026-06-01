//
// Created by ZQD on 2026/5/31.
//

#pragma once
#include <QMainWindow>


class AppContext;

class MainWindow : public QMainWindow {
public:
    MainWindow(AppContext *context);

private:
    AppContext *m_Context;
};


