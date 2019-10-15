//
// Created by s41m0n on 15/10/19.
//

#ifndef COOPERATIVEEDITOR_USER_H
#define COOPERATIVEEDITOR_USER_H

#include <QImage>

class User {

private:
    QImage icon;

    QString username;

    QString password;

    QString name;

    QString surname;

    QString email;

public:

    User(QImage &icon, QString &username, QString &password, QString &name, QString &surname, QString &email);

    QString &getUsername();

    QString &getPassword();

    QString &getName();

    QString &getSurname();

    QString &getEmail();

    QImage &getIcon();

};


#endif //COOPERATIVEEDITOR_USER_H
