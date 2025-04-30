#include <QApplication>
#include <QDebug>
#include "CryptoManager/CryptoManager.h"
#include <QMainWindow>
#include "DatabaseManager/DatabaseManager.h"
#include <QApplication>
#include "UI/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qDebug() << "Приложение запущено";

    MainWindow w;
    qDebug() << "Окно MainWindow создано";

    if (!w.centralWidget()) {
        qDebug() << "Ошибка: центральный виджет не установлен!";
        return -1;
    }

    w.show();
    qDebug() << "Окно MainWindow отображено";

    return app.exec();
}
