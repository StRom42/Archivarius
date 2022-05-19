#include "authwidget.h"
#include <QDebug>

AuthWidget::AuthWidget(QWidget *parent) : QWidget(parent), ui_auth(new Ui::auth_form)
{
    ui_auth->setupUi(this);

    connect(ui_auth->LoginButton, SIGNAL(pressed()), this, SLOT(on_LoginButton_clicked()));
    connect(ui_auth->RegistrationLink, SIGNAL(pressed()), this, SLOT(on_RegistrationLink_clicked()));
}

AuthWidget::~AuthWidget()
{
    delete ui_auth;
}

QString AuthWidget::encrypt(QString str)
{
    QByteArray hashed_str = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Algorithm::Sha256);
    QString h = "";
    for (uchar i: hashed_str) h += QString::number(i);
    return h;
}

void AuthWidget::on_LoginButton_clicked()
{
    QString login = ui_auth->LoginInput->text();
    QString password = ui_auth->PasswordInput->text();

    QString hash = encrypt(login+password);
    QFile auth_db(path);
    if (!(auth_db.exists() && auth_db.open(QIODevice::ReadOnly))) return;
    QString line;
    bool access = false;  // access
    while (!auth_db.atEnd()) {
        line = auth_db.readLine();
        line.replace('\n', "");
        if (hash == line){
            access = true;
            break;
        }
    }

    if (access) emit access_allowed(1);
    else QMessageBox::critical(this, "Ошибка входа", "Пользователь не найден");

}

void AuthWidget::on_RegistrationLink_clicked()
{
    emit to_registration(2);
}
