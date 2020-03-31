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

  imageLabel = new QLabel("Icon (MaxSize 1 MB):", registerBox);
  registerBox->layout()->addWidget(imageLabel);

  imageBorder = new QGroupBox(registerBox);
  imageBorder->setLayout(new QVBoxLayout);
  imageBorder->setFixedSize(90, 90);
  imageBorder->hide();
  registerBox->layout()->addWidget(imageBorder);
  registerBox->layout()->setAlignment(imageBorder, Qt::AlignCenter);

  displayImage = new QLabel(registerBox);
  displayImage->hide();
  registerBox->layout()->addWidget(displayImage);

  buttonSelectImage = new QPushButton("Select Icon", registerBox);
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
  alreadyRegisteredBox->setWindowTitle("Error");
  layout->addWidget(alreadyRegisteredBox, 2, 0, 1, 2);

  buttonBackToLogin = new QPushButton("Log In", alreadyRegisteredBox);
  buttonBackToLogin->setAutoDefault(true);
  alreadyRegisteredBox->layout()->addWidget(buttonBackToLogin);

  buttonExit = new QPushButton("Exit", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 3, 0, 1, 2);

  buttonExit->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this, [this]() {
    int result = QMessageBox::warning(this, "CooperativeEditor",
                                      "Are you sure you want to exit?",
                                      QMessageBox::Yes, QMessageBox::No);

    if (result == QMessageBox::Yes) {
      this->close();
    }
  });

  QObject::connect(
      buttonSelectImage, &QAbstractButton::clicked, this, [this]() {
        auto path =
            QFileDialog::getOpenFileName(this, tr("Open Image"), "/home",
                                         tr("Image Files (*.png *.jpg *.bmp)"));
        if (!path.isEmpty()) {
          userImage = QImage(path);

          if (userImage.sizeInBytes() > 1048576) { // maxSize = 1MB
            QMessageBox::information(
                this, "CooperativeEditor",
                "The image you have selected is too big. Try again.");
            userImage = QImage();
          } else {
            displayImage->setPixmap(QPixmap::fromImage(userImage).scaled(
                75, 75, Qt::KeepAspectRatio));
            imageBorder->layout()->addWidget(displayImage);
            imageBorder->show();
            displayImage->show();
            buttonSelectImage->setText("Select Another Icon");
          }
        } else {
          buttonSelectImage->setText("Select Icon");
        }
      });

  QObject::connect(buttonBackToLogin, &QAbstractButton::clicked, this,
                   [this]() {
                     emit backToLogin();
                     this->hide();
                   });

  QObject::connect(buttonSignUp, &QAbstractButton::clicked, this, [this]() {
    if (!userImage.isNull() && !nameTextField->text().isEmpty() &&
        !surnameTextField->text().isEmpty() &&
        !usernameTextField->text().isEmpty() &&
        !emailTextField->text().isEmpty() &&
        !passwordTextField->text().isEmpty()) {

      if (passwordTextField->text() == passwordTextFieldConfirm->text()) {
        emit signUpRequest(User(usernameTextField->text(),
                                nameTextField->text(), surnameTextField->text(),
                                emailTextField->text(),
                                passwordTextField->text(), userImage));
      } else {
        QMessageBox::warning(this, "CooperativeEditor",
                             "The two passwords must match.");
      }
    } else {
      QMessageBox::information(this, "CooperativeEditor",
                               "Please fill all the requested fields.");
    }
  });
}
