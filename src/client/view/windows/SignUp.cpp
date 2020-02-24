#include "SignUp.h"

SignUp::SignUp(QWidget *parent) : QMainWindow(parent) {

  this->setWindowTitle("Sign Up Form");
  this->setFixedSize(this->minimumSize());

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  title = new QLabel(this);
  title->setText("Welcome to Our Cooperative Editor!");
  layout->addWidget(title, 0, 0);

  registerBox = new QGroupBox(
      "Fill the following fields to sign up to the system:", mainWidget);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 1, 0, 1, 2);

  imageLabel = new QLabel("Select your image:");
  registerBox->layout()->addWidget(imageLabel);

  buttonSelectImage = new QPushButton("Select file", registerBox);
  buttonSelectImage->setAutoDefault(true);
  registerBox->layout()->addWidget(buttonSelectImage);

  nameLabel = new QLabel("Name:", registerBox);
  registerBox->layout()->addWidget(nameLabel);

  nameTextField = new QLineEdit();
  registerBox->layout()->addWidget(nameTextField);

  surnameLabel = new QLabel("Surname:", registerBox);
  registerBox->layout()->addWidget(surnameLabel);

  surnameTextField = new QLineEdit();
  registerBox->layout()->addWidget(surnameTextField);

  emailLabel = new QLabel("E-mail:", registerBox);
  registerBox->layout()->addWidget(emailLabel);

  emailTextField = new QLineEdit();
  registerBox->layout()->addWidget(emailTextField);

  usernameLabel = new QLabel("Username:", registerBox);
  registerBox->layout()->addWidget(usernameLabel);

  usernameTextField = new QLineEdit();
  registerBox->layout()->addWidget(usernameTextField);

  passwordLabel = new QLabel("Password:", registerBox);
  registerBox->layout()->addWidget(passwordLabel);

  passwordTextField = new QLineEdit(registerBox);
  passwordTextField->setEchoMode(passwordTextField->Password);
  passwordTextField->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(passwordTextField);

  passwordLabelConfirm = new QLabel("Repeat Password:", registerBox);
  registerBox->layout()->addWidget(passwordLabelConfirm);

  passwordTextFieldConfirm = new QLineEdit(registerBox);
  passwordTextFieldConfirm->setEchoMode(passwordTextFieldConfirm->Password);
  passwordTextFieldConfirm->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(passwordTextFieldConfirm);

  buttonSignUp = new QPushButton("Sign Up", registerBox);
  buttonSignUp->setAutoDefault(true);
  registerBox->layout()->addWidget(buttonSignUp);

  alreadyRegisteredBox =
      new QGroupBox("Are you already registered?", mainWidget);
  alreadyRegisteredBox->setLayout(new QVBoxLayout());
  layout->addWidget(alreadyRegisteredBox, 2, 0, 1, 2);

  buttonBackToLogin = new QPushButton("Log In", alreadyRegisteredBox);
  buttonBackToLogin->setAutoDefault(true);
  alreadyRegisteredBox->layout()->addWidget(buttonBackToLogin);

  buttonExit = new QPushButton("Exit", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 3, 0, 1, 2);

  errorMessageEmptyFields = new QMessageBox(this);
  errorMessageEmptyFields->setText("Please fill all the requested fields.");
  errorMessageEmptyFields->setFixedSize(this->minimumSize());

  areYouSureQuit = new QMessageBox(this);
  areYouSureQuit->setText("Are you sure you want to exit?");
  areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  areYouSureQuit->setFixedSize(this->minimumSize());

  errorMessageDifferentPasswords = new QMessageBox(this);
  errorMessageDifferentPasswords->setText("The two passwords must match.");
  errorMessageDifferentPasswords->setFixedSize(this->minimumSize());

  buttonExit->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this, [this]() {
    int result = areYouSureQuit->exec();

    switch (result) {
    case QMessageBox::Yes:
      this->close();
      break;
    case QMessageBox::No:
      areYouSureQuit->close();
      break;
    default:
      // error, should never be reached
      break;
    }
  });

  QObject::connect(
      buttonSelectImage, &QAbstractButton::clicked, this, [this]() {
        auto path =
            QFileDialog::getOpenFileName(this, tr("Open Image"), "/home",
                                         tr("Image Files (*.png *.jpg *.bmp)"));
        if (!path.isEmpty()) {
          buttonSelectImage->setText(path);
          userImage = QImage(path);
        } else {
          buttonSelectImage->setText("Select File");
        }
      });
  QObject::connect(buttonBackToLogin, &QAbstractButton::clicked, this,
                   [this]() { emit backToLogin(); });

  QObject::connect(buttonSignUp, &QAbstractButton::clicked, this, [this]() {
    if (!userImage.isNull() && !nameTextField->text().isEmpty() &&
        !surnameTextField->text().isEmpty() &&
        !usernameTextField->text().isEmpty() &&
        !emailTextField->text().isEmpty() &&
        !passwordTextField->text().isEmpty()) {

      if (passwordTextField->text() == passwordTextFieldConfirm->text()) {
        emit signUpRequest(userImage, nameTextField->text(),
                           surnameTextField->text(), usernameTextField->text(),
                           emailTextField->text(), passwordTextField->text());
      } else {
        errorMessageDifferentPasswords->exec();
      }
    } else {
      errorMessageEmptyFields->exec();
    }
  });
}
