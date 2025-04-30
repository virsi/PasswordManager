#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include "../DatabaseManager/DatabaseManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QTableWidget *passwordTable;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QLineEdit *searchField;
    QListWidget *categoryList;

    DatabaseManager dbManager;

    void loadPasswords();
    void setupUI();
    void populateCategories();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();

};

#endif // MAINWINDOW_H
