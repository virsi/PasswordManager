#include <QCoreApplication>
#include <QDebug>
#include "CryptoManager.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    CryptoManager crypto("MyStrongPassword");

    QString password = "SuperSecret123!";
    QByteArray encrypted = crypto.encrypt(password);
    QString decrypted = crypto.decrypt(encrypted);

    qDebug() << "Original: " << password;
    qDebug() << "Encrypted: " << encrypted.toHex();
    qDebug() << "Decrypted: " << decrypted;

    return a.exec();
}
