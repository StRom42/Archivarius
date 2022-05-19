#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QFile>
#include <QCryptographicHash>
#include <QMessageBox>

#include "ui_auth_form.h"
#include "ui_reg_form.h"

using namespace std;

namespace Ui {
    class auth_form;
}

class AuthWidget : public QWidget
{
    Q_OBJECT;

    Ui::auth_form *ui_auth;
    QString path = "auth.conf";

    QString encrypt(QString);
public:
    explicit AuthWidget(QWidget *parent = nullptr);
    ~AuthWidget();
signals:
    void access_allowed(int);
    void to_registration(int);

private slots:
    void on_LoginButton_clicked();
    void on_RegistrationLink_clicked();
};

#endif // AUTHWIDGET_H
