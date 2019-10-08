#include <spdlog/spdlog.h>

#include "client/view/windows/Login.h"
#include "client/view/windows/Editor.h"
#include "client/view/View.h"
#include "client/controller/Controller.h"

View::View(Controller *controller) :controller(controller){
  spdlog::debug("Created View");
}

View::~View() {
  spdlog::debug("Destroyed View");
}

void View::init() {
  auto login = new Login();
  auto editor = new Editor();
  QObject::connect(login, &Login::loginRequest, controller, &Controller::onLoginRequest);
  QObject::connect(controller, &Controller::loginResponse, login, &Login::onLoginResponse);
  QObject::connect(controller, &Controller::serverUnreachable, login, &Login::onServerUnreachable);
  QObject::connect(controller, &Controller::loginResponse, [editor](bool isLogged){
      if(isLogged) {
        editor->show();
      }
  });
  QObject::connect(controller, &Controller::fileListing, editor, &Editor::onFileListing);
  QObject::connect(controller, &Controller::fileResult, editor, &Editor::onFileResult);
  QObject::connect(controller, &Controller::remoteUpdate, editor, &Editor::onRemoteUpdate);

  login->show();

}
