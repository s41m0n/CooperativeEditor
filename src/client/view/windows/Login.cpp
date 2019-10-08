#include "Login.h"
#include <QGridLayout>
#include <spdlog/spdlog.h>

Login::Login(QWidget *parent) : QMainWindow(parent) {

  //this->setFixedSize(355, 240);
  this->setWindowTitle("Login Form");

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  title = new QLabel(this);
  title->setText("Welcome to Our Cooperative Editor!");
  layout->addWidget(title, 0, 0);

  loginBox = new QGroupBox("Insert your Personal Data:", mainWidget);
  loginBox->setLayout(new QVBoxLayout());
  layout->addWidget(loginBox, 1, 0, 1, 2);

  usernameLabel = new QLabel("Username:", loginBox);
  loginBox->layout()->addWidget(usernameLabel);

  usernameTextField = new QLineEdit();
  loginBox->layout()->addWidget(usernameTextField);

  passwordLabel = new QLabel("Password:", loginBox);
  loginBox->layout()->addWidget(passwordLabel);

  passwordTextField = new QLineEdit();
  passwordTextField->setEchoMode(passwordTextField->Password);
  passwordTextField->setStyleSheet("lineedit-password-character: 42");
  loginBox->layout()->addWidget(passwordTextField);

  buttonEnter = new QPushButton("Enter");
  buttonCancel = new QPushButton("Exit");

  errorMessageEmptyFields = new QMessageBox();
  errorMessageEmptyFields->setText("Please insert username and password.");

  areYouSureQuit = new QMessageBox();
  areYouSureQuit->setText("Are you sure you want to exit?");
  areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  errorNotLogged = new QMessageBox();
  errorNotLogged->setText("Username and Password are not correct.");

  errorNotConnected = new QMessageBox();
  errorNotConnected->setText("Sorry, the server is unreachable. Try later, please.");

  layout->addWidget(buttonEnter, 2, 0);
  layout->addWidget(buttonCancel, 3, 0);

  QObject::connect(buttonEnter, SIGNAL(clicked()), this,
                   SLOT(emitLoginSignal()));

  QObject::connect(buttonCancel, SIGNAL(clicked()), this,
                   SLOT(cancel()));
}

void Login::onServerUnreachable() {
  errorNotConnected->exec();
}

void Login::onLoginResponse(bool result) {
  if(result) {
    this->close();
  } else {
    errorNotLogged->exec();
  }
}

void Login::emitLoginSignal() {
  if(!usernameTextField->text().isEmpty() && !passwordTextField->text().isEmpty()) {
    emit loginRequest(usernameTextField->text(), passwordTextField->text());
  }else {
    errorMessageEmptyFields->exec();
  }
}

void Login::cancel() {
  int result = areYouSureQuit->exec();

  switch (result) {
    case QMessageBox::Yes:
      this->close();
      break;
    case QMessageBox::No:
      areYouSureQuit->close();
      break;
    default:
      //error, should never be reached
      break;
  }
}


