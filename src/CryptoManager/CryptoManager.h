#ifndef CRYPTOMANAGER_H
#define CRYPTOMANAGER_H

#include <QString>
#include <QByteArray>
#include <stdexcept>

class CryptoManager {
public:
    explicit CryptoManager(const QString& password);

    void setPassword(const QString& password);

    QByteArray encrypt(const QString& plaintext);

    QString decrypt(const QByteArray& data);

private:
    QByteArray generateKey(const QString& password);

    QByteArray key;
};

#endif // CRYPTOMANAGER_H
