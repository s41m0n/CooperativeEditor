#ifndef COOPERATIVEEDITOR_USER_H
#define COOPERATIVEEDITOR_USER_H

#include <QImage>

/**
 * User class to define a User data structure
 */
class User {

private:

    QImage icon;

    QString username;

    QString name;

    QString surname;

    QString email;

public:

    User(QImage &icon, QString &username, QString &password, QString &name, QString &surname, QString &email);

    QString &getUsername();

    QString &getName();

    QString &getSurname();

    QString &getEmail();

    QImage &getIcon();

};


#endif //COOPERATIVEEDITOR_USER_H
