#ifndef COOPERATIVEEDITOR_SIGNUP_H
#define COOPERATIVEEDITOR_SIGNUP_H

#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <src/common/User.h>

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
    QGroupBox *imageBorder;
    QLabel *displayImage;
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
    QImage userImage;

protected:

    QPushButton *buttonBackToLogin;

public:
    explicit SignUp(QWidget *parent = nullptr);

signals:

    ///Signal to notify the server an user wants to sign up
    void signUpRequest(User user);

    ///Signal to go back to the login page
    void backToLogin();


};

#endif //COOPERATIVEEDITOR_SIGNUP_H
