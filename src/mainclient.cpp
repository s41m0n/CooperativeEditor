#include <QApplication>
#include <spdlog/spdlog.h>

#include "client/view/View.h"

int main(int argc, char **argv) {

  if (argc < 3) {
    spdlog::info("Usage: {} <serverIp> <serverPort> [-d/--debug]", argv[0]);
    exit(-1);
  }

  // Setting logging level
  if (argc == 4 && (std::strncmp(argv[3], "-d", 2) == 0 ||
                    std::strncmp(argv[3], "--debug", 7) == 0)) {
    spdlog::set_level(spdlog::level::debug);
  } else {
    spdlog::set_level(spdlog::level::critical);
  }

  qRegisterMetaType<QVector<QString>>();
  qRegisterMetaType<FileText>();
  qRegisterMetaType<User>();
  qRegisterMetaType<Attribute>();

  QApplication app(argc, argv);

  Model model;
  Controller controller(&model, argv[1], std::stoi(argv[2]));
  View view(&controller);

  return app.exec();
}
