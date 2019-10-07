//  controller->start();

// Created by s41m0n on 10/06/19.
//

#include <spdlog/spdlog.h>

#include "client/view/windows/Login.h"
#include "client/view/View.h"
#include "client/controller/Controller.h"

View::View(Model *model, Controller *controller) : model(model), controller(controller){
  spdlog::debug("Created View");
}

View::~View() {
  spdlog::debug("Destroyed View");
}

void View::init() {
  auto login = new Login();
  QObject::connect(login, &Login::loginRequest, controller, &Controller::onLoginRequest);
  QObject::connect(controller, &Controller::loginResponse, login, &Login::onLoginResponse);
  login->show();
}
