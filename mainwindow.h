#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "authwidget.h"
#include "regwidget.h"
#include "archivewidget.h"
#include <iostream>

class MainWindow : public QMainWindow
{
    AuthWidget *auth_widget;
    RegWidget *reg_widget;
    ArchiveWidget *archivarius;
public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();
public slots:
    void change_widget(int);
};

#endif // MAINWINDOW_H
