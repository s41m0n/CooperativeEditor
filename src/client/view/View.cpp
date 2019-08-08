//  controller->start();

// Created by s41m0n on 10/06/19.
//

#include <spdlog/spdlog.h>
#include <vector>
#include "View.h"

View::View(Controller* controller, int argc, char** argv) : controller(controller), app(new QApplication(argc, argv)), label(new QLabel("HELLO QT")){
  spdlog::debug("SharedEditor::Created View");
}

View::~View() {
  spdlog::debug("SharedEditor::Destroyed View");
}

int View::init() {
  std::vector<std::pair<int, char>> toInsert({{0,'c'}, {1, 'i'}, {2, 'a'}, {3, 'o'}, {4, ' '},
                                              {5,'s'}, {6, 'i'}, {7, 'm'}, {8, 'o'},
                                              {4, ' '}, {5, 's'}, {6, 'c'}, {7, 'u'}, {8, 's'}, {9, 'a'}});
  std::vector<int> toDelete({});

  for(auto pair : toInsert) {
    controller->handle_insert(pair.first, pair.second);
  }

  for(auto pos : toDelete) {
    controller->handle_erase(pos);
  }

  label->show();
  return app->instance()->exec();
}
