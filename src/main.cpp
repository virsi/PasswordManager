#include <QCoreApplication>
#include <QDebug>
#include <QMainWindow>
#include "CryptoManager.h"
#include "DatabaseManager.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    CryptoManager crypto("MyStrongPassword");

    QString password = "SuperSecret123!";
    QByteArray encrypted = crypto.encrypt(password);
    QString decrypted = crypto.decrypt(encrypted);

    qDebug() << "Original: " << password;
    qDebug() << "Encrypted: " << encrypted.toHex();
    qDebug() << "Decrypted: " << decrypted;

    DatabaseManager dbManager;
    if (dbManager.openDatabase()) {
        dbManager.createTable();
        dbManager.addPassword("ExampleService", "user@example.com", "examplepassword123");
        dbManager.getPasswords();
    }

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Password Manager");
    mainWindow.resize(600, 400);
    mainWindow.show();

    return a.exec();
}
