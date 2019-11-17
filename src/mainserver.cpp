#include <spdlog/spdlog.h>
#include <QApplication>
#include <iostream>

#include "server/controller/Controller.h"
#include "server/model/Model.h"


int main(int argc, char **argv) {

  if (argc < 2) {
    spdlog::info("Usage: {} <port> [-d/--debug]", argv[0]);
    exit(-1);
  }

  //Setting logging level
  if (argc == 3 && (std::strncmp(argv[2], "-d", 2) == 0 ||
                    std::strncmp(argv[2], "--debug", 7) == 0)) {
    spdlog::set_level(spdlog::level::debug);
  } else {
    spdlog::set_level(spdlog::level::critical);
  }

  QApplication app(argc, argv);

  Model model;

  /*User user("path","Ciccio2","Frank","Palm","frank2@email","Ciccione");
  model.registerUser(user);
  User user2("Ciccio2","Ciccione");
  model.logInUser(user2);
  User user3(user2.getUsername(),"Francesco","Palmieri","frank2@email.it","Ciccione",user2.getPicture());
  model.updateUser(user3);
  User user4("Ciccio2","Ciccione");
  model.logInUser(user4);*/

  Controller controller(&model, std::stoi(argv[1]));


  return app.exec();
}
