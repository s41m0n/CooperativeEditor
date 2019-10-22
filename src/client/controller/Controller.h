#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <string>
#include <QTcpSocket>
#include <QDataStream>
#include <QObject>
#include <QString>

#include "components/messages/BasicMessage.h"
#include "client/model/Model.h"
#include "common/TcpSocket.h"


/**
 * Controller class for the client
 *
 */
class Controller : public QObject {

Q_OBJECT


private:
    Model *model;

    TcpSocket socket;

public:

    Controller(Model *model, const std::string &host, int port);

public slots:

    ///Slot to wake up when data ready on the socket
    void onReadyRead();

    ///Slot to wake up when LoginRequest arrived from View
    void onLoginRequest(const QString &username, const QString &password);

    // TODO: Refactor using class USER
    ///Slot to wake up when SignUpRequest arrived from View
    void onSignUpRequest(QString image, QString name, QString surname,
                         QString username,
                         QString email, const QString &password);

    ///Slot to wake up when user insert a char locally
    void onCharInserted(int index, QChar value);

    ///Slot to wake up when user erase a char locally
    void onCharErased(int index);

    void onFileRequest(const QString &filename, bool exists);

signals:

    ///Signal to notify the view about the login
    void loginResponse(bool response);

    ///Signal to notify the view that the client is not connected
    void serverUnreachable();

    ///Signal to notify the view that the server has sent the list of files
    void fileListing(QVector<QString> files);

    ///Signal to notify the view about the possibility to open/create a file
    void fileResult(bool result);

    ///Method to notify the view that text must be updated after a remote op.
    void remoteUpdate(QString text);

};

#endif //COOPERATIVEEDITOR_CONTROLLER_H
