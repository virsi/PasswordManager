#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QString>
#include <QVector>
#include "../DatabaseManager/DatabaseManager.h"
#include "../CryptoManager/CryptoManager.h"

class PasswordManager {
public:
    PasswordManager(DatabaseManager& db, CryptoManager& crypto);

    bool addPassword(const QString& service, const QString& username, const QString& password);
    bool updatePassword(int id, const QString& newPassword);
    bool deletePassword(int id);

    QVector<PasswordEntry> getAllPasswords();
    QString getDecryptedPassword(int id);

private:
    DatabaseManager& db;
    CryptoManager& crypto;
};

#endif // PASSWORDMANAGER_H
