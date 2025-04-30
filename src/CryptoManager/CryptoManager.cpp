#include "./CryptoManager.h"
#include <QCryptographicHash>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <stdexcept>
#include <QDebug>

CryptoManager::CryptoManager(const QString& password) {
    setPassword(password);
}

void CryptoManager::setPassword(const QString& password) {
    key = generateKey(password);
}

QByteArray CryptoManager::generateKey(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hash.left(32); // AES-256 требует 32 байта ключа
}

QByteArray CryptoManager::encrypt(const QString& plaintext) {
    QByteArray iv(16, 0);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size()) != 1) {
        throw std::runtime_error("Failed to generate IV");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    QByteArray ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH, 0);
    int len = 0, ciphertext_len = 0;

    try {
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                               reinterpret_cast<const unsigned char*>(key.constData()),
                               reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
            throw std::runtime_error("EVP_EncryptInit_ex failed");
        }

        if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()), &len,
                              reinterpret_cast<const unsigned char*>(plaintext.toUtf8().constData()), plaintext.size()) != 1) {
            throw std::runtime_error("EVP_EncryptUpdate failed");
        }
        ciphertext_len = len;

        if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()) + len, &len) != 1) {
            throw std::runtime_error("EVP_EncryptFinal_ex failed");
        }
        ciphertext_len += len;
        ciphertext.resize(ciphertext_len);

    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    EVP_CIPHER_CTX_free(ctx);

    QByteArray result = iv + ciphertext;
    qDebug() << "IV (hex):" << iv.toHex();
    qDebug() << "Ciphertext (hex):" << ciphertext.toHex();
    return result;
}

QString CryptoManager::decrypt(const QByteArray& data) {
    if (data.size() < 16) {
        qDebug() << "Ошибка: данные слишком короткие для IV. Размер данных:" << data.size();
        throw std::runtime_error("Data too short to contain IV");
    }

    QByteArray iv = data.left(16);
    QByteArray ciphertext = data.mid(16);

    qDebug() << "IV (hex):" << iv.toHex();
    qDebug() << "Ciphertext (hex):" << ciphertext.toHex();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    QByteArray plaintext(ciphertext.size() + EVP_MAX_BLOCK_LENGTH, 0);
    int len = 0, plaintext_len = 0;

    try {
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                               reinterpret_cast<const unsigned char*>(key.constData()),
                               reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
            throw std::runtime_error("EVP_DecryptInit_ex failed");
        }

        if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len,
                              reinterpret_cast<const unsigned char*>(ciphertext.constData()), ciphertext.size()) != 1) {
            throw std::runtime_error("EVP_DecryptUpdate failed");
        }
        plaintext_len = len;

        if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()) + len, &len) != 1) {
            unsigned long errCode = ERR_get_error();
            char errBuffer[120];
            ERR_error_string_n(errCode, errBuffer, sizeof(errBuffer));
            qDebug() << "Ошибка: EVP_DecryptFinal_ex не удалось выполнить. Код ошибки:" << errCode << "Сообщение:" << errBuffer;
            throw std::runtime_error(std::string("EVP_DecryptFinal_ex failed: ") + errBuffer);
        }
        plaintext_len += len;
        plaintext.resize(plaintext_len);

    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    EVP_CIPHER_CTX_free(ctx);

    qDebug() << "Расшифрованный текст (UTF-8):" << QString::fromUtf8(plaintext);
    return QString::fromUtf8(plaintext);
}
