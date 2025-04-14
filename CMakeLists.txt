cmake_minimum_required(VERSION 3.10)
project(PasswordManager)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 20)

# Подключение Qt, SQLite и OpenSSL
set(Qt6_DIR "/usr/local/opt/qt/lib/cmake/Qt6")
set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
#/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Sql)
#find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Qt6 COMPONENTS Core QUIET)

# Указываем пути к SQLite3 вручную
include_directories(/usr/local/opt/sqlite/include)
link_directories(/usr/local/opt/sqlite/lib)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/DatabaseManager/DatabaseManager.cpp
    src/CryptoManager/CryptoManager.cpp
    src/PasswordManager/PasswordManager.cpp
    src/UI/MainWindow.cpp
)

set(HEADERS
    src/DatabaseManager/DatabaseManager.h
    src/CryptoManager/CryptoManager.h
    src/PasswordManager/PasswordManager.h
    src/UI/MainWindow.h
)

# Основной исполняемый файл
add_executable(password_manager ${SOURCES} ${HEADERS})

# Линкуем библиотеки
target_link_libraries(password_manager
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Sql
    sqlite3 # Используем sqlite3 напрямую
    OpenSSL::SSL
    OpenSSL::Crypto
)

find_package(Qt6 COMPONENTS Core QUIET)
