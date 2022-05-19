#include "regwidget.h"

RegWidget::RegWidget(QWidget *parent) : QWidget(parent), ui_reg(new Ui::reg_form)
{
    ui_reg->setupUi(this);

    connect(ui_reg->RegistrationButton, SIGNAL(pressed()), this, SLOT(on_RegistrationButton_clicked()));
    connect(ui_reg->BackButton, SIGNAL(pressed()), this, SLOT(on_BackButton_clicked()));
}
RegWidget::~RegWidget()
{
    delete ui_reg;
}

QString RegWidget::encrypt(QString str)
{
    QByteArray hashed_str = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Algorithm::Sha256);
    QString h = "";
    for (uchar i: hashed_str) h += QString::number(i);
    return h;
}

void RegWidget::on_RegistrationButton_clicked()
{
    QString login = ui_reg->LoginInput->text();
    QString password = ui_reg->PasswordInput->text();

    if (login == "" || password == ""){
        QMessageBox::critical(this, "Ошибка регистрации", "Неправильный формат логина или пароля");
        return;
    }

    QString hash = encrypt(login+password);
    QFile auth_db(path);
    if (!(auth_db.exists() && auth_db.open(QIODevice::ReadWrite))) return;
    QString line;
    bool has_account = false;
    while (!auth_db.atEnd()) {
        line = auth_db.readLine();
        line.replace('\n', "");
        if (hash == line){
            has_account = true;
            break;
        }
    }
    if (!has_account) {
        QString content = auth_db.readAll();
        content += hash + "\n";
        auth_db.write(content.toUtf8());
        QMessageBox::information(this, "Успешная регистрация", "Пользователь успешно зарегистрирован");
        on_BackButton_clicked();
    }
    else QMessageBox::critical(this, "Ошибка регистрации", "Пользователь уже существует");
}

void RegWidget::on_BackButton_clicked()
{
    emit back_to_auth(0);
}
