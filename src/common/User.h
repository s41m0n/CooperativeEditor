#ifndef COOPERATIVEEDITOR_USER_H
#define COOPERATIVEEDITOR_USER_H

#include <QString>
#include <QtGui/QImage>
#include <utility>

#include "src/include/lib/serialization/Serializable.h"

/**
 * User class to define a User data structure
 */
class User : public Serializable {

private:

    QString username;

    QString password;

    QString name;

    QString surname;

    QString email;

    QImage picture;


public:
    User(QString username, QString name, QString surname,
         QString email, QString password, QImage picture);

    User(QString username, QString name, QString surname,
         QString email, QString password);

    User(QString username, QString name, QString surname,
         QString email);

    User(QString username, QString name, QString surname,
         QString email, QImage picture);

    User(QString username, QString password);

    User() = default;

    QString &getUsername();

    QString &getPassword();

    QString &getName();

    QString &getSurname();

    QString &getEmail();

    QImage &getPicture();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

};

Q_DECLARE_METATYPE(User);

#endif //COOPERATIVEEDITOR_USER_H