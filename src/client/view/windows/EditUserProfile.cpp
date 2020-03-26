#include "EditUserProfile.h"

EditUserProfile::EditUserProfile(QWidget *parent) : QWidget(parent) {

  this->setWindowFlags(Qt::WindowStaysOnTopHint); //windows always on top

  this->setWindowTitle("Edit User Profile");

  auto layout = new QGridLayout(this);

  this->setLayout(layout);

  registerBox = new QGroupBox(
          "Change the following fields to edit your profile:", this);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 1, 0, 1, 2);

  imageLabel = new QLabel("Icon (MaxSize 1 MB):");
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
  usernameTextField->setReadOnly(true);
  registerBox->layout()->addWidget(usernameTextField);

  oldPasswordLabel = new QLabel("Old Password:", registerBox);
  registerBox->layout()->addWidget(oldPasswordLabel);

  oldPasswordTextField = new QLineEdit(registerBox);
  oldPasswordTextField->setEchoMode(oldPasswordTextField->Password);
  oldPasswordTextField->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(oldPasswordTextField);

  newPasswordLabel = new QLabel("New Password:", registerBox);
  registerBox->layout()->addWidget(newPasswordLabel);

  newPasswordTextField = new QLineEdit(registerBox);
  newPasswordTextField->setEchoMode(newPasswordTextField->Password);
  newPasswordTextField->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(newPasswordTextField);

  newPasswordLabelConfirm = new QLabel("Repeat New Password:", registerBox);
  registerBox->layout()->addWidget(newPasswordLabelConfirm);

  newPasswordTextFieldConfirm = new QLineEdit(registerBox);
  newPasswordTextFieldConfirm->setEchoMode(
          newPasswordTextFieldConfirm->Password);
  newPasswordTextFieldConfirm->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(newPasswordTextFieldConfirm);

  buttonSaveAndBackToEditor = new QPushButton("Save and Exit", this);
  buttonSaveAndBackToEditor->setAutoDefault(true);
  layout->addWidget(buttonSaveAndBackToEditor, 2, 0, 1, 2);

  buttonExit = new QPushButton("Exit Without Saving", this);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 3, 0, 1, 2);

  buttonDeleteProfile = new QPushButton("Delete Your Profile", this);
  buttonDeleteProfile->setAutoDefault(true);
  layout->addWidget(buttonDeleteProfile, 4, 0, 1, 2);

  areYouSureQuit = new QMessageBox(this);
  areYouSureQuit->setText("Are you sure you want to exit?");
  areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  areYouSureQuit->setFixedSize(this->minimumSize());

  errorMessageDifferentPasswords = new QMessageBox(this);
  errorMessageDifferentPasswords->setText("The two passwords must match.");
  errorMessageDifferentPasswords->setWindowTitle("Error");
  errorMessageDifferentPasswords->setFixedSize(this->minimumSize());

  errorMessageWrongOldPassword = new QMessageBox(this);
  errorMessageWrongOldPassword->setText("The old password inserted is wrong.");
  errorMessageWrongOldPassword->setWindowTitle("Error");
  errorMessageWrongOldPassword->setFixedSize(this->minimumSize());

  buttonSaveAndBackToEditor->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this,
                   [this]() {
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
                   });

  QObject::connect(buttonSelectImage, &QAbstractButton::clicked, this,
                   [this]() {
                       auto path =
                               QFileDialog::getOpenFileName(this,
                                                            tr("Open Image"),
                                                            "/home",
                                                            tr("Image Files (*.png *.jpg *.bmp)"));
                       if (!path.isEmpty()) {
                         userImage = QImage(path);

                         if (userImage.sizeInBytes() >
                             1048576) { //maxSize = 1MB
                           auto errorSizeLimit = new QMessageBox(this);
                           errorSizeLimit->setText(
                                   "The image you have selected is too big. Try again.");
                           errorSizeLimit->setFixedSize(this->minimumSize());
                           errorSizeLimit->setWindowTitle("Error");
                           errorSizeLimit->show();
                           userImage = QImage();
                         } else {
                           displayImage->setPixmap(
                                   QPixmap::fromImage(userImage).scaled(75, 75,
                                                                        Qt::KeepAspectRatio));

                           imageBorder->layout()->addWidget(displayImage);
                           imageBorder->show();
                           displayImage->show();
                           buttonSelectImage->setText("Select Another Icon");
                         }
                       } else {
                         buttonSelectImage->setText("Select Icon");
                       }
                   });

  QObject::connect(buttonSaveAndBackToEditor, &QAbstractButton::clicked, this,
                   [this]() {
                       //TODO: devo mandare i nuovi dati al server che deve salvarli + check vecchia password
                       this->close();
                   });

  QObject::connect(buttonDeleteProfile, &QAbstractButton::clicked, this,
                   [this]() {
                       //TODO: segnale per mandare al server la richiesta di cancellazione account
                   });
}

void
EditUserProfile::onUserProfileInfo(const QImage &image, const QString &name,
                                   const QString &surname,
                                   const QString &email,
                                   const QString &username) {
  displayImage->setPixmap(
          QPixmap::fromImage(image).scaled(75, 75, Qt::KeepAspectRatio));
  imageBorder->layout()->addWidget(displayImage);
  nameTextField->setText(name);
  surnameTextField->setText(surname);
  emailTextField->setText(email);
  usernameTextField->setText(username);
}

void EditUserProfile::showEvent(QShowEvent *ev) {
  emit requestUserProfile();
}

void EditUserProfile::closeEvent(QCloseEvent *event) {
  emit openEditorFromEditProfileNoChanges();
}
