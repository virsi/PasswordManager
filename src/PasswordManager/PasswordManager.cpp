#include "./PasswordManager.h"
#include <QDebug>

PasswordManager::PasswordManager(DatabaseManager& db, CryptoManager& crypto)
    : db(db), crypto(crypto) {}

bool PasswordManager::addPassword(const QString& service, const QString& username, const QString& password) {
    QByteArray encryptedPassword = crypto.encrypt(password);
    return db.insertPassword(service, username, encryptedPassword);
}

bool PasswordManager::updatePassword(int id, const QString& newPassword) {
    QByteArray encryptedPassword = crypto.encrypt(newPassword);
    return db.updatePassword(id, encryptedPassword);
}

bool PasswordManager::deletePassword(int id) {
    return db.deletePassword(id);
}

QVector<PasswordEntry> PasswordManager::getAllPasswords() {
    return db.getAllPasswords();
}

QString PasswordManager::getDecryptedPassword(int id) {
    QByteArray encryptedPassword = db.getPasswordById(id);
    if (encryptedPassword.isEmpty()) {
        return QString();
    }
    return crypto.decrypt(encryptedPassword);
}
