#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-static-accessed-through-instance"
#include <iostream>
#include <spdlog/spdlog.h>
#include <QApplication>

#include "client/controller/Controller.h"
#include "client/view/View.h"
#include "client/model/Model.h"

int main(int argc, char** argv) {

  if(argc != 3) {
    std::cout << "Usage: " << argv[0] << "<serverIp> <serverPort>" << std::endl;
    exit(-1);
  }

  //Setting LogLevel=debug
  spdlog::set_level(spdlog::level::debug);

  QApplication app(argc, argv);

  Model model;
  Controller controller(&model, argv[1], std::stoi(argv[2]));
  View view(&controller);

  controller.setView(&view);
  view.init();

  return app.exec();
}
#pragma clang diagnostic pop