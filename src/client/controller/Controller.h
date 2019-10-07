#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <string>
#include <QTcpSocket>
#include <QObject>

#include "components/Message.h"
#include "client/model/Model.h"

/**
 * Controller class for the client
 *
 * @autor Simone Magnani - s41m0n
 */
class Controller : public QObject {

Q_OBJECT

private:
    ///The instance of the model
    Model *model;

    ///The QTcpSOcket
    QTcpSocket _socket;

public:
    ///Classic constructor
    Controller(Model *model, const std::string &host, int port);

    ///Destructor used for debug purpose only by now
    ~Controller() override;

public slots:

    ///Method to be called once something has been read on socket
    void onReadyRead();

    void onLoginRequest(std::string &username, std::string &password);

    ///Method to handle a CrdtMessage - symbol inserted
    void handle_insert(int index, char value);

    ///Method to handle a CrdtMessage - symbol erased
    void handle_erase(int index);

signals:

  void loginResponse(bool response);


};


#endif //COOPERATIVEEDITOR_CONTROLLER_H
