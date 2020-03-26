#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

/**
 * Login application window
 */
class Login : public QWidget {

  Q_OBJECT

private:
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

public:
  explicit Login(QWidget *parent = nullptr);

signals:

  /// Signal to notify the controller that user wants log in
  void loginRequest(QString username, QString password);

  /// Signal to notify the view that a user wants to register himself
  void signUp();
};

#endif // COOPERATIVEEDITOR_LOGIN_H
