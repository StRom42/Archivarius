#ifndef REGWIDGET_H
#define REGWIDGET_H

#include <QWidget>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QFile>
#include "ui_reg_form.h"

namespace Ui {
    class reg_form;
}

class RegWidget : public QWidget
{
    Q_OBJECT

    Ui::reg_form *ui_reg;
    QString path = "auth.conf";

    QString encrypt(QString);

public:
    explicit RegWidget(QWidget *parent = nullptr);
    ~RegWidget();

public slots:
    void on_RegistrationButton_clicked();
    void on_BackButton_clicked();
signals:
    void back_to_auth(int);

};

#endif // REGWIDGET_H
