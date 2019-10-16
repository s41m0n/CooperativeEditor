#include <spdlog/spdlog.h>
#include <QApplication>

#include "server/controller/Controller.h"
#include "server/model/Model.h"
#include "server/database/Database.h"

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
  Controller controller(&model, std::stoi(argv[1]));
  Database DB;
  //DB.insertUser("Brazorf33","Ajeje22","Ajeje","Brazorf","ajebra@email.it");

  return app.exec();
}
