#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include "Login.h"

Login::Login(QWidget *parent) : QMainWindow(parent){

  //this->setFixedSize(355, 240);
  this->setWindowTitle("Login Form");

  auto w = new QWidget(this);
  auto layout = new QGridLayout(w);

  setCentralWidget(w);
  w->setLayout(layout);

  auto label = new QLabel(this);
  label->setText("Welcome to Our Cooperative Editor!");
  layout->addWidget(label, 0, 0);

  auto *b = new QGroupBox("Insert your Personal Data::", w);
  b->setLayout(new QVBoxLayout());
  layout->addWidget(b, 1, 0, 1, 2);

  auto *user = new QLabel("Username:", b);
  b->layout()->addWidget(user);

  auto *u = new QLineEdit();
  b->layout()->addWidget(u);

  auto *psswd = new QLabel("Password:", b);
  b->layout()->addWidget(psswd);

  auto *p = new QLineEdit();
  p->setEchoMode(p->Password);
  p->setStyleSheet("lineedit-password-character: 42");
  b->layout()->addWidget(p);

  auto* button1 = new QPushButton("Enter");
  auto* button2 = new QPushButton("Cancel");

  layout->addWidget(button1, 2, 0);
  layout->addWidget(button2, 3, 0);

}

void Login::onLoginResponse(bool result) {

}
