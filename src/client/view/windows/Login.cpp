#include "Login.h"
#include <QGridLayout>

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
  buttonCancel = new QPushButton("Cancel");

  layout->addWidget(buttonEnter, 2, 0);
  layout->addWidget(buttonCancel, 3, 0);

  QObject::connect(buttonEnter, SIGNAL(clicked()), this,
                   SLOT(emitLoginSignal()));

  QObject::connect(buttonCancel, SIGNAL(clicked()), this,
                   SLOT(cancel()));
}

void Login::onLoginResponse(bool result) {

}

void Login::emitLoginSignal() {
  emit loginRequest(usernameTextField->text(), passwordTextField->text());
}

void Login::cancel() {
  this->close();
}


