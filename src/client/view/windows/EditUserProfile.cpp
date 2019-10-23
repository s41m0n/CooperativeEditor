#include "EditUserProfile.h"

EditUserProfile::EditUserProfile(QWidget *parent) {

  //TODO: vanno recuperati tutti i dati dell'utente e messi come placeholder dei vari campi di test, sarebbe figo mostrare l'icona

  this->setWindowTitle("Edit User Profile");
  this->setFixedSize(this->minimumSize());

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  registerBox = new QGroupBox(
          "Edit your profile:", mainWidget);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 1, 0, 1, 3);

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

  buttonSaveAndBackToEditor = new QPushButton("Save and Exit", mainWidget);
  buttonSaveAndBackToEditor->setAutoDefault(true);
  layout->addWidget(buttonSaveAndBackToEditor, 2, 1, 1, 1);

  buttonExit = new QPushButton("Exit without saving", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 2, 0, 1, 1);

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

  errorNotConnected = new QMessageBox(this);
  errorNotConnected->setText(
          "Sorry, the server is unreachable. Try later, please.");
  errorNotConnected->setFixedSize(this->minimumSize());

  buttonSaveAndBackToEditor->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this,
                   [this]() {
                       int result = areYouSureQuit->exec();

                       switch (result) {
                         case QMessageBox::Yes:
                           this->close();
                           //TODO: riapri editor
                           break;
                         case QMessageBox::No:
                           areYouSureQuit->close();
                           break;
                         default:
                           //error, should never be reached
                           break;
                       }
                   });

  QObject::connect(buttonSelectImage, &QAbstractButton::clicked, this,
                   [this]() {
                       userImage = QFileDialog::getOpenFileName(this,
                                                                tr("Open Image"),
                                                                "/home",
                                                                tr("Image Files (*.png *.jpg *.bmp)"));
                       //TODO: size limit per immagine
                       if (!userImage.isEmpty()) {
                         buttonSelectImage->setText(userImage);
                       } else {
                         buttonSelectImage->setText("Select File");
                       }
                   });

  QObject::connect(buttonSaveAndBackToEditor, &QAbstractButton::clicked, this,
                   [this]() {
                       //TODO: devo mandare i nuovi dati al server che deve salvarli + aprire di nuovo l'editor con il file di prima
                       this->close();
                   });
}
