#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.load();
    w.show();
    if (w.showIntro()) {
        w.showInstructions();
    }

    return a.exec();
}
