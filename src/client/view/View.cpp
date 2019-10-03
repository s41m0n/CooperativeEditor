//  controller->start();

// Created by s41m0n on 10/06/19.
//

#include <spdlog/spdlog.h>

#include "client/view/View.h"

View::View(Controller *controller) : controller(controller),
                                     label(new QLabel(&mainWindow)) {
  spdlog::debug("Created View");
}

View::~View() {
  spdlog::debug("Destroyed View");
}

void View::init() {
  label->setText("HELLO QT");
  mainWindow.show();
}
