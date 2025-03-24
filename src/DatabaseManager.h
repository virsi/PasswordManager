#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QString>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool openDatabase();
    void closeDatabase();
    bool createTable();
    bool addPassword(const QString &service, const QString &login, const QString &password);
    bool getPasswords();
    bool insertPassword(const QString& service, const QString& username, const QByteArray& encryptedPassword);
    bool updatePassword(int id, const QByteArray& encryptedPassword);
    bool deletePassword(int id);
    QVector<PasswordEntry> getAllPasswords();
    QByteArray getPasswordById(int id);


private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
