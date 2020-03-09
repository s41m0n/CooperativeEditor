#include "EditUserProfile.h"

EditUserProfile::EditUserProfile(QWidget *parent) : QMainWindow(parent) {

  //TODO: vanno recuperati tutti i dati dell'utente e messi come placeholder dei vari campi di test. I dati sono nel model del client, non serve chiederli al server

  this->setWindowTitle("Edit User Profile");
  this->setFixedSize(this->minimumSize());

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  registerBox = new QGroupBox(
          "Change the following fields to edit your profile:", mainWidget);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 1, 0, 1, 2);

  imageLabel = new QLabel("Icon:");
  registerBox->layout()->addWidget(imageLabel);

  imageBorder = new QGroupBox(registerBox);
  imageBorder->setLayout(new QVBoxLayout);
  imageBorder->setFixedSize(90, 90);
  registerBox->layout()->addWidget(imageBorder);
  registerBox->layout()->setAlignment(imageBorder, Qt::AlignCenter);

  displayImage = new QLabel(registerBox);
  registerBox->layout()->addWidget(displayImage);

  buttonSelectImage = new QPushButton("Select New Icon", registerBox);
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
  layout->addWidget(buttonSaveAndBackToEditor, 2, 0, 1, 2);

  buttonExit = new QPushButton("Exit Without Saving", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 3, 0, 1, 2);

  buttonDeleteProfile = new QPushButton("Delete Your Profile", mainWidget);
  buttonDeleteProfile->setAutoDefault(true);
  layout->addWidget(buttonDeleteProfile, 4, 0, 1, 2);

  areYouSureQuit = new QMessageBox(this);
  areYouSureQuit->setText("Are you sure you want to exit?");
  areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  areYouSureQuit->setFixedSize(this->minimumSize());

  errorMessageDifferentPasswords = new QMessageBox(this);
  errorMessageDifferentPasswords->setText("The two passwords must match.");
  errorMessageDifferentPasswords->setFixedSize(this->minimumSize());

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
                       auto path =
                               QFileDialog::getOpenFileName(this, tr("Open Image"), "/home",
                                                            tr("Image Files (*.png *.jpg *.bmp)"));
                       if (!path.isEmpty()) {
                         userImage = QImage(path);
                         displayImage->setPixmap(QPixmap::fromImage(userImage).scaled(75, 75,  Qt::KeepAspectRatio));

                         imageBorder->layout()->addWidget(displayImage);
                         imageBorder->show();
                         displayImage->show();
                         buttonSelectImage->setText("Select Another Icon");
                       } else {
                         buttonSelectImage->setText("Select Icon");
                       }
                   });

  QObject::connect(buttonSaveAndBackToEditor, &QAbstractButton::clicked, this,
                   [this]() {
                       //TODO: devo mandare i nuovi dati al server che deve salvarli + aprire di nuovo l'editor con il file di prima
                       this->close();
                   });
}
