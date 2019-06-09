//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H


#include <queue>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include "../components/Message.h"
#include "../network/Connection.h"
#include "../crdtAlgorithm/CrdtAlgorithm.h"

/**
 * NetworkServer class, the server in this architecture
 *
 * @author Simone Magnani - s41m0n
 */
class Controller {

private:
    std::map<int, std::pair<boost::shared_ptr<BasicMessage>, boost::shared_ptr<BasicMessage>>> buffers;
    ///The map of the connection, containing the editorId and the pointer to the connection
    std::map<std::string, std::vector<Symbol>> openedFiles;
    ///Map of connection ID, filename required by user and Connection ptr
    std::map<int, std::pair<std::string, connection_ptr>> connections;
    ///The queue containing all the messages
    std::queue<boost::shared_ptr<CrdtMessage>> messages;
    ///The mutex used for the message queue
    std::mutex queueMutex;
    ///The mutex used to update the symbols array
    std::mutex symbolsMutes;
    ///The mutex used for the connections map
    std::mutex connectionMapMutex;
    ///Editor Id unique generator
    unsigned int idGenerator;
    ///Set of all available files
    std::string availableFiles;
    /// The acceptor object used to accept incoming socket connections.
    boost::asio::ip::tcp::acceptor acceptor_;

public:
    ///Class constructor, given an io_service and a port
    Controller(boost::asio::io_service &io_service, unsigned short port);

    ///Class destructor (actually used to debug)
    ~Controller();

    ///Method to dispatch messages
    void dispatch();

    /// Handle completion of a accept operation.
    void handle_accept(const boost::system::error_code &e, connection_ptr &conn);

    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code &e, int connId);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code &e, int connId);

    ///Method to write on file the respective sequence of symbols
    void writeOnFile(std::string& filename);

    ///Method to restore from file the respective sequence of symbols
    void restoreFromFile(std::string& filename);

    ///Method to perform directory listing for file with certain extension
    void loadAllFileNames();

};

#endif //COOPERATIVEEDITOR_CONTROLLER_H
