#pragma once
#include <QWidget>
#include "DatabaseManager.h"

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