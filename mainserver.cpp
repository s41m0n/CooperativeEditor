#include <iostream>
#include <boost/serialization/export.hpp>
#include <spdlog/spdlog.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include "server/NetworkServer.h"


BOOST_CLASS_EXPORT(CrdtMessage)
BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(FileContentMessage)
BOOST_CLASS_EXPORT(FilesListingMessage)


int main() {
    const std::string host("127.0.0.1");
    const std::string service("3000");
    auto port = boost::lexical_cast<unsigned short>(service);

    //Setting LogLevel=debug
    spdlog::set_level(spdlog::level::debug);

    //Starting Server
    boost::asio::io_service io_service;
    std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
    std::shared_ptr<NetworkServer> server(new NetworkServer(io_service, port));

    while(1) server->dispatch();
}
