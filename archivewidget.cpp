#include "archivewidget.h"
#include <QtDebug>

ArchiveWidget::ArchiveWidget(QWidget *parent) : QWidget(parent), ui_archive(new Ui::archive_form)
{
    ui_archive->setupUi(this);
    this->setFixedSize(parent->size());
    ui_archive->filter_scroll->setFixedHeight(parent->height() * 0.3);

    DataBase db();

    filter_grid = new QGridLayout(ui_archive->filter_area);

    connect(ui_archive->birth_o, SIGNAL(pressed()), this, SLOT(show_table()));
    connect(ui_archive->marriage_o, SIGNAL(pressed()), this, SLOT(show_table()));
    connect(ui_archive->death_o, SIGNAL(pressed()), this, SLOT(show_table()));
    connect(ui_archive->search_button, SIGNAL(pressed()), this, SLOT(search()));

    connect(ui_archive->birth_e, SIGNAL(pressed()), this, SLOT(show_table()));
    connect(ui_archive->marriage_e, SIGNAL(pressed()), this, SLOT(show_table()));
    connect(ui_archive->death_e, SIGNAL(pressed()), this, SLOT(show_table()));

    connect(ui_archive->add_button, SIGNAL(pressed()), this, SLOT(add_to_archive()));
    connect(ui_archive->edit_button, SIGNAL(pressed()), this, SLOT(edit_archive()));
    connect(ui_archive->delete_button, SIGNAL(pressed()), this, SLOT(delete_in_archive()));
    connect(ui_archive->print_button, SIGNAL(pressed()), this, SLOT(print_archive()));

    connect(ui_archive->analysis_button, SIGNAL(pressed()), this, SLOT(year_analize()));

    connect(ui_archive->tabs, SIGNAL(tabBarClicked(int)), this, SLOT(analizing(int)));
    ui_archive->birth_o->pressed();
}

ArchiveWidget::~ArchiveWidget()
{
    delete_filter();
    delete average_year_plot;
    delete death_plot;
    delete marriage_plot;
    delete names_plot;
    delete filter_grid;
    delete ui_archive;
}

QVector<QPair<QString, QString>> ArchiveWidget::get_row(int row)
{
    QRegExp re_date("\\d{1,4}\\-((0[1-9])|(1[0-2]))\\-((0[1-9])|([12][0-9])|(3[01]))");
    int cols_amount = ui_archive->table_edited->columnCount();
    QVector<QPair<QString, QString>> cols;
    QStringList num_fields = {"№ описи", "номер записи", "возраст жениха", "возраст невесты", "возраст на момент смерти"};
    QStringList date_fields = {"дата рождения", "дата крещения","дата венчания, вступления в брак","дата смерти", "дата погребения"};
    QString val, field, date;
    for (int i = 0; i < cols_amount; ++i){
        val = ui_archive->table_edited->item(row, i)->text();
        field = ui_archive->table_edited->horizontalHeaderItem(i)->text();
        if (val == "") {
            if (std::find(num_fields.begin(), num_fields.end(), field) != num_fields.end()) val = "0";
            else val = "NULL";
        }
        if (std::find(date_fields.begin(), date_fields.end(), field) != date_fields.end()) {
            if (!re_date.exactMatch(val)) val = "NULL";
        }
        cols.append(qMakePair(field, val));
    }
    return cols;
}

void ArchiveWidget::allow_editing(int row)
{
    int cols_amount = ui_archive->table_edited->columnCount();
    for (int i = 0; i < cols_amount; ++i){
        ui_archive->table_edited->item(row, i)->setFlags(ui_archive->table_edited->item(row, i)->flags() | Qt::ItemIsEditable);
    }
}

void ArchiveWidget::show_table()
{
    QRadioButton * button = dynamic_cast<QRadioButton*>(this->sender());
    table_name = button->text();



    if (ui_archive->tabs->currentIndex() == 0){
        QStringList *filter_kind;
        if (table_name == "Рождение") filter_kind = &birth_filter;
        else if (table_name == "Брак") filter_kind = &marriage_filter;
        else filter_kind = &death_filter;

        delete_filter();
        QLabel *filter_label;
        QLineEdit *filter_input;
        for (int i = 0; i < filter_kind->size(); ++i)
        {
            filter_label = new QLabel(filter_kind->at(i));
            filter_input = new QLineEdit();
            filter_grid->addWidget(filter_label, i, 0);
            filter_grid->addWidget(filter_input, i, 1);
            filter_inputs.push_back(filter_input);
            filter_labels.push_back(filter_label);
        }
        ui_archive->filter_scroll->setWidget(ui_archive->filter_area);
    }

    QSqlQuery *query = db.get_whole_table(table_name);
    render_table(query);
}

void ArchiveWidget::render_table(QSqlQuery * query)
{
    int rows = 0;
    if(query->last())
    {
        rows =  query->at() + 1;
        query->first();
        query->previous();
    }

    int max;
    QStringList *table_headers;
    QString table_type = table_name;
    if (table_type == "Брак") {
        max = 18;
        table_headers = &marriage_filter;
    }
    else {
        max = 13;
        if (table_type == "Рождение") table_headers = &birth_filter;
        else if (table_type == "Смерть") table_headers = &death_filter;
    }


    if (ui_archive->tabs->currentIndex() == 0){
        ui_archive->table_observe->clear();
        ui_archive->table_observe->setColumnCount(max);
        ui_archive->table_observe->setRowCount(rows);
        ui_archive->table_observe->setHorizontalHeaderLabels(*table_headers);
        QString val;
        int cells_amount = 0;
        while (query->next()){
            for (int i = 0; i < max; ++i){
                val = query->value(i).toString();
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(val);
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);

                ui_archive->table_observe->setItem(cells_amount / max, i, item);

                ui_archive->table_observe->resizeRowsToContents();
                ui_archive->table_observe->resizeColumnsToContents();

                ++cells_amount;
            }
        }
    }
    else {
        ui_archive->table_edited->clear();
        ui_archive->table_edited->setColumnCount(max);
        ui_archive->table_edited->setRowCount(rows);
        ui_archive->table_edited->setHorizontalHeaderLabels(*table_headers);
        QString val;
        int cells_amount = 0;
        while (query->next()){
            for (int i = 0; i < max; ++i){
                val = query->value(i).toString();
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(val);
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);

                ui_archive->table_edited->setItem(cells_amount / max, i, item);

                ui_archive->table_edited->resizeRowsToContents();
                ui_archive->table_edited->resizeColumnsToContents();

                ++cells_amount;
            }
        }
    }
    query->clear();
}

void ArchiveWidget::delete_filter()
{
    while (!filter_inputs.empty()) {
        delete filter_inputs.back();
        filter_inputs.pop_back();
    }
    while (!filter_labels.empty()) {
        delete filter_labels.back();
        filter_labels.pop_back();
    }
}

void ArchiveWidget::paint_plot(QVector<QPair<QString, int>> data, int plot_type)
{
    if (plot_type == YEAR_PLOT) ui_archive->year_chart->setChart(new Plot(data));
    else if (plot_type == DEATH_PLOT) ui_archive->death_chart->setChart(new Plot(data));
    else if (plot_type == MARRIAGE_PLOT) ui_archive->marriage_chart->setChart(new Plot(data));
    else if (plot_type == NAMES_PLOT) ui_archive->names_chart->setChart(new Plot(data));
//    plot_view->setRenderHint(QPainter::Antialiasing);

}

QString ArchiveWidget::table_to_html(QModelIndexList rows)
{
    bool not_selected = rows.isEmpty();
    int rows_amount = not_selected ? ui_archive->table_edited->rowCount() : rows.size(), row_index;

    QVector<QPair<QString, QString>> row_vals;
    QString html = "<html><body><table border=1 cellpadding=5>", row = "";

    for (int i = 0; i < rows_amount; ++i){
        row_index = not_selected ? i : rows[i].row();
        row_vals = get_row(row_index);
        row = "";

        html += "<tr>";
        for (auto header: row_vals){
            if (i == 0) html += "<th>" + header.first + "</th>";
            row += "<td>" + header.second + "</td>";
        }
        html += "</tr>";
        html += "<tr>" + row + "</tr>";
    }

    html += "</table></body></html>";
    return html;
}

void ArchiveWidget::search()
{
    QString table_kind = table_name;
    if (table_kind == "") {
        QMessageBox::critical(this, "Ошибка поиска", "Сначала откройте нужную таблицу");
        return;
    }
    QVector<QPair<QString, QString>> filter;
    int rows = filter_inputs.size();
    QString val, field;
    for (int i = 0; i < rows; ++i){
        val = filter_inputs[i]->text();
        if (val == "") continue;
        field = filter_labels[i]->text();
        filter.append(qMakePair(field, val));
    }

    QSqlQuery * query;
    if (filter.empty()) query = db.get_whole_table(table_kind);
    else query = db.search(table_kind, filter);
    render_table(query);
    query->clear();
}

void ArchiveWidget::add_to_archive()
{
    if (ui_archive->add_button->text() == "Готово") {
        QVector<QPair<QString, QString>> added_row = get_row(ui_archive->table_edited->rowCount() - 1);
        db.add_row(table_name, added_row);

        QSqlQuery *query = db.get_whole_table(table_name);
        render_table(query);

        ui_archive->add_button->setText("Добавить");
    }
    else {
        if (table_name == "") {
            QMessageBox::critical(this, "Ошибка добавления", "Сначала откройте нужную таблицу");
            return;
        }
        if (ui_archive->edit_button->text() == "Готово") {
            QMessageBox::critical(this, "Ошибка добавления", "Сначала завершите редактирование");
            return;
        }


        int added_row_index = ui_archive->table_edited->rowCount();
        ui_archive->table_edited->setRowCount(added_row_index+1);
        for (int i = 0; i < ui_archive->table_edited->columnCount(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText("");
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui_archive->table_edited->setItem(added_row_index, i, item);
        }
        ui_archive->add_button->setText("Готово");
    }
}

void ArchiveWidget::edit_archive()
{
    if (ui_archive->edit_button->text() == "Готово") {
        QVector<QPair<QString, QString>> new_row = get_row(old_row_number);
        db.update_row(table_name, old_row, new_row);
        ui_archive->edit_button->setText("Изменить");

        QSqlQuery *query = db.get_whole_table(table_name);
        render_table(query);
    }
    else {
        int selected_row = ui_archive->table_edited->currentRow();
        if (selected_row == -1) {
            QMessageBox::critical(this, "Ошибка редактирования", "Сначала выделите строку");
            return;
        }
        if (ui_archive->add_button->text() == "Готово"){
            QMessageBox::critical(this, "Ошибка редактирования", "Сначала закончите добавление");
            return;
        }
        old_row = get_row(selected_row);
        old_row_number = selected_row;

        allow_editing(old_row_number);
        ui_archive->edit_button->setText("Готово");
    }
}

void ArchiveWidget::delete_in_archive()
{
    int row_to_delete = ui_archive->table_edited->currentRow();
    if (row_to_delete == -1) {
        QMessageBox::critical(this, "Ошибка удаления", "Сначала выделите ряд");
        return;
    }
    if (ui_archive->add_button->text() == "Готово"){
        QMessageBox::critical(this, "Ошибка удаления", "Сначала закончите добавление");
        return;
    }
    if (ui_archive->edit_button->text() == "Готово"){
        QMessageBox::critical(this, "Ошибка удаления", "Сначала закончите редактирование");
        return;
    }
    QVector<QPair<QString, QString>> cols = get_row(row_to_delete);

    db.delete_row(table_name, cols);

    QSqlQuery *query = db.get_whole_table(table_name);
    render_table(query);
}

void ArchiveWidget::print_archive()
{
    QPrinter printer;

    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(5, 5, 5, 5), QPageLayout::Millimeter);
    printer.setFullPage(true);
    printer.setOutputFormat(QPrinter::NativeFormat);

    QPrintPreviewDialog dialog(&printer, this);
    dialog.setWindowTitle("Печать таблицы");
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(paint_doc(QPrinter*)));

    dialog.exec();

}

void ArchiveWidget::paint_doc(QPrinter *printer)
{
    QModelIndexList selection = ui_archive->table_edited->selectionModel()->selectedRows();
    QString html = table_to_html(selection);
    QTextDocument document;
    QFont DocFont("Verdana");
    DocFont.setPixelSize(12);
    document.setDefaultFont(DocFont);
    document.setHtml(html);
    document.print(printer);
}

void ArchiveWidget::analizing(int tab_index)
{
    if (tab_index == 2) {

        int marriage_av = db.average("Брак");
        int death_av = db.average("Смерть");
        QVector<QPair<QString, int>> data = db.names_analize();
        QString report = "Средний возраст брака: " + QString::number(marriage_av) +
                         "\nСредний возраст смерти: " + QString::number(death_av);
        if (! data.empty()) report += "\nСамое популярное имя ребенка: " + data[0].first;
        ui_archive->average_label->setText(report);
    }
    if (tab_index == 1) {
        ui_archive->tabs->setCurrentIndex(1);
        ui_archive->birth_e->pressed();
    }
}

void ArchiveWidget::year_analize()
{
    QRegExp re_year("\\d{1,4}");
    QString year = ui_archive->year_line->text();
    int start_year = year.toInt();
    if (!re_year.exactMatch(year)) {
        QMessageBox::critical(this, "Ошибка анализа", "Неправильный формат года");
        return;
    }

    QVector<QPair<QString, int>> year_data, death_data, marriage_data, names_data;

    int births = db.year_analize("Рождение", year);
    int marriages = db.year_analize("Брак", year);
    int deaths = db.year_analize("Смерть", year);
    year_data.push_back(qMakePair(QString("Рождения"), births));
    year_data.push_back(qMakePair(QString("Браки"), marriages));
    year_data.push_back(qMakePair(QString("Смерти"), deaths));
    paint_plot(year_data, YEAR_PLOT);

    QString current_year;
    int amount = 0;
    for (int i = 0; i < 8; ++i) {
        current_year = QString::number(start_year + i);
        amount = db.year_analize("Смерть", current_year);
        death_data.push_back(qMakePair(current_year, amount));
        amount = db.year_analize("Брак", current_year);
        marriage_data.push_back(qMakePair(current_year, amount));
    }
    paint_plot(death_data, DEATH_PLOT);
    paint_plot(marriage_data, MARRIAGE_PLOT);

    names_data = db.names_analize(year);
    paint_plot(names_data, NAMES_PLOT);

    int marriage_av = db.average("Брак", year);
    int death_av = db.average("Смерть", year);
    QString report = "Средний возраст брака: " + QString::number(marriage_av) +
                     "\nСредний возраст смерти: " + QString::number(death_av);
    if (! names_data.empty()) report += "\nСамое популярное имя ребенка: " + names_data[0].first;
    ui_archive->year_average_label->setText(report);
}
