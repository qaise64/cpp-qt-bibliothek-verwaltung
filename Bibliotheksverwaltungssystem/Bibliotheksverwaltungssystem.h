#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Bibliotheksverwaltungssystem.h"

class Bibliotheksverwaltungssystem : public QMainWindow
{
    Q_OBJECT

public:
    Bibliotheksverwaltungssystem(QWidget *parent = nullptr);
    ~Bibliotheksverwaltungssystem();

private:
    Ui::BibliotheksverwaltungssystemClass ui;
};
