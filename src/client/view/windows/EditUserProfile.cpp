#include "EditUserProfile.h"

EditUserProfile::EditUserProfile(QWidget *parent) : QMainWindow(parent) {

  this->setWindowFlags(Qt::WindowStaysOnTopHint); // windows always on top

  this->setWindowTitle("Edit User Profile");
  this->setFixedSize(this->minimumSize());

  auto mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  auto registerBox = new QGroupBox(
      "Change the following fields to edit your profile:", mainWidget);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 1, 0, 1, 2);

  auto imageLabel = new QLabel("Icon (MaxSize 1 MB):");
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

  auto nameLabel = new QLabel("Name:", registerBox);
  registerBox->layout()->addWidget(nameLabel);

  nameTextField = new QLineEdit();
  registerBox->layout()->addWidget(nameTextField);

  auto surnameLabel = new QLabel("Surname:", registerBox);
  registerBox->layout()->addWidget(surnameLabel);

  surnameTextField = new QLineEdit();
  registerBox->layout()->addWidget(surnameTextField);

  auto emailLabel = new QLabel("E-mail:", registerBox);
  registerBox->layout()->addWidget(emailLabel);

  emailTextField = new QLineEdit();
  registerBox->layout()->addWidget(emailTextField);

  auto usernameLabel = new QLabel("Username:", registerBox);
  registerBox->layout()->addWidget(usernameLabel);

  usernameTextField = new QLineEdit();
  usernameTextField->setReadOnly(true);
  registerBox->layout()->addWidget(usernameTextField);

  auto oldPasswordLabel = new QLabel("Old Password:", registerBox);
  registerBox->layout()->addWidget(oldPasswordLabel);

  oldPasswordTextField = new QLineEdit(registerBox);
  oldPasswordTextField->setEchoMode(oldPasswordTextField->Password);
  oldPasswordTextField->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(oldPasswordTextField);

  auto newPasswordLabel = new QLabel("New Password:", registerBox);
  registerBox->layout()->addWidget(newPasswordLabel);

  newPasswordTextField = new QLineEdit(registerBox);
  newPasswordTextField->setEchoMode(newPasswordTextField->Password);
  newPasswordTextField->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(newPasswordTextField);

  auto newPasswordLabelConfirm = new QLabel("Repeat New Password:", registerBox);
  registerBox->layout()->addWidget(newPasswordLabelConfirm);

  newPasswordTextFieldConfirm = new QLineEdit(registerBox);
  newPasswordTextFieldConfirm->setEchoMode(
      newPasswordTextFieldConfirm->Password);
  newPasswordTextFieldConfirm->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(newPasswordTextFieldConfirm);

  auto buttonSaveAndBackToEditor = new QPushButton("Save and Exit", mainWidget);
  buttonSaveAndBackToEditor->setAutoDefault(true);
  layout->addWidget(buttonSaveAndBackToEditor, 2, 0, 1, 2);

  auto buttonExit = new QPushButton("Exit Without Saving", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 3, 0, 1, 2);

  auto buttonDeleteProfile = new QPushButton("Delete Your Profile", mainWidget);
  buttonDeleteProfile->setAutoDefault(true);
  layout->addWidget(buttonDeleteProfile, 4, 0, 1, 2);

  // TODO: implementare questi messaggi lato server
  // QMessageBox::warning(this, "CooperativeEditor", "The two passwords must
  // match.") QMessageBox::warning(this, "CooperativeEditor", "The old password
  // inserted is wrong.")

  buttonSaveAndBackToEditor->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this, [this]() {
    int result = QMessageBox::question(this, "CooperativeEditor",
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
            auto errorSizeLimit = new QMessageBox(this);
            errorSizeLimit->setText(
                "The image you have selected is too big. Try again.");
            errorSizeLimit->setFixedSize(this->minimumSize());
            errorSizeLimit->setWindowTitle("Error");
            errorSizeLimit->show();
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

  QObject::connect(buttonSaveAndBackToEditor, &QAbstractButton::clicked, this,
                   [this]() {
                     // TODO: devo mandare i nuovi dati al server che deve
                     // salvarli + check vecchia password
                     this->close();
                   });

  QObject::connect(buttonDeleteProfile, &QAbstractButton::clicked, this,
                   [this]() {
                     // TODO: segnale per mandare al server la richiesta di
                     // cancellazione account
                   });
}

void EditUserProfile::onUserProfileInfo(User &user) {
  displayImage->setPixmap(QPixmap::fromImage(user.getPicture())
                              .scaled(75, 75, Qt::KeepAspectRatio));
  imageBorder->layout()->addWidget(displayImage);
  nameTextField->setText(user.getName());
  surnameTextField->setText(user.getSurname());
  emailTextField->setText(user.getEmail());
  usernameTextField->setText(user.getUsername());
}

void EditUserProfile::showEvent(QShowEvent *ev) { emit requestUserProfile(); }

void EditUserProfile::closeEvent(QCloseEvent *event) {
  emit openEditorFromEditProfileNoChanges();
}
