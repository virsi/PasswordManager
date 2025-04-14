#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT // Необходим для работы механизма сигналов и слотов Qt

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTableWidget* tableWidget;
    void setupUI();
    void loadPasswords(); // загрузка паролей из базы
};

#endif // MAINWINDOW_H
