#include <QtCharts>
#include <QVector>
#include <QPair>

using namespace QtCharts;

class Plot : public QChart
{
    QBarSet  *bar;
    QBarSeries *series;
    QValueAxis *yaxis;
    QBarCategoryAxis *xaxis;

public:
    Plot(QVector<QPair<QString, int>> data) : QChart()
    {
        series = new QBarSeries();
        QStringList categories;
        int max = 0;
        bar = new QBarSet("");
        for (int i = 0; i < data.size(); ++i){
            categories << data[i].first;
            *bar << data[i].second;
            if (data[i].second > max) max = data[i].second;
        }

        series->append(bar);
        this->addSeries(series);

        xaxis = new QBarCategoryAxis();
        xaxis->append(categories);
        this->addAxis(xaxis, Qt::AlignBottom);
        series->attachAxis(xaxis);

        yaxis = new QValueAxis();
        yaxis->setTickType(QValueAxis::TicksDynamic);
        yaxis->setTickInterval(1);
        yaxis->setRange(0, max+2);
        yaxis->setLabelFormat("%d");
        this->addAxis(yaxis, Qt::AlignLeft);
        series->attachAxis(yaxis);

        this->legend()->hide();

    }
    ~Plot()
    {
        delete series;
        delete bar;
        delete yaxis;
        delete xaxis;
    }
};
