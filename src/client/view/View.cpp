#include "View.h"

View::View(Controller *controller) : controller(controller) {
  msg = new QMessageBox();

  QObject::connect(controller, &QAbstractSocket::connected, this,
                   &View::init);

  QObject::connect(controller, QOverload<QAbstractSocket::SocketError>::of(
          &QAbstractSocket::error), this, [this]() {
      msg->setText(
              "Connection error, the server is unreachable. Try later, please.");
      msg->exec();
      exit(-1);
  });
}

//TODO: aggiungi metodo che raccolga errori lanciati dalla GUI e faccia vedere il msg d'errore come sopra

void View::init() {
  login = new Login();
  editor = new Editor();
  fileVisualizer = new FileVisualizer();
  signUp = new SignUp();
  editProfile = new EditUserProfile();

  ///Bottone semplice da Login a SignUp se non ho un account e voglio registrarmi
  QObject::connect(login, &Login::signUp, signUp, &QMainWindow::show);

  ///Bottone semplice da SignUp a Login se non voglio registrarmi e tornare al login
  QObject::connect(signUp, &SignUp::backToLogin, login, &QMainWindow::show);

  ///Bottone semplice (azione da menu dropdown) da Editor e EditUserProfile per modificare il mio profilo
  QObject::connect(editor, &Editor::openEditProfileFromEditor, editProfile, &QMainWindow::show);

  ///Bottone semplice (azione da menu dropdown) da Editor a FileVisualizer per aprire un nuovo file
  QObject::connect(editor, &Editor::openVisualizerFromEditor, fileVisualizer, &QMainWindow::show);

  ///Segnale dal fileVisualizer per chiedere al server di aprire un file
  QObject::connect(fileVisualizer, &FileVisualizer::fileRequest, controller,
                   &Controller::onFileRequest);

  ///Segnale dal login al controller per inviare al server la login request
  QObject::connect(login, &Login::loginRequest, controller,
                   &Controller::onLoginRequest);

  ///Segnale dal register al controller per inviare al server la register request
  QObject::connect(signUp, &SignUp::signUpRequest, controller,
                   &Controller::onSignUpRequest);

  ///Risposta di login
  QObject::connect(controller, &Controller::loginResponse,
                   [this](bool isLogged) {
                       if (isLogged) {
                           fileVisualizer->show();
                           login->hide();
                           signUp->hide();
                       } else {
                         msg->setText(
                                 "Username and Password are not correct.");
                         msg->exec();
                       }
                   });

  ///Risposta con la lista di file ricevuti
  QObject::connect(controller, &Controller::fileListing, fileVisualizer,
                   &FileVisualizer::onFileListing);

  ///Risposta con risultato di apertura/creazione file
  QObject::connect(controller, &Controller::fileResult, this,
                   [this](bool result) {
                       if (result) {
                         editor->show();
                         fileVisualizer->hide();
                       } else {
                         msg->setText(
                                 "Sorry, the action requested cannot be performed (have you inserted"
                                 "an already existing file name?). Retry?");
                         msg->setStandardButtons(
                                 QMessageBox::Yes | QMessageBox::No);

                         int dialogResult = msg->exec();

                         switch (dialogResult) {
                           case QMessageBox::Yes:
                             msg->close();
                             break;
                           case QMessageBox::No:
                             fileVisualizer->hide();
                             break;
                           default:
                             //error, should never be reached
                             break;
                         }
                       }
                   });

  ///Nuovo testo da settare alla view in seguito ad un remote update
  QObject::connect(controller, &Controller::remoteUpdate, editor,
                   &Editor::onRemoteUpdate);

  ///Segnale dall'editor al controller in seguito ad un inserimento
  QObject::connect(editor, &Editor::symbolInserted, controller,
                   &Controller::onCharInserted);

  ///Segnale dall'editor al controller in seguito ad una cancellazione
  QObject::connect(editor, &Editor::symbolDeleted, controller,
                   &Controller::onCharErased);

  login->show();
}
