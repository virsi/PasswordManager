#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QSplitter>
#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include "AddPasswordDialog.h"
#include "../CryptoManager/CryptoManager.h"  // если нужно шифровать


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "Инициализация MainWindow начата";

    if (!dbManager.openDatabase()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
        qDebug() << "Ошибка подключения к базе данных";
    }

    setupUI();
    qDebug() << "setupUI завершен";

    loadPasswords();
    qDebug() << "loadPasswords завершен";

    qDebug() << "Инициализация MainWindow завершена";
}

void MainWindow::setupUI() {
    qDebug() << "Начата настройка UI";

    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);

    // Боковая панель категорий
    categoryList = new QListWidget(this);
    populateCategories();

    // Правая часть
    auto *rightPanel = new QWidget(this);
    auto *rightLayout = new QVBoxLayout(rightPanel);

    // Верхняя панель (поиск + кнопка)
    auto *topBar = new QHBoxLayout();
    searchField = new QLineEdit(this);
    searchField->setPlaceholderText("Поиск...");

    connect(searchField, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged); // добавлено

    addButton = new QPushButton("+ Добавить", this);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);


    topBar->addWidget(searchField);
    topBar->addWidget(addButton);

    // Таблица
    passwordTable = new QTableWidget(this);
    passwordTable->setColumnCount(4);
    QStringList headers = {"ID", "Сервис", "Логин", "Пароль"};
    passwordTable->setHorizontalHeaderLabels(headers);
    passwordTable->setColumnHidden(0, true); // Скрываем столбец с id
    passwordTable->horizontalHeader()->setStretchLastSection(true);
    // passwordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // passwordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Кнопки управления
    editButton = new QPushButton("✏ Редактировать", this);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditClicked);

    deleteButton = new QPushButton("🗑 Удалить", this);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);

    // Собираем всё
    rightLayout->addLayout(topBar);
    rightLayout->addWidget(passwordTable);
    rightLayout->addLayout(buttonLayout);

    mainLayout->addWidget(categoryList);
    mainLayout->addWidget(rightPanel);

    // Устанавливаем stretch: categoryList - меньше, rightPanel (таблица) - больше
    mainLayout->setStretch(0, 1); // categoryList
    mainLayout->setStretch(1, 3); // rightPanel

    setCentralWidget(central); // Убедитесь, что центральный виджет установлен
    qDebug() << "Центральный виджет установлен";

    setWindowTitle("Менеджер Паролей");
    resize(800, 600);
}

void MainWindow::populateCategories() {
    categoryList->addItem("📁 Все");
    categoryList->addItem("🌐 Веб-сайты");
    categoryList->addItem("🔐 Банки");
    categoryList->addItem("💼 Работа");
}

void MainWindow::onAddClicked() {
    AddPasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString service = dialog.getService();
        QString login = dialog.getlogin();
        QString password = dialog.getPassword();

        // Шифруем пароль
        CryptoManager crypto("master"); // В будущем можно получать от пользователя

        QByteArray encryptedPassword;
        try {
            encryptedPassword = crypto.encrypt(password);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", QString("Шифрование не удалось: %1").arg(e.what()));
            return;
        }

        if (!dbManager.addPassword(service, login, encryptedPassword)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить запись в базу данных.");
            return;
        }

        // Обновляем таблицу
        qDebug() << "Обновление таблицы после добавления записи...";
        loadPasswords();
    }
}

void MainWindow::onEditClicked() {
    int row = passwordTable->currentRow();
    if (row >= 0) {
        QMessageBox::information(this, "Редактировать", "Редактирование записи на строке: " + QString::number(row));
    } else {
        QMessageBox::warning(this, "Редактировать", "Выберите запись для редактирования.");
    }
}

void MainWindow::onDeleteClicked() {
    int row = passwordTable->currentRow();
    if (row >= 0) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Удалить", "Удалить выбранную запись?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Получаем id из таблицы (предполагается, что id в первом скрытом столбце)
            int id = passwordTable->item(row, 0)->text().toInt();

            // Удаляем из базы данных
            if (dbManager.deletePassword(id)) {
                // Удаляем из таблицы
                passwordTable->removeRow(row);
                QMessageBox::information(this, "Удалено", "Запись успешно удалена.");
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось удалить запись из базы данных.");
            }
        }
    } else {
        QMessageBox::warning(this, "Удалить", "Выберите запись для удаления.");
    }
}

void MainWindow::loadPasswords() {
    qDebug() << "Загрузка паролей из базы данных...";
    passwordTable->setRowCount(0); // Очистить таблицу перед обновлением

    QVector<PasswordEntry> entries = dbManager.getAllPasswords();
    qDebug() << "Найдено записей:" << entries.size();

    for (int i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        qDebug() << "Извлеченная запись: ID:" << entry.id << "Service:" << entry.service
                 << "Login:" << entry.login << "Password (Base64):" << entry.encryptedPassword;

        qDebug() << "Добавление записи в таблицу:" << entry.service << entry.login;

        // Проверяем, что зашифрованный пароль не пуст
        if (entry.encryptedPassword.isEmpty()) {
            qDebug() << "Ошибка: зашифрованный пароль пуст для записи:" << entry.service;
            continue; // Пропускаем некорректную запись
        }

        // Расшифровка пароля
        CryptoManager crypto("master"); // в будущем заменить master-пароль
        QString decryptedPassword;
        try {
            decryptedPassword = crypto.decrypt(entry.encryptedPassword); // Убрано toUtf8()
        } catch (const std::exception& e) {
            qDebug() << "Ошибка при расшифровке пароля:" << e.what();
            decryptedPassword = "[Ошибка шифрования]";
        } catch (...) {
            qDebug() << "Неизвестная ошибка при расшифровке пароля.";
            decryptedPassword = "[Ошибка шифрования]";
        }

        passwordTable->insertRow(i);
        passwordTable->setItem(i, 0, new QTableWidgetItem(QString::number(entries[i].id))); // ID
        passwordTable->setItem(i, 1, new QTableWidgetItem(entry.service));
        passwordTable->setItem(i, 2, new QTableWidgetItem(entry.login));
        passwordTable->setItem(i, 3, new QTableWidgetItem(decryptedPassword));

        qDebug() << "Строка добавлена: " << i;
    }
    qDebug() << "Пароли успешно загружены в таблицу.";
    onSearchTextChanged(searchField->text()); // фильтруем после загрузки
}

// Новый слот для поиска
void MainWindow::onSearchTextChanged(const QString& text) {
    QString search = text.trimmed();
    for (int row = 0; row < passwordTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 1; col <= 2; ++col) { // 1: сервис, 2: логин
            QTableWidgetItem* item = passwordTable->item(row, col);
            if (item && item->text().contains(search, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        passwordTable->setRowHidden(row, !match && !search.isEmpty());
    }
}
