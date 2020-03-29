#include "Login.h"

Login::Login(QWidget *parent) : QMainWindow(parent) {

  this->setWindowTitle("Login Form");
  this->setFixedSize(this->minimumSize());

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  loginBox = new QGroupBox("Insert your Personal Data:", mainWidget);
  loginBox->setLayout(new QVBoxLayout());
  layout->addWidget(loginBox, 1, 0, 1, 2);

  usernameLabel = new QLabel("Username:", loginBox);
  loginBox->layout()->addWidget(usernameLabel);

  usernameTextField = new QLineEdit(loginBox);
  loginBox->layout()->addWidget(usernameTextField);

  passwordLabel = new QLabel("Password:", loginBox);
  loginBox->layout()->addWidget(passwordLabel);

  passwordTextField = new QLineEdit(loginBox);
  passwordTextField->setEchoMode(passwordTextField->Password);
  passwordTextField->setStyleSheet("lineedit-password-character: 42");
  loginBox->layout()->addWidget(passwordTextField);

  buttonEnter = new QPushButton("Log In", loginBox);
  buttonEnter->setAutoDefault(true);
  loginBox->layout()->addWidget(buttonEnter);

  registerBox = new QGroupBox("Sign up if you do not have credentials:",
                              mainWidget);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 4, 0, 1, 2);

  buttonRegister = new QPushButton("Sign Up", registerBox);
  buttonRegister->setAutoDefault(true);
  registerBox->layout()->addWidget(buttonRegister);

  buttonExit = new QPushButton("Exit", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 5, 0, 1, 2);


  buttonExit->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this,
                   [this]() {
                       int result = QMessageBox::question(this, "CooperativeEditor", "Are you sure you want to exit?", QMessageBox::Yes, QMessageBox::No);

                       if(result == QMessageBox::Yes) {
                         this->close();
                       }
                   });

  QObject::connect(buttonEnter, &QAbstractButton::clicked, this,
                   [this]() {
                       if (!usernameTextField->text().isEmpty() &&
                           !passwordTextField->text().isEmpty()) {
                         emit loginRequest(usernameTextField->text(),
                                           passwordTextField->text());
                       } else {
                         QMessageBox::information(this, "CooperativeEditor", "Please insert username and password.");
                       }
                   });

  QObject::connect(buttonRegister, &QAbstractButton::clicked, this,
                   [this]() {
                       emit signUp();
                       this->hide();
                   });
}
