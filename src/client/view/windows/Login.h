#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QMainWindow>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QGridLayout>

/**
 * Login application window
 */
class Login : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QLabel *title;
    QGroupBox *loginBox;
    QLabel *usernameLabel;
    QLineEdit *usernameTextField;
    QLabel *passwordLabel;
    QLineEdit *passwordTextField;
    QGroupBox *registerBox;
    QPushButton *buttonRegister;
    QPushButton *buttonEnter;
    QPushButton *buttonExit;
    QMessageBox *errorMessageEmptyFields;
    QMessageBox *areYouSureQuit;
    QMessageBox *errorNotConnected;
    QMessageBox *errorNotLogged;

public:

    explicit Login(QWidget *parent = nullptr);

public slots:

    ///Slot to notify view when socket not connected to server
    void onServerUnreachable();

    ///Slot to notify view when login response received
    void onLoginResponse(bool result);

signals:

    ///Signal to notify the controller that user wants log in
    void loginRequest(QString username, QString password);

    ///Signal to notify the view that a user wants to register himself
    void signUp();
};


#endif //COOPERATIVEEDITOR_LOGIN_H
