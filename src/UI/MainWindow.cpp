#include "MainWindow.h"

#include "MainWindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QHeaderView>
#include "../DatabaseManager/DatabaseManager.h"
#include "../CryptoManager/CryptoManager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    loadPasswords();
}

MainWindow::~MainWindow() {
    delete tableWidget;
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout();

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels({ "Сервис", "Имя пользователя", "Пароль" });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tableWidget);

    // Кнопки (добавим обработчики позже)
    QPushButton* addButton = new QPushButton("Добавить");
    QPushButton* editButton = new QPushButton("Изменить");
    QPushButton* deleteButton = new QPushButton("Удалить");

    layout->addWidget(addButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::loadPasswords() {
    DatabaseManager db;

    // Открываем базу данных
    if (!db.openDatabase()) {
        qDebug() << "Не удалось открыть базу данных.";
        return;
    }

    QVector<PasswordEntry> entries = db.getAllPasswords();

    tableWidget->setRowCount(entries.size());
    for (int i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        tableWidget->setItem(i, 0, new QTableWidgetItem(e.service));
        tableWidget->setItem(i, 1, new QTableWidgetItem(e.username));
        tableWidget->setItem(i, 2, new QTableWidgetItem("[скрыто]")); // показываем скрытую строку
    }

    // Закрываем базу данных
    db.closeDatabase();
}
