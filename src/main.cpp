#include <QApplication>
#include <QDebug>
#include "CryptoManager/CryptoManager.h"
#include <QMainWindow>
#include "DatabaseManager/DatabaseManager.h"
#include <QApplication>
#include "UI/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}

