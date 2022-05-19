#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QString>
#include <QMap>

#define SELECT 0
#define DELETE 1
#define UPDATE 2
#define INSERT 3

using namespace std;

class DataBase
{
    QString change_name(QString);
    void prepare_query(QString, int,  QVector<QPair<QString, QString>> where = {},  QVector<QPair<QString, QString>> values = {});

    QString path = "Archivarius.db";
    QSqlDatabase db;
    QSqlQuery *query;
    QString table_name;
public:
    explicit DataBase();
    ~DataBase();

    QSqlQuery* get_whole_table(QString);
    QSqlQuery* search(QString,  QVector<QPair<QString, QString>>);
    void delete_row(QString,  QVector<QPair<QString, QString>>);
    void update_row(QString,  QVector<QPair<QString, QString>>,  QVector<QPair<QString, QString>>);
    void add_row(QString,  QVector<QPair<QString, QString>>);
    int average(QString, QString year = "\\d{1,4}");
    int year_analize(QString,QString);
    QVector<QPair<QString, int>> names_analize(QString year = "\\d{1,4}");
};

#endif // DATABASE_H
