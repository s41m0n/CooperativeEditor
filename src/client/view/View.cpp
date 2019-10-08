#include <spdlog/spdlog.h>

#include "client/view/windows/Login.h"
#include "client/view/windows/Editor.h"
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
  auto editor = new Editor();
  QObject::connect(login, &Login::loginRequest, controller, &Controller::onLoginRequest);
  QObject::connect(controller, &Controller::loginResponse, login, &Login::onLoginResponse);
  QObject::connect(controller, &Controller::loginResponse, [editor](bool isLogged){
      editor->show();
  });
  login->show();

  /*
   * std::vector<std::pair<int, char>> toInsert({{0, 's'},
                                                  {1, 'i'},
                                                  {2, 'm'},
                                                  {3, 'o'}});
      std::vector<int> toDelete({});

      for (auto pair : toInsert) {
        handle_insert(pair.first, pair.second);
      }

      for (auto pos : toDelete) {
        handle_erase(pos);
      }
*/
}
