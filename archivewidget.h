#ifndef ARCHIVEWIDGET_H
#define ARCHIVEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QSqlQuery>
#include <QStringList>
#include <QMap>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QPainter>
#include <QTextDocument>
#include <QPrintPreviewDialog>
#include <QtCharts>
#include <Plots.cpp>

#include <database.h>
#include "ui_archive_form.h"

#define YEAR_PLOT 0
#define DEATH_PLOT 1
#define MARRIAGE_PLOT 2
#define NAMES_PLOT 3

namespace Ui {
    class archive_form;
}

class ArchiveWidget : public QWidget
{
    Q_OBJECT

    void render_table(QSqlQuery *);
    void delete_filter();
    void sync_table_names(QString);
    void paint_plot(QVector<QPair<QString, int>>, int);

    QString table_to_html(QModelIndexList);

    QString date_format(QString);
    QVector<QPair<QString, QString>> get_row(int);
    void allow_editing(int);

private:
    Ui::archive_form *ui_archive;
    QGridLayout *filter_grid;
    Plot *average_year_plot, *death_plot, *marriage_plot, *names_plot;
    QString table_name = "";

    QVector<QLineEdit*> filter_inputs;
    QVector<QLabel*> filter_labels;
    QVector<QPair<QString, QString>> old_row;
    int old_row_number;
    DataBase db;

    QStringList birth_filter = {"№ описи", "№ дела", "№ листа", "номер записи",
                                "дата рождения", "дата крещения", "наименование населенного пункта, в котором родился ребенок",
                                "имя ребенка", "ФИО отца ребенка, его социальное положение", "имя и отчество матери ребенка",
                                "сведения о крестных родителях", "сведения о священнике, который проводил обряд", "вероисповедание"};
    QStringList marriage_filter = {"№ описи", "№ дела", "№ листа", "номер записи", "дата венчания, вступления в брак", "населенный пункт, в котором проживает жених",
                                   "ФИО жениха", "возраст жениха", "социальное положение жениха", "ФИО невесты", "возраст невесты", "социальное положение невесты",
                                   "населенный пункт, в котором проживает невеста", "сведения о вступлении в брак впервые жениха и невесты",
                                   "являются ли жених или невеста вдовцом (вдовой)", "поручители (венчальные родители)", "сведения о священнике, который проводил обряд",
                                   "Вероисповедание"};
    QStringList death_filter = {"№ описи", "№ дела", "№ листа", "номер записи", "дата смерти", "дата погребения", "населенный пункт, в котором жил человек",
                                "ФИО умершего", "социальное положение умершего", "возраст на момент смерти", "причина смерти", "сведения о священнике, который проводил обряд",
                                "сведения о месте погребения"};

    QRegExp re_month, re_day, re_num, re_name;
public:
    explicit ArchiveWidget(QWidget *parent = nullptr);
    ~ArchiveWidget();

public slots:
    void show_table();
    void search();
    void add_to_archive();
    void edit_archive();
    void delete_in_archive();
    void print_archive();
    void paint_doc(QPrinter *);
    void year_analize();
    void analizing(int);


};

#endif // ARCHIVEWIDGET_H
