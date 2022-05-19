#include "database.h"
#include <QDebug>

DataBase::DataBase()
{
    if (table_name == "Рождение") this->table_name = "Birth";
    else if (table_name == "Брак") this->table_name = "Marriage";
    else this->table_name = "Death";

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();
    query = new QSqlQuery(db);
}

DataBase::~DataBase()
{
    query->clear();
    delete query;
    db.close();

    QSqlDatabase::removeDatabase(path);
}

QSqlQuery* DataBase::get_whole_table(QString name)
{
    QString table_name = change_name(name);
    query->exec("SELECT * FROM " + table_name);
    return query;
}

QSqlQuery *DataBase::search(QString name,  QVector<QPair<QString, QString>> filter)
{
    QString table_name = change_name(name);
    prepare_query(table_name, SELECT, filter);

    return query;
}

void DataBase::delete_row(QString name,  QVector<QPair<QString, QString>> cols)
{
    QString table_name = change_name(name);
    prepare_query(table_name, DELETE, cols);

    query->clear();
}

void DataBase::update_row(QString name, QVector<QPair<QString, QString>> old_row, QVector<QPair<QString, QString>> new_row)
{
    QString table_name = change_name(name);
    QVector<QPair<QString, QString>> updated_row;
    for (int i = 0; i < old_row.size(); ++i) {
        if (old_row[i].second != new_row[i].second) updated_row.push_back(new_row[i]);
    }
    prepare_query(table_name, UPDATE, old_row, updated_row);
    query->clear();
}

void DataBase::add_row(QString name, QVector<QPair<QString, QString>> data)
{
    QString table_name = change_name(name);
    prepare_query(table_name, INSERT, {}, data);
    query->clear();
}

int DataBase::average(QString name, QString year)
{
    QString table_name = change_name(name);
    QString column_name;
    QRegExp yearexp(year+"\\-\\d\\d\\-\\d\\d");
    if (table_name == "Marriage") column_name = "[дата венчания, вступления в брак], [возраст жениха], [возраст невесты]";
    else column_name = "[дата смерти], [возраст на момент смерти]";

    query->exec("SELECT " + column_name +  " FROM " + table_name);


    int sum = 0;
    int amount = 0;
    QString date;

        while (query->next()) {
            date = query->value(0).toString();
            if (yearexp.exactMatch(date)){
                ++amount;
                sum += query->value(1).toInt();
                if (table_name == "Marriage"){
                    sum += query->value(2).toInt();
                    ++amount;
                }
            }
        }

    amount = (amount == 0) ? 1: amount;
    return sum / amount;
}

int DataBase::year_analize(QString name, QString year)
{
    QString table_name = change_name(name);
    QString column_name = "";

    QRegExp yearexp(year+"\\-\\d\\d\\-\\d\\d");

    if (table_name == "Birth") column_name = "[дата рождения]";
    else if (table_name == "Marriage") column_name = "[дата венчания, вступления в брак]";
    else column_name = "[дата смерти]";

    query->exec("SELECT " + column_name + " FROM " + table_name + ";");

    int amount = 0;
    QString date;

    while (query->next()) {
        date = query->value(0).toString();
        if (yearexp.exactMatch(date)) ++amount;
    }

    return amount;
}

QVector<QPair<QString, int>> DataBase::names_analize(QString year)
{
    QRegExp yearexp(year+"\\-\\d\\d\\-\\d\\d");

    query->exec("SELECT [дата рождения], [имя ребенка] FROM Birth;");
    QVector<QString> names;
    QVector<int> amounts;
    QString date, name;
    int index;
    while (query->next()) {
        date = query->value(0).toString();
        if (yearexp.exactMatch(date)) {
            name = query->value(1).toString();
            if (name == NULL) continue;
            index = names.indexOf(name);
            if (index != -1) amounts[index] += 1;
            else {
                names.push_back(name);
                amounts.push_back(1);
            }
        }
    }
    QVector<QPair<QString, int>> data;
    for (int i = 0; i < names.size(); ++i){
        data.append(qMakePair(names[i], amounts[i]));
    }
    if (data.empty()) return data;
    sort(data.begin(), data.end(), [](QPair<QString, int> a, QPair<QString, int> b){return a.second > b.second;});
    return data.mid(0, 4);

}


QString DataBase::change_name(QString name)
{
    QString res;
    if (name == "Рождение") res = "Birth";
    else if (name == "Брак") res = "Marriage";
    else if (name == "Смерть") res = "Death";

    return res;
}

void DataBase::prepare_query(QString table_name, int command, QVector<QPair<QString, QString>> where, QVector<QPair<QString, QString>> values)
{  
    if (command == UPDATE && values.empty()) return;



    QStringList nums = {"№ описи", "номер записи", "возраст жениха", "возраст невесты", "возраст на момент смерти"};
    QString conditions = "";
    QString settings = "";
    QString adding = "";

    if (command != INSERT) {
        for (int i = 0; i < where.size(); ++i) {
            if (i == where.size() - 1) conditions += "["+ where[i].first +"] =:a" + QString::number(i);
            else conditions += "["+ where[i].first +"] =:a" + QString::number(i) + " AND ";
        }
    }
    else {
        for (int i = 0; i < values.size(); ++i) {
            if (i == values.size() - 1) {
                conditions += "["+ values[i].first +"]";
                adding += ":c" + QString::number(i);
            }
            else {
                conditions += "["+ values[i].first +"], ";
                adding += ":c" + QString::number(i) + ", ";
            }
        }
    }

    if (command == UPDATE) {
        for (int i = 0; i < values.size(); ++i){
            if (i == values.size() - 1) settings += "["+ values[i].first +"] =:b" + QString::number(i);
            else settings += "["+ values[i].first +"] =:b" + QString::number(i) + ", ";
        }
    }

    if (command == SELECT) {
        query->prepare("SELECT * FROM " + table_name + " WHERE " + conditions + ";");
    }
    else if (command == DELETE) {
        query->prepare("DELETE FROM " + table_name + " WHERE " + conditions + ";");
    }
    else if (command == UPDATE) {
        query->prepare("UPDATE " + table_name + " SET " + settings +  " WHERE " + conditions + ";");
    }
    else if (command == INSERT) {
        query->prepare("INSERT INTO " + table_name + " (" + conditions + ") VALUES (" + adding + ");");
    }

    if (command != INSERT){
        for (int i = 0; i < where.size(); ++i){
            if (find(nums.begin(), nums.end(), where[i].first) != nums.end()) {
                query->bindValue(":a"+QString::number(i), where[i].second.toInt());
            }
            else {
                query->bindValue(":a"+QString::number(i), where[i].second);
            }
        }
    }
    else  {
        for (int i = 0; i < values.size(); ++i){
            if (find(nums.begin(), nums.end(), values[i].first) != nums.end()) {
                query->bindValue(":c"+QString::number(i), values[i].second.toInt());
            }
            else {
                query->bindValue(":c"+QString::number(i), values[i].second);
            }
        }
    }

    if (command == UPDATE){
        for (int i = 0; i < values.size(); ++i){
                if (find(nums.begin(), nums.end(), values[i].first) != nums.end()) {
                    query->bindValue(":b"+QString::number(i), values[i].second.toInt());
                }
                else {
                    query->bindValue(":b"+QString::number(i), values[i].second);
                }
        }
    }

    query->exec();
}

