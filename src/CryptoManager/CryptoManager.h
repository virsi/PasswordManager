#ifndef CRYPTOMANAGER_H
#define CRYPTOMANAGER_H

#include <QString>
#include <QByteArray>

class CryptoManager {
public:
    CryptoManager(const QString& password);

    QByteArray encrypt(const QString& plaintext);
    QString decrypt(const QByteArray& ciphertext);

private:
    QByteArray generateKey(const QString& password);

    QByteArray key;
};

#endif // CRYPTOMANAGER_H
