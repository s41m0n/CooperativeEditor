#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMainWindow>
#include <QString>
#include <QMessageBox>

class Login : public QMainWindow {

    Q_OBJECT

private:
    QWidget* mainWidget;
    QLabel* title;
    QGroupBox* loginBox;
    QLabel* usernameLabel;
    QLineEdit* usernameTextField;
    QLabel* passwordLabel;
    QLineEdit* passwordTextField;
    QPushButton* buttonEnter;
    QPushButton* buttonCancel;
    QMessageBox* errorMessageEmptyFields;
    QMessageBox* areYouSureQuit;
    QMessageBox* errorNotConnected;
    QMessageBox* errorNotLogged;
void emitLoginSignal();
    void cancel();

public:
    explicit Login(QWidget *parent = nullptr);

public slots:
    void onServerUnreachable();
    void onLoginResponse(bool result);

signals:
    void loginRequest(QString username, QString password);
};


#endif //COOPERATIVEEDITOR_LOGIN_H
