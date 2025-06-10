#pragma once
#include <QWidget>
#include "DatabaseManager.h"
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

QT_BEGIN_NAMESPACE
namespace QtCharts {
    class QChartView;
    class QPieSeries;
}
QT_END_NAMESPACE

class StatisticsWidget : public QWidget {
    Q_OBJECT
public:
    StatisticsWidget(DatabaseManager* db, QWidget* parent = nullptr);
};