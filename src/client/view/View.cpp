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

  //Bottone semplice da SignUp a Login se non voglio registrarmi e tornare al login
  QObject::connect(signUp, &SignUp::backToLogin, login, &Login::show);

  //Bottone semplice (azione da menu dropdown) da Editor e EditUserProfile per modificare il mio profilo
  QObject::connect(editor, &Editor::openEditProfileFromEditor, editProfile,
                   &EditUserProfile::show);

  //Bottone semplice (azione da menu dropdown) da Editor a FileVisualizer per aprire un nuovo file
  QObject::connect(editor, &Editor::openVisualizerFromEditor, fileVisualizer,
                   &FileVisualizer::show);

  //Segnale dal fileVisualizer per chiedere al server di aprire un file
  QObject::connect(fileVisualizer, &FileVisualizer::fileRequest, controller,
                   &Controller::onFileRequest);

  //Segnale dal login al controller per inviare al server la login request
  QObject::connect(login, &Login::loginRequest, controller,
                   &Controller::onLoginRequest);

  //Segnale dal controller al login per notificare l'esito della login request
  QObject::connect(controller, &Controller::loginResponse, login,
                   &Login::onLoginResponse);

  //Server unreachable nelle varie finestre
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

  QObject::connect(controller, &Controller::fileResult,
                   [editor](bool result) {
                       if (result) {
                         editor->show();
                       }
                   });

  QObject::connect(controller, &Controller::remoteUpdate, editor,
                   &Editor::onRemoteUpdate);

  QObject::connect(signUp, &SignUp::signUpRequest, controller,
                   &Controller::onSignUpRequest);

  QObject::connect(editor, &Editor::symbolInserted, controller, &Controller::onCharInserted);
  QObject::connect(editor, &Editor::symbolDeleted, controller, &Controller::onCharErased);

  login->show();
  //editor->show();
}
