#include <QtGui/QApplication>
#include "loader.h"

int main(int argc, char *argv[]) //The first thing the application launches on startup. Launches the initial browser.
{
    QApplication a(argc, argv);
    a.setApplicationName("Symphony");
    a.setApplicationVersion("0.0.005");
    Loader w;
    Q_UNUSED(w); //Just so it won't give off a warning. All of Loader's functionality is handled upon object construction.
    return a.exec();
}
