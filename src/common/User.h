#ifndef COOPERATIVEEDITOR_USER_H
#define COOPERATIVEEDITOR_USER_H

#include <QString>

#include "src/include/lib/serialization/Serializable.h"

/**
 * User class to define a User data structure
 */
class User : public Serializable {

private:

    QString icon;

    QString username;

    QString password;

    QString name;

    QString surname;

    QString email;

public:

    User(QString icon, QString username, QString name, QString surname,
         QString email, QString password);

    User(QString icon, QString username, QString name, QString surname,
         QString email);

    User(QString username, QString password);

    User() = default;

    QString &getUsername();

    QString &getPassword();

    QString &getName();

    QString &getSurname();

    QString &getEmail();

    QString &getIcon();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

};


#endif //COOPERATIVEEDITOR_USER_H
