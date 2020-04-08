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
  QLineEdit *nameTextField;
  QLineEdit *surnameTextField;
  QLineEdit *usernameTextField;
  QLineEdit *emailTextField;
  QLineEdit *passwordTextField;
  QLineEdit *passwordTextFieldConfirm;
  QLabel *displayImage;
  QPushButton *buttonSelectImage;
  QGroupBox *imageBorder;
  QImage userImage;

public:
  explicit SignUp(QWidget *parent = nullptr);

signals:

  /**
   * Signal to notify signup attempt
   * @param user  the user structure wiht all parameters
   */
  void signUpRequest(User user);

  /**
   * Signal to notify to go back to login view
   */
  void backToLogin();
};

#endif // COOPERATIVEEDITOR_SIGNUP_H
