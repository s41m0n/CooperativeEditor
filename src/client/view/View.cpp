#include <spdlog/spdlog.h>

#include "client/view/windows/Login.h"
#include "client/view/windows/Editor.h"
#include "client/view/windows/FileVisualizer.h"
#include "client/view/View.h"
#include "client/controller/Controller.h"

View::View(Controller *controller) : controller(controller) {
}

void View::init() {
  auto login = new Login();
  auto editor = new Editor();
  auto fileVisualizer = new FileVisualizer();

  QObject::connect(login, &Login::loginRequest, controller,
                   &Controller::onLoginRequest);
  QObject::connect(controller, &Controller::loginResponse, login,
                   &Login::onLoginResponse);
  QObject::connect(controller, &Controller::serverUnreachable, login,
                   &Login::onServerUnreachable);
  QObject::connect(controller, &Controller::loginResponse,
                   [fileVisualizer](bool isLogged) {
                       if (isLogged) {
                         fileVisualizer->show();
                       }
                   });
  QObject::connect(controller, &Controller::fileListing, fileVisualizer,
                   &FileVisualizer::onFileListing);
  QObject::connect(controller, &Controller::fileResult, fileVisualizer,
                   &FileVisualizer::onFileResult);
  QObject::connect(controller, &Controller::remoteUpdate, editor,
                   &Editor::onRemoteUpdate);

  login->show();
  //editor->show();

}
