#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QMainWindow>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QMessageBox>

/**
 * Login application window
 */
class Login : public QMainWindow {

Q_OBJECT

private:
    ///The private components
    QWidget *mainWidget;
    QLabel *title;
    QGroupBox *loginBox;
    QLabel *usernameLabel;
    QLineEdit *usernameTextField;
    QLabel *passwordLabel;
    QLineEdit *passwordTextField;
    QPushButton *buttonEnter;
    QPushButton *buttonCancel;
    QMessageBox *errorMessageEmptyFields;
    QMessageBox *areYouSureQuit;
    QMessageBox *errorNotConnected;
    QMessageBox *errorNotLogged;

public:
    ///Constructor
    explicit Login(QWidget *parent = nullptr);

public slots:

    ///Slot to notify view when socket not connected to server
    void onServerUnreachable();

    ///Slot to notify view when login response received
    void onLoginResponse(bool result);

signals:

    ///Signal to notify controller that user wants log in
    void loginRequest(QString username, QString password);
};


#endif //COOPERATIVEEDITOR_LOGIN_H
