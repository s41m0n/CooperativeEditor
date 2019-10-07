//
// Created by s41m0n on 07/10/19.
//

#include <QLabel>
#include "LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent){
  auto label = new QLabel(this);
  label->setText("Hello World");
}
