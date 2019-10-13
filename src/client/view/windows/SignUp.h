#ifndef COOPERATIVEEDITOR_SIGNUP_H
#define COOPERATIVEEDITOR_SIGNUP_H

#include <QMainWindow>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QGridLayout>
#include <QFileDialog>

/**
 * SignUp application window
 */
class SignUp : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QLabel *title;
    QGroupBox *registerBox;
    QLabel *imageLabel;
    QLabel *nameLabel;
    QLineEdit *nameTextField;
    QLabel *surnameLabel;
    QLineEdit *surnameTextField;
    QLabel *usernameLabel;
    QLineEdit *usernameTextField;
    QLabel *emailLabel;
    QLineEdit *emailTextField;
    QLabel *passwordLabel;
    QLineEdit *passwordTextField;
    QLabel *passwordLabelConfirm;
    QLineEdit *passwordTextFieldConfirm;
    QPushButton *buttonSelectImage;
    QPushButton *buttonSignUp;
    QPushButton *buttonExit;
    QGroupBox *alreadyRegisteredBox;
    QPushButton *buttonBackToLogin;
    QMessageBox *errorMessageEmptyFields;
    QMessageBox *errorMessageDifferentPasswords;
    QMessageBox *areYouSureQuit;
    QMessageBox *errorNotConnected;
    QString userImage;

public:
    explicit SignUp(QWidget *parent = nullptr);

public slots:

    ///Slot to notify view when socket not connected to server
    void onServerUnreachable();

signals:

    ///Signal to notify the server an user wants to sign up
    void signUpRequest(QString image, QString name, QString surname, QString username,
                       QString email, QString password);

    ///Signl to go back to the login page
    void backToLogin();


};

#endif //COOPERATIVEEDITOR_SIGNUP_H
