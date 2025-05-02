#include "./DatabaseManager.h"
#include "../PasswordManager/PasswordManager.h"
#include <QtSql/QSqlError>
#include <QDir>
#include <QDebug>

DatabaseManager::DatabaseManager() {
    QDir dataDir("../data");
    if (!dataDir.exists()) {
        if (!dataDir.mkpath(".")) {
            qDebug() << "Ошибка: не удалось создать директорию 'data'";
            return;
        }
    }

    QString dbPath = dataDir.filePath("passwords.db");
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    qDebug() << "База данных будет сохранена по пути:" << dbPath;
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
    if (db.isOpen()) {
        return true;
    }

    if (!db.open()) {
        qDebug() << "Error: Unable to open database" << db.lastError();
        return false;
    }
    return true;
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::createTable() {
    qDebug() << "Попытка создать таблицу 'passwords'...";
    QSqlQuery query;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS passwords ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "service TEXT, "
                               "login TEXT, "
                               "password TEXT);");
    if (!success) {
        qDebug() << "Ошибка: не удалось создать таблицу 'passwords':" << query.lastError();
    } else {
        qDebug() << "Таблица 'passwords' успешно создана или уже существует.";
    }
    return success;
}

bool DatabaseManager::addPassword(const QString &service, const QString &login, const QByteArray &password) {
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта.";
        return false;
    }

    QString base64Password = password.toBase64();
    qDebug() << "Добавление пароля в базу данных. Service:" << service << "Login:" << login << "Password (Base64):" << base64Password;

    QSqlQuery query;
    query.prepare("INSERT INTO passwords (service, login, password) VALUES (?, ?, ?)");
    query.addBindValue(service);
    query.addBindValue(login);
    query.addBindValue(base64Password);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось добавить пароль:" << query.lastError().text();
        return false;
    }

    qDebug() << "Пароль успешно добавлен.";
    return true;
}

bool DatabaseManager::getPasswords() {
    QSqlQuery query("SELECT * FROM passwords");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString service = query.value(1).toString();
        QString login = query.value(2).toString();
        QByteArray encryptedPassword = QByteArray::fromBase64(query.value(3).toString().toUtf8());


        qDebug() << "ID:" << id << "Service:" << service << "Login:" << login << "Password:" << encryptedPassword;
    }
    return true;
}

bool DatabaseManager::insertPassword(const QString& service, const QString& login, const QByteArray& encryptedPassword) {
    QByteArray base64Password = encryptedPassword.toBase64();
    qDebug() << "Сохраняемый пароль (Base64):" << base64Password;

    qDebug() << "Попытка вставить запись в таблицу 'passwords'...";
    QSqlQuery query;
    query.prepare("INSERT INTO passwords (service, login, password) VALUES (:service, :login, :password)");

    query.bindValue(":service", service);
    query.bindValue(":login", login);
    query.bindValue(":password", base64Password);

    if (query.boundValues().size() != 3) {
        qDebug() << "Ошибка: количество привязанных параметров не совпадает с ожидаемым.";
        return false;
    }

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении пароля:" << query.lastError().text();
        return false;
    }
    qDebug() << "Запись успешно добавлена в таблицу 'passwords'.";
    return true;
}

bool DatabaseManager::updatePassword(int id, const QByteArray& encryptedPassword) {
    QSqlQuery query;
    query.prepare("UPDATE passwords SET password = :password WHERE id = :id");
    query.bindValue(":password", encryptedPassword.toBase64());
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Ошибка при обновлении пароля:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deletePassword(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM passwords WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Ошибка при удалении пароля:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<PasswordEntry> DatabaseManager::getAllPasswords() {
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта.";
        return {};
    }

    QVector<PasswordEntry> passwords;
    QSqlQuery query("SELECT id, service, login, password FROM passwords");

    while (query.next()) {
        PasswordEntry entry;
        entry.id = query.value(0).toInt();
        entry.service = query.value(1).toString();
        entry.login = query.value(2).toString();
        entry.encryptedPassword = QByteArray::fromBase64(query.value(3).toByteArray());

        qDebug() << "Извлечена запись: ID:" << entry.id << "Service:" << entry.service << "Login:" << entry.login
                 << "Password (Base64):" << query.value(3).toString();
        passwords.append(entry);
    }

    return passwords;
}

QByteArray DatabaseManager::getPasswordById(int id) {
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта.";
        return {};
    }

    QSqlQuery query;
    query.prepare("SELECT password FROM passwords WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка: не удалось получить пароль для ID:" << id << query.lastError().text();
        return {};
    }

    QByteArray base64Password = query.value(0).toByteArray();
    qDebug() << "Получен пароль (Base64):" << base64Password << "для ID:" << id;

    QByteArray decodedPassword = QByteArray::fromBase64(base64Password);
    qDebug() << "Декодированный пароль (hex):" << decodedPassword.toHex();

    return decodedPassword;
}
