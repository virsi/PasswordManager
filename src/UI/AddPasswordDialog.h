#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class AddPasswordDialog : public QDialog {
    Q_OBJECT

public:
    AddPasswordDialog(QWidget *parent = nullptr);

    QString getService() const;
    QString getlogin() const;
    QString getPassword() const;

private:
    QLineEdit *serviceEdit;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
};

#endif // ADDPASSWORDDIALOG_H
