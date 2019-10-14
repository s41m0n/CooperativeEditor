#include "client/view/View.h"

View::View(Controller *controller) : controller(controller) {
}

void View::init() {
  auto login = new Login();
  auto editor = new Editor();
  auto fileVisualizer = new FileVisualizer();
  auto signUp = new SignUp();
  auto editProfile = new EditUserProfile();

  //TODO: valutare ipotesi di creare le varie finestre solo quando sono necessarie, comporta creare come variabili di classe le varie finestre

  //Bottone semplice da Login a SignUp se non ho un account e voglio registrarmi
  QObject::connect(login, &Login::signUp, signUp, &SignUp::show);

  //Bottone semplice da SignUp a Login se non voglio registrarmi
  QObject::connect(signUp, &SignUp::backToLogin, login, &Login::show);

  //Bottone semplice (azione da menu dropdown) da Editor e EditUserProfile per modificare il mio profile
  QObject::connect(editor, &Editor::openEditProfileFromEditor, editProfile,
                   &EditUserProfile::show);

  //TODO: Simo guarda qua che queste connect le hai scritte tu e non vorrei aver scritto cazzate nei commenti

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

}
