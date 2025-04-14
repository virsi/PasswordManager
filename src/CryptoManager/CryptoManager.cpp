#include "./CryptoManager.h"
#include <QCryptographicHash>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>

CryptoManager::CryptoManager(const QString& password) {
    key = generateKey(password);
}

QByteArray CryptoManager::generateKey(const QString& password) {
    QByteArray salt = "some_random_salt";
    return QCryptographicHash::hash(password.toUtf8() + salt, QCryptographicHash::Sha256);
}

QByteArray CryptoManager::encrypt(const QString& plaintext) {
    QByteArray iv(16, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());

    QByteArray encrypted(plaintext.size() + 16, 0);
    int outlen = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data()));
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(encrypted.data()), &outlen, reinterpret_cast<const unsigned char*>(plaintext.toUtf8().data()), plaintext.size());

    int final_len = 0;
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(encrypted.data()) + outlen, &final_len);
    EVP_CIPHER_CTX_free(ctx);

    encrypted.resize(outlen + final_len);
    return iv + encrypted;
}

QString CryptoManager::decrypt(const QByteArray& ciphertext) {
    if (ciphertext.size() < 16) return QString();

    QByteArray iv = ciphertext.left(16);
    QByteArray encryptedData = ciphertext.mid(16);

    QByteArray decrypted(encryptedData.size(), 0);
    int outlen = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data()));
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decrypted.data()), &outlen, reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size());

    int final_len = 0;
    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decrypted.data()) + outlen, &final_len);
    EVP_CIPHER_CTX_free(ctx);

    decrypted.resize(outlen + final_len);
    return QString::fromUtf8(decrypted);
}
