#include "StatisticsWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

StatisticsWidget::StatisticsWidget(DatabaseManager* db, QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    int totalBooks = db->getTotalBooks();
    int availableBooks = db->getAvailableBooks();
    int lentBooks = db->getLentBooks();
    int totalLendings = db->getTotalLendings();
    double avgDuration = db->getAverageLendingDuration();

    // Kreisdiagramm für Buchstatus
    QtCharts::QPieSeries* series = new QtCharts::QPieSeries();
    series->append("Verfügbar", availableBooks);
    series->append("Ausgeliehen", lentBooks);

    // Optional: Farben und Hervorhebung
    QPieSlice* sliceAvailable = series->slices().at(0);
    sliceAvailable->setBrush(QColor("#4CAF50"));
    sliceAvailable->setLabelVisible(true);

    QPieSlice* sliceLent = series->slices().at(1);
    sliceLent->setBrush(QColor("#FFC107"));
    sliceLent->setLabelVisible(true);

    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Bücherstatus");
    chart->setTheme(QtCharts::QChart::ChartThemeLight);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QtCharts::QChartView* chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(chartView);

    // Weitere Statistiken als Text
    layout->addWidget(new QLabel(QString("Gesamtanzahl Bücher: %1").arg(totalBooks)));
    layout->addWidget(new QLabel(QString("Verfügbare Bücher: %1").arg(availableBooks)));
    layout->addWidget(new QLabel(QString("Ausgeliehene Bücher: %1").arg(lentBooks)));
    layout->addWidget(new QLabel(QString("Anzahl aller Ausleihen: %1").arg(totalLendings)));
    layout->addWidget(new QLabel(QString("Ø Ausleihdauer (Tage): %1").arg(avgDuration, 0, 'f', 2)));
    layout->addStretch();
}