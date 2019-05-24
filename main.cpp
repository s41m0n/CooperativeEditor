#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include "server/NetworkServer.h"
#include "client/SharedEditor.h"

void spawnClient(const std::string& host, const std::string& port, std::vector<std::pair<int, char>> toInsert, std::vector<int> toDelete, int clientId) {
    boost::thread t;
    if(clientId == 2)
        sleep(5);
    try {

        boost::asio::io_service io_service;

        std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
        t = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));

        {
            std::shared_ptr<SharedEditor> client(new SharedEditor(io_service, host, port));

            std::for_each(toInsert.begin(), toInsert.end(), [&client](std::pair<int, char> pair) -> void {
                client->localInsert(pair.first, pair.second);
            });
            std::for_each(toDelete.begin(), toDelete.end(), [&client](int index) -> void {
                client->localErase(index);
            });
            while(1);
        }
    }
    catch (std::exception& e) {
        spdlog::debug("Client {} has thrown an exception -> {}", clientId, e.what());
    }

    t.join();
    spdlog::debug("Client {} has successfully ended", clientId);
}

int main() {
    const std::string host("127.0.0.1");
    const std::string service("3000");
    auto port = boost::lexical_cast<unsigned short>(service);

    std::vector<std::pair<int, char>> client1ToInsert({{0,'c'}, {1, 'i'}, {2, 'a'}, {3, 'o'}});
    std::vector<int> client1ToDelete;

    std::vector<std::pair<int, char>> client2ToInsert({{4,'s'}, {5, 'i'}, {6, 'm'}, {7, 'o'}});
    std::vector<int> client2ToDelete;

    //Setting LogLevel=debug
    spdlog::set_level(spdlog::level::debug);

    //Starting Server
    boost::asio::io_service io_service;
    std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
    std::shared_ptr<NetworkServer> server(new NetworkServer(io_service, port));

    //Starting client1
    std::thread client1(spawnClient, host, service, client1ToInsert, client1ToDelete, 1);
    client1.detach();

    //Starting client2
    std::thread client2(spawnClient, host, service, client2ToInsert, client2ToDelete, 2);
    client2.detach();

    while(1) server->dispatch();
}
