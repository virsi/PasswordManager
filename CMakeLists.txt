cmake_minimum_required(VERSION 3.10)
project(PasswordManager)

set(CMAKE_CXX_STANDARD 20)

# Подключение Qt, SQLite и OpenSSL
set(Qt6_DIR "/usr/local/opt/qt/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Sql)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/DatabaseManager/DatabaseManager.cpp
    src/CryptoManager/CryptoManager.cpp
    src/PasswordManager/PasswordManager.cpp
)

set(HEADERS
    src/DatabaseManager/DatabaseManager.h
    src/CryptoManager/CryptoManager.h
    src/PasswordManager/PasswordManager.h
)

# Основной исполняемый файл
add_executable(password_manager ${SOURCES} ${HEADERS})

# Линкуем библиотеки
target_link_libraries(password_manager
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Sql
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
)
