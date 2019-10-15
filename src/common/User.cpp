//
// Created by s41m0n on 15/10/19.
//

#include "User.h"

User::User(QImage &icon, QString &username, QString &password, QString &name,
           QString &surname, QString &email) : icon(icon), username(username),
                                               password(password), name(name),
                                               surname(surname), email(email) {
}

QString &User::getUsername() {
  return username;
}

QString &User::getPassword() {
  return password;
}

QString &User::getName() {
  return name;
}

QString &User::getSurname() {
  return surname;
}

QString &User::getEmail() {
  return email;
}

QImage &User::getIcon() {
  return icon;
}
