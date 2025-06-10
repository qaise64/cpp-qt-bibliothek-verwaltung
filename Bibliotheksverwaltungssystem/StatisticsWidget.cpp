#include "StatisticsWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QFont>
#include <QFrame>
#include <QPainter>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

StatisticsWidget::StatisticsWidget(DatabaseManager* db, QWidget* parent)
    : QWidget(parent)
{

    QVBoxLayout* pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);

    // Titel oben, zentriert und groß
    QLabel* titleLabel = new QLabel("Statistiken", this);
    titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    titleLabel->setFont(QFont("Segoe UI", 24, QFont::Bold));
    titleLabel->setStyleSheet("color: #22334a; margin-top: 24px;");
    pageLayout->addWidget(titleLabel);

    // Hauptinhalt
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(40);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Linke Seite: Statistiken als Text, vertikal zentriert
    QVBoxLayout* statsLayout = new QVBoxLayout();
    statsLayout->setSpacing(24);
    statsLayout->setAlignment(Qt::AlignCenter);

    // Statistiken abrufen
    int totalBooks = db->getTotalBooks();
    int availableBooks = db->getAvailableBooks();
    int lentBooks = db->getLentBooks();
    int totalLendings = db->getTotalLendings();
    double avgDuration = db->getAverageLendingDuration();

    // Professionelle Darstellung der Kennzahlen
    auto addStat = [&](const QString& label, const QString& value, const QString& unit = QString()) {
        QHBoxLayout* row = new QHBoxLayout();
        row->setAlignment(Qt::AlignCenter);
        QLabel* valueLabel = new QLabel(value, this);
        valueLabel->setObjectName("statValueLabel");
        valueLabel->setFont(QFont("Segoe UI", 36, QFont::Bold));
        valueLabel->setStyleSheet("color: #457B9D; margin-right: 12px;");
        QLabel* labelLabel = new QLabel(label, this);
        labelLabel->setObjectName("statLabel");
        labelLabel->setFont(QFont("Segoe UI", 15, QFont::Normal));
        labelLabel->setStyleSheet("color: #22334a;");
        row->addWidget(valueLabel, 0, Qt::AlignVCenter);
        row->addWidget(labelLabel, 0, Qt::AlignVCenter);
        if (!unit.isEmpty()) {
            QLabel* unitLabel = new QLabel(unit, this);
            unitLabel->setFont(QFont("Segoe UI", 15, QFont::Normal));
            unitLabel->setStyleSheet("color: #888;");
            row->addWidget(unitLabel, 0, Qt::AlignVCenter);
        }
        statsLayout->addLayout(row);
        };

    addStat("Bücher insgesamt", QString::number(totalBooks));
    addStat("Verfügbare Bücher", QString::number(availableBooks));
    addStat("Ausgeliehene Bücher", QString::number(lentBooks));
    addStat("Anzahl Ausleihen", QString::number(totalLendings));
    addStat("Ø Ausleihdauer", QString::number(avgDuration, 'f', 2), "Tage");

    // Rechte Seite: Kreisdiagramm, vertikal & horizontal zentriert, 2x größer
    QVBoxLayout* chartLayout = new QVBoxLayout();
    chartLayout->setContentsMargins(0, 0, 0, 0);
    chartLayout->setAlignment(Qt::AlignCenter);

    QPieSeries* series = new QPieSeries();
    series->append("Verfügbar", availableBooks);
    series->append("Ausgeliehen", lentBooks);

    QPieSlice* sliceAvailable = series->slices().at(0);
    sliceAvailable->setBrush(QColor("#4CAF50"));
    sliceAvailable->setLabel(QString("Verfügbar\n%1").arg(availableBooks));
    sliceAvailable->setLabelVisible(true);
    sliceAvailable->setLabelColor(Qt::white);
    sliceAvailable->setLabelPosition(QPieSlice::LabelInsideHorizontal);

    QPieSlice* sliceLent = series->slices().at(1);
    sliceLent->setBrush(QColor("#FFC107"));
    sliceLent->setLabel(QString("Ausgeliehen\n%1").arg(lentBooks));
    sliceLent->setLabelVisible(true);
    sliceLent->setLabelColor(Qt::black);
    sliceLent->setLabelPosition(QPieSlice::LabelInsideHorizontal);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Bücherstatus");
    chart->setTheme(QChart::ChartThemeLight);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setBackgroundVisible(false);
    chart->setMargins(QMargins(0, 0, 0, 0));

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(700, 700); // 2x größer als vorher (vorher 350x350)
    chartView->setMaximumSize(900, 900);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartView->setStyleSheet("background: transparent;");

    chartLayout->addWidget(chartView, 0, Qt::AlignCenter);

    // Layouts zusammenfügen
    mainLayout->addLayout(statsLayout, 1);
    mainLayout->addLayout(chartLayout, 1);

    pageLayout->addLayout(mainLayout, 1);

}