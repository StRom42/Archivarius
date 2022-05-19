#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(380, 530);
    this->setStyleSheet("QMainWindow{background:#e3f3ff}");
    change_widget(0);
}

MainWindow::~MainWindow()
{
    delete archivarius;
    delete auth_widget;
    delete reg_widget;
}

void MainWindow::change_widget(int window)
{
    if (window == 0) {
        auth_widget = new AuthWidget();
        connect(auth_widget, &AuthWidget::access_allowed, this, &MainWindow::change_widget);
        connect(auth_widget, &AuthWidget::to_registration, this, &MainWindow::change_widget);
        this->setCentralWidget(auth_widget);
    }
    else if (window == 1) {
        delete auth_widget;
        QSize screen_size = QGuiApplication::screens()[0]->availableSize();
        this->setFixedSize(screen_size);
        this->move(0, 0);
//        this->showFullScreen();
        archivarius = new ArchiveWidget(this);
        this->setCentralWidget(archivarius);
    }
    else if (window == 2){
        reg_widget = new RegWidget();
        connect(reg_widget, &RegWidget::back_to_auth, this, &MainWindow::change_widget);
        this->setCentralWidget(reg_widget);
        delete auth_widget;
    }

}
