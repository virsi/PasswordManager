#include "DatabaseManager.h"
#include "PasswordManager.h"
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("passwords.db");
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
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
    QSqlQuery query;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS passwords ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "service TEXT, "
                               "login TEXT, "
                               "password TEXT);");
    if (!success) {
        qDebug() << "Error: Unable to create table" << query.lastError();
    }
    return success;
}

bool DatabaseManager::addPassword(const QString &service, const QString &login, const QString &password) {
    QSqlQuery query;
    query.prepare("INSERT INTO passwords (service, login, password) VALUES (?, ?, ?)");
    query.addBindValue(service);
    query.addBindValue(login);
    query.addBindValue(password);

    if (!query.exec()) {
        qDebug() << "Error: Unable to add password" << query.lastError();
        return false;
    }
    return true;
}

bool DatabaseManager::getPasswords() {
    QSqlQuery query("SELECT * FROM passwords");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString service = query.value(1).toString();
        QString login = query.value(2).toString();
        QString password = query.value(3).toString();

        qDebug() << "ID:" << id << "Service:" << service << "Login:" << login << "Password:" << password;
    }
    return true;
}

bool DatabaseManager::insertPassword(const QString& service, const QString& username, const QByteArray& encryptedPassword) {
    QSqlQuery query;
    query.prepare("INSERT INTO passwords (service, username, password) VALUES (:service, :username, :password)");
    query.bindValue(":service", service);
    query.bindValue(":username", username);
    query.bindValue(":password", encryptedPassword);

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении пароля:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updatePassword(int id, const QByteArray& encryptedPassword) {
    QSqlQuery query;
    query.prepare("UPDATE passwords SET password = :password WHERE id = :id");
    query.bindValue(":password", encryptedPassword);
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
    QVector<PasswordEntry> passwords;
    QSqlQuery query("SELECT id, service, username, password FROM passwords");

    while (query.next()) {
        PasswordEntry entry;
        entry.id = query.value(0).toInt();
        entry.service = query.value(1).toString();
        entry.username = query.value(2).toString();
        entry.encryptedPassword = query.value(3).toByteArray();
        passwords.append(entry);
    }

    return passwords;
}

QByteArray DatabaseManager::getPasswordById(int id) {
    QSqlQuery query;
    query.prepare("SELECT password FROM passwords WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка при получении пароля:" << query.lastError().text();
        return QByteArray();
    }

    return query.value(0).toByteArray();
}
