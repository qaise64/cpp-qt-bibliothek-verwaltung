#include "Bibliotheksverwaltungssystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Bibliotheksverwaltungssystem w;
    w.show();
    return a.exec();
}
