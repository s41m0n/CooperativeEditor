#include <boost/serialization/export.hpp>
#include <boost/lexical_cast.hpp>
#include <spdlog/spdlog.h>
#include "server/controller/Controller.h"
#include "server/model/Model.h"


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

    auto model = new Model();
    auto controller = new Controller(model, port);

    return controller->start();

}
