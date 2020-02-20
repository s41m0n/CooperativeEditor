#include "User.h"

#include <utility>

User::User(QString username, QString name,
           QString surname, QString email, QString password, QImage picture) : username(
        std::move(username)), name(std::move(name)), password(std::move(password)),
                                                                               surname(std::move(
                                                                                       surname)),
                                                                               email(std::move(
                                                                                       email)),
                                                                               picture(std::move(
                                                                                       picture)) {
}

User::User(QString username, QString name,
           QString surname, QString email, QImage picture) : username(
        std::move(username)), name(std::move(name)), surname(std::move(surname)),
                                                             email(std::move(email)),
                                                             picture(std::move(picture)) {
}

User::User(QString username, QString name,
           QString surname, QString email, QString password) : username(std::move(username)),
                                                               name(std::move(name)),
                                                               password(std::move(password)),
                                                               surname(std::move(surname)),
                                                               email(std::move(email)) {
}


User::User(QString username, QString name, QString surname,
           QString email) : username(std::move(username)), name(std::move(name)),
                            surname(std::move(surname)), email(std::move(email)) {
}

User::User(QString username, QString password) : User(std::move(username),
                                                      {}, {}, {},
                                                      std::move(password)) {
}

QString &User::getUsername() {
    return username;
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

QString &User::getPassword() {
    return password;
}

QImage &User::getPicture() {
    return picture;
}

std::string User::toStdString(int level) {
    return std::string(std::string(level, '\t') + "User{\n" +
                       std::string(level + 1, '\t') + "username: " +
                       username.toStdString() + "\n" +
                       std::string(level + 1, '\t') + "password: " +
                       password.toStdString() + "\n" +
                       std::string(level + 1, '\t') + "name: " +
                       name.toStdString() + "\n" +
                       std::string(level + 1, '\t') + "surname: " +
                       surname.toStdString() + "\n" +
                       std::string(level + 1, '\t') + "email: " +
                       email.toStdString() + "\n" +
                       std::string(level + 1, '\t') + "pictureSize: " +
                       std::to_string(picture.sizeInBytes()) + "\n" +
                       std::string(level + 1, '\t') + "}");
}

QDataStream &User::serialize(QDataStream &stream) const {
    stream << username << password << email << name << surname << picture;
    return stream;
}

QDataStream &User::deserialize(QDataStream &stream) {
    stream >> username >> password >> email >> name >> surname >> picture;
    return stream;
}


