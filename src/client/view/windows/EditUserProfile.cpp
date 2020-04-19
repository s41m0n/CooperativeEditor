#include <src/include/lib/spdlog/spdlog.h>
#include "EditUserProfile.h"

EditUserProfile::EditUserProfile(QWidget *parent) : QMainWindow(parent),
                                                    usernameLabel(
                                                            new QLabel("")) {

  this->setWindowFlags(Qt::WindowStaysOnTopHint); // windows always on top

  this->setWindowTitle("CooperativeEditor - Edit");
  this->setFixedSize(this->minimumSize());

  auto mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  layout->addWidget(usernameLabel, 1, 0, 1, 1);

  auto registerBox = new QGroupBox(
          "Change the following fields to edit your profile:", mainWidget);
  registerBox->setLayout(new QVBoxLayout());
  layout->addWidget(registerBox, 2, 0, 1, 2);

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

  auto newPasswordLabelConfirm = new QLabel("Repeat New Password:",
                                            registerBox);
  registerBox->layout()->addWidget(newPasswordLabelConfirm);

  newPasswordTextFieldConfirm = new QLineEdit(registerBox);
  newPasswordTextFieldConfirm->setEchoMode(
          newPasswordTextFieldConfirm->Password);
  newPasswordTextFieldConfirm->setStyleSheet("lineedit-password-character: 42");
  registerBox->layout()->addWidget(newPasswordTextFieldConfirm);

  auto buttonSaveAndBackToEditor = new QPushButton("Save and Exit", mainWidget);
  buttonSaveAndBackToEditor->setAutoDefault(true);
  layout->addWidget(buttonSaveAndBackToEditor, 3, 0, 1, 2);

  auto buttonExit = new QPushButton("Exit Without Saving", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 4, 0, 1, 2);

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
                      QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                   "/home",
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
                       if (newPasswordTextField->text() !=
                           newPasswordTextFieldConfirm->text()) {
                         QMessageBox::warning(this, "CooperativeEditor",
                                              "The two new password must match");
                         return;
                       }
                       if (!newPasswordTextField->text().isEmpty() &&
                           oldPasswordTextField->text().isEmpty()) {
                         QMessageBox::warning(this, "CooperativeEditor",
                                              "To modify your password you need to insert also the old one");
                         return;
                       }
                       if (!oldPasswordTextField->text().isEmpty() &&
                           newPasswordTextField->text().isEmpty()) {
                         QMessageBox::warning(this, "CooperativeEditor",
                                              "The old password is required only if you want to change it");
                         return;
                       }
                       emit updateRequest(
                           User(usernameLabel->text(),
                                nameTextField->text(), surnameTextField->text(),
                                emailTextField->text(),
                                newPasswordTextField->text(), userImage),
                           oldPasswordTextField->text());
                   });
}

void EditUserProfile::onUserProfileInfo(User &user) {
  this->userImage = user.getPicture();
  displayImage->setPixmap(QPixmap::fromImage(user.getPicture())
                                  .scaled(75, 75, Qt::KeepAspectRatio));
  imageBorder->layout()->addWidget(displayImage);
  nameTextField->setText(user.getName());
  surnameTextField->setText(user.getSurname());
  emailTextField->setText(user.getEmail());
  usernameLabel->setText("Hello " + user.getUsername() + "!");
}

void EditUserProfile::showEvent(QShowEvent *ev) { emit requestUserProfile(); }

void EditUserProfile::closeEvent(QCloseEvent *event) {
  emit openEditorFromEditProfileNoChanges();
}

void EditUserProfile::onUpdateResponse(bool response) {
  QMessageBox::information(this, "CooperativeEditor",
                           response ? "Your info has been successfully updated"
                                    : "Unable to update your info");
  if (response) {
    close();
  }
}
