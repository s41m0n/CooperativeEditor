#include "EditUserProfile.h"

EditUserProfile::EditUserProfile(QWidget *parent) {
}

void EditUserProfile::onEditProfile() {

  this->setWindowTitle("Edit Your Profile");
  this->setFixedSize(this->minimumSize());

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);
}
