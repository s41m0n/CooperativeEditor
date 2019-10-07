//  controller->start();

// Created by s41m0n on 10/06/19.
//

#include <spdlog/spdlog.h>

#include "client/view/windows/LoginDialog.h"
#include "client/view/View.h"

View::View(Controller *controller) : controller(controller){
  spdlog::debug("Created View");
}

View::~View() {
  spdlog::debug("Destroyed View");
}

void View::init() {
  auto login = new LoginDialog();
  login->show();
}
