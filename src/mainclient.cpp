#include <iostream>
#include <boost/serialization/export.hpp>
#include <spdlog/spdlog.h>
#include <boost/lexical_cast.hpp>
#include "client/controller/Controller.h"
#include "client/view/View.h"
#include "client/model/Model.h"


BOOST_CLASS_EXPORT(CrdtMessage)
BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(FileContentMessage)
BOOST_CLASS_EXPORT(FilesListingMessage)

int main(int argc, char** argv) {

  const std::string host("127.0.0.1");
  const std::string port("3000");

  //Setting LogLevel=debug
  spdlog::set_level(spdlog::level::debug);

  auto controller = new Controller(host, port);
  auto view = new View(controller, argc, argv);

  controller->setView(view);
  std::thread t([controller]()-> int{
      return controller->start();
  });
  t.detach();

  return view->init();
}