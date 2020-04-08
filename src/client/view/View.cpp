#include "View.h"

View::View(Controller *controller, QWidget *parent)
    : QWidget(parent), controller(controller) {

  connect(controller, &Controller::connected, this, &View::init);
  connect(controller, &Controller::error, this, [this]() {
    QMessageBox::warning(nullptr, "CooperativeEditor",
                         "Connection error. Try later, please.");
    exit(-1);
  });
}

void View::init() {

  login = new Login();
  editor = new Editor();
  fileVisualizer = new FileVisualizer();
  signUp = new SignUp();
  editProfile = new EditUserProfile();

  /// Bottone semplice da Login a SignUp se non ho un account e voglio
  /// registrarmi
  QObject::connect(login, &Login::signUp, signUp, &QMainWindow::show);

  /// Bottone semplice da SignUp a Login se non voglio registrarmi e tornare al
  /// login
  QObject::connect(signUp, &SignUp::backToLogin, login, &QMainWindow::show);

  /// Bottone semplice (azione da menu dropdown) da Editor e EditUserProfile per
  /// modificare il mio profilo
  QObject::connect(editor, &Editor::openEditProfileFromEditor, editProfile,
                   &QMainWindow::show);

  /// Segnale intermedio per farsi passare dati utente
  QObject::connect(editProfile, &EditUserProfile::requestUserProfile,
                   controller, &Controller::onShowEditProfile);

  /// Ottengo i dati utente
  QObject::connect(controller, &Controller::userProfileInfo, editProfile,
                   &EditUserProfile::onUserProfileInfo);

  /// Da EditUserProfile a Editor ma senza effettuare modifiche
  QObject::connect(editProfile,
                   &EditUserProfile::openEditorFromEditProfileNoChanges, editor,
                   &Editor::onComeBackFromEditProfileNoChanges);

  /// Bottone semplice (azione da menu dropdown) da Editor a FileVisualizer per
  /// aprire un nuovo file
  QObject::connect(editor, &Editor::openVisualizerFromEditor, fileVisualizer,
                   &QMainWindow::show);

  /// Oltre a tornare a FileVisualizer devo informare il server della chiusura
  /// del file
  QObject::connect(editor, &Editor::fileClosed, controller,
                   &Controller::onFileClosed);

  QObject::connect(editor, &Editor::generateLink, controller, &Controller::onGenerateLink);
  QObject::connect(controller, &Controller::generateLinkAnswer, editor, &Editor::onGenerateLinkAnswer);

  QObject::connect(fileVisualizer, &FileVisualizer::insertInviteLink, controller, &Controller::onInsertInviteCode);

  /// Segnale dal fileVisualizer per chiedere al server di aprire un file
  QObject::connect(fileVisualizer, &FileVisualizer::fileRequest, controller,
                   &Controller::onFileRequest);

  /// Segnale dal login al controller per inviare al server la login request
  QObject::connect(login, &Login::loginRequest, controller,
                   &Controller::onLoginRequest);

  /// Segnale dal register al controller per inviare al server la register
  /// request
  QObject::connect(signUp, &SignUp::signUpRequest, controller,
                   &Controller::onSignUpRequest);

  /// Risposta di login
  QObject::connect(controller, &Controller::loginResponse, this,
                   &View::onLoginResponse);

  /// Risposta con la lista di file ricevuti
  QObject::connect(controller, &Controller::fileListing, fileVisualizer,
                   &FileVisualizer::onFileListing);

  /// Risposta con risultato di apertura/creazione file
  QObject::connect(controller, &Controller::fileResult, this,
                   &View::onFileResult);

  /// Prima apertura del file nell'editor, setto il testo con i vari stili dei
  /// caratteri
  QObject::connect(controller, &Controller::loadFileText, editor,
                   &Editor::onFileTextLoad);

  /// Un utente remoto ha inserito un carattere nel file
  QObject::connect(controller, &Controller::remoteUserInsert, editor,
                   &Editor::onRemoteInsert);

  /// Un utente remoto ha cancellato un carattere dal file
  QObject::connect(controller, &Controller::remoteUserDelete, editor,
                   &Editor::onRemoteErase);

  /// Un utente remoto ha modificato lo stile di un carattere nel file
  QObject::connect(controller, &Controller::remoteUserUpdate, editor,
                   &Editor::onRemoteUpdate);

  /// Segnale dall'editor al controller in seguito ad un inserimento
  QObject::connect(editor, &Editor::symbolInserted, controller,
                   &Controller::onCharInserted);

  /// Segnale dall'editor al controller in seguito ad una cancellazione
  QObject::connect(editor, &Editor::symbolDeleted, controller,
                   &Controller::onCharErased);

  QObject::connect(editor, &Editor::cursorChanged, controller, &Controller::onCursorChanged);

  QObject::connect(controller, &Controller::userCursorChanged, editor, &Editor::onUserCursorChanged);

  /// Segnale dal controller all'editor per informarlo della presenza di un
  /// nuovo client collegato al file
  QObject::connect(controller, &Controller::remoteUserConnected, editor,
                   &Editor::onRemoteUserConnected);

  /// Segnale dal controller all'editor per informarlo che un client si è
  /// scollegato
  QObject::connect(controller, &Controller::remoteUserDisconnected, editor,
                   &Editor::onRemoteUserDisconnected);

  login->show();
}
void View::onLoginResponse(bool result) {
  if (result) {
    fileVisualizer->show();
    login->hide();
    signUp->hide();
  } else {
    QMessageBox::information(login, "CooperativeEditor",
                             "Username and Password are not correct.");
  }
}

void View::onFileResult(bool result) {
  if (result) {
    editor->show();
    fileVisualizer->hide();
  } else {
    auto reply = QMessageBox::question(
        fileVisualizer, "Cooperative",
        "Sorry, the action requested cannot be performed (have "
        "you inserted"
        "an already existing file name?). Retry?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
      fileVisualizer->close();
    }
  }
}
