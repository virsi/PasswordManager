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
    this->key = generateKey(password);
}

QByteArray CryptoManager::generateKey(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    qDebug() << "Сгенерированный ключ (hex):" << hash.toHex();
    return hash.left(32); // AES-256 требует 32 байта ключа
}

QByteArray CryptoManager::encrypt(const QString& plaintext) {
    QByteArray iv(16, 0);  // Инициализация IV нулями
    if (RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size()) != 1) {
        throw std::runtime_error("Failed to generate IV");
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        throw std::runtime_error("Failed to create cipher context");

    QByteArray ciphertext;
    ciphertext.resize(plaintext.size() + EVP_MAX_BLOCK_LENGTH);

    int len;
    int ciphertext_len;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.constData()),
                           reinterpret_cast<const unsigned char*>(iv.constData())) != 1)
        throw std::runtime_error("EVP_EncryptInit_ex failed");

    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(ciphertext.data()), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.toUtf8().constData()), plaintext.size()) != 1)
        throw std::runtime_error("EVP_EncryptUpdate failed");

    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()) + len, &len) != 1)
        throw std::runtime_error("EVP_EncryptFinal_ex failed");

    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);

    EVP_CIPHER_CTX_free(ctx);

    // Возвращаем IV + зашифрованные данные
    QByteArray result = iv + ciphertext;
    return result;
}


QString CryptoManager::decrypt(const QByteArray& data) {
    if (data.isEmpty()) {
        qDebug() << "Ошибка: входные данные пусты.";
        throw std::runtime_error("Input data is empty");
    }

    if (data.size() < 16) {
        qDebug() << "Ошибка: данные слишком короткие для IV. Размер данных:" << data.size();
        throw std::runtime_error("Data too short to contain IV");
    }

    // Извлечение IV и зашифрованного текста
    QByteArray iv = data.left(16);  // Первые 16 байт — это IV
    QByteArray ciphertext = data.mid(16);  // Оставшаяся часть — это зашифрованный текст
    //QByteArray allText = data;

    qDebug() << "IV:" << iv.toHex();
    qDebug() << "Зашифрованный текст:" << ciphertext.toHex();
    //qDebug() << "Весь шифр:" << data;


    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qDebug() << "Ошибка: не удалось создать контекст шифрования.";
        throw std::runtime_error("Failed to create cipher context");
    }

    QByteArray plaintext;
    plaintext.resize(ciphertext.size());


    int len;
    int plaintext_len;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.constData()),
                           reinterpret_cast<const unsigned char*>(iv.constData())) != 1)
        throw std::runtime_error("EVP_DecryptInit_ex failed");

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len,
                          reinterpret_cast<const unsigned char*>(ciphertext.constData()), ciphertext.size()) != 1)
        throw std::runtime_error("EVP_DecryptUpdate failed");

    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()) + len, &len) != 1) {
        unsigned long errCode = ERR_get_error();
        char errBuffer[120];
        ERR_error_string_n(errCode, errBuffer, sizeof(errBuffer));
        qDebug() << "Ошибка: EVP_DecryptFinal_ex не удалось выполнить. Код ошибки:" << errCode << "Сообщение:" << errBuffer;
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptFinal_ex failed");
    }

    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);

    return QString::fromUtf8(plaintext);
}
