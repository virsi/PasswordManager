#include "AddPasswordDialog.h"

AddPasswordDialog::AddPasswordDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Добавить пароль");

    QVBoxLayout *layout = new QVBoxLayout(this);

    serviceEdit = new QLineEdit(this);
    serviceEdit->setPlaceholderText("Сервис");

    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Имя пользователя");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *addButton = new QPushButton("Добавить", this);
    QPushButton *cancelButton = new QPushButton("Отмена", this);

    layout->addWidget(serviceEdit);
    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(addButton);
    layout->addWidget(cancelButton);

    connect(addButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString AddPasswordDialog::getService() const {
    return serviceEdit->text();
}

QString AddPasswordDialog::getlogin() const {
    return loginEdit->text();
}

QString AddPasswordDialog::getPassword() const {
    return passwordEdit->text();
}
