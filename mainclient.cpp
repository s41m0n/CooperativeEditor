#include <iostream>
#include <boost/serialization/export.hpp>
#include <spdlog/spdlog.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include "client/SharedEditor.h"


BOOST_CLASS_EXPORT(CrdtMessage)
BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(FileContentMessage)
BOOST_CLASS_EXPORT(FilesListingMessage)

int main() {
    const std::string host("127.0.0.1");
    const std::string port("3000");

    std::vector<std::pair<int, char>> toInsert({{0,'c'}, {1, 'i'}, {2, 'a'}, {3, 'o'}, {4, ' '},
                                                       {5,'s'}, {6, 'i'}, {7, 'm'}, {8, 'o'},
                                                       {4, ' '}, {5, 's'}, {6, 'c'}, {7, 'u'}, {8, 's'}, {9, 'a'}});
    std::vector<int> toDelete;

    //Setting LogLevel=debug
    spdlog::set_level(spdlog::level::debug);


    boost::asio::io_service io_service;

    //std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));

    std::shared_ptr<SharedEditor> client(new SharedEditor(io_service, host, port));

    boost::thread io_thread(boost::bind(&boost::asio::io_service::run, &io_service));

    std::for_each(toInsert.begin(), toInsert.end(), [&client](std::pair<int, char> pair) -> void {
        client->localInsert(pair.first, pair.second);
    });

    std::for_each(toDelete.begin(), toDelete.end(), [&client](int index) -> void {
        client->localErase(index);
    });

}