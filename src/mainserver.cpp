#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-static-accessed-through-instance"
#include <iostream>
#include <spdlog/spdlog.h>
#include <QApplication>

#include "server/controller/Controller.h"
#include "server/model/Model.h"

int main(int argc, char **argv) {

  QApplication app(argc, argv);

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <port_number>" << std::endl;
    return -1;
  }

  //Setting LogLevel=debug
  spdlog::set_level(spdlog::level::debug);

  Model model;
  Controller controller(&model, std::stoi(argv[1]));

  return app.exec();
}

#pragma clang diagnostic pop