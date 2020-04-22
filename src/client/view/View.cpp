#include "View.h"

View::View(Controller *controller, QWidget *parent)
    : QWidget(parent), controller(controller) {

  connect(controller, &Controller::connected, this, &View::init);
  connect(controller, &Controller::error, this, [this](const QString& what) {
    QMessageBox::warning(nullptr, "CooperativeEditor",
                         "Error: " + what);
    exit(-1);
  });
}

void View::init() {

  login = new Login();
  editor = new Editor();
  fileVisualizer = new FileVisualizer();
  signUp = new SignUp();
  editProfile = new EditUserProfile();

  /*From login to signup*/
  QObject::connect(login, &Login::signUp, signUp, &QMainWindow::show);

  /*From signup to login*/
  QObject::connect(signUp, &SignUp::backToLogin, login, &QMainWindow::show);

  /*From editor to user profile*/
  QObject::connect(editor, &Editor::openEditProfileFromEditor, editProfile,
                   &QMainWindow::show);

  /*Notify controller to get user data*/
  QObject::connect(editProfile, &EditUserProfile::requestUserProfile,
                   controller, &Controller::onShowEditProfile);

  /*Notify controller that user wants update its data*/
  QObject::connect(editProfile, &EditUserProfile::updateRequest, controller, &Controller::onUpdateRequest);

  /*Notify view editProfile about result of update request*/
  QObject::connect(controller, &Controller::updateResponse, editProfile, &EditUserProfile::onUpdateResponse);

  /*Controller answers with user data*/
  QObject::connect(controller, &Controller::userProfileInfo, editProfile,
                   &EditUserProfile::onUserProfileInfo);

  /*From user profile to editor*/
  QObject::connect(editProfile,
                   &EditUserProfile::openEditorFromEditProfileNoChanges, editor,
                   &Editor::onComeBackFromEditProfileNoChanges);

  /*From editor to file visualizer*/
  QObject::connect(editor, &Editor::openVisualizerFromEditor, fileVisualizer,
                   &QMainWindow::show);

  /*Editor closes file*/
  QObject::connect(editor, &Editor::fileClosed, controller,
                   &Controller::onFileClosed);

  /*Editor notify controller to generate link*/
  QObject::connect(editor, &Editor::generateLink, controller,
                   &Controller::onGenerateLink);

  /*Controller notify editor of generate link answer*/
  QObject::connect(controller, &Controller::generateLinkAnswer, editor,
                   &Editor::onGenerateLinkAnswer);

  /*Notify view the user wants to edit its profile from FileSelection*/
  QObject::connect(fileVisualizer, &FileVisualizer::openEditProfile, editProfile, &QMainWindow::show);

  /*File visualizer notify controller of invite link insertion*/
  QObject::connect(fileVisualizer, &FileVisualizer::insertInviteLink,
                   controller, &Controller::onInsertInviteCode);

  /*From file visualizer to require file list*/
  QObject::connect(fileVisualizer, &FileVisualizer::requestFileList, controller, &Controller::onRequestFileList);

  /*File visualizer request a file to controller*/
  QObject::connect(fileVisualizer, &FileVisualizer::fileRequest, controller,
                   &Controller::onFileRequest);

  /*Login notify login attempt*/
  QObject::connect(login, &Login::loginRequest, controller,
                   &Controller::onLoginRequest);

  /*Signup notify signup attempt*/
  QObject::connect(signUp, &SignUp::signUpRequest, controller,
                   &Controller::onSignUpRequest);

  /*Controller notify login response*/
  QObject::connect(controller, &Controller::loginResponse, this,
                   &View::onLoginResponse);

  /*Controller notify file listing*/
  QObject::connect(controller, &Controller::fileListing, fileVisualizer,
                   &FileVisualizer::onFileListing);

  /*Controller notify file opening/closure result*/
  QObject::connect(controller, &Controller::fileResult, this,
                   &View::onFileResult);

  /*Controller notify file text load*/
  QObject::connect(controller, &Controller::loadFileText, editor,
                   &Editor::onFileTextLoad);

  /*Controller notify remote user insert*/
  QObject::connect(controller, &Controller::remoteUserInsert, editor,
                   &Editor::onRemoteInsert);

  /*Controller notify remote user delete*/
  QObject::connect(controller, &Controller::remoteUserDelete, editor,
                   &Editor::onRemoteErase);

  /*Editor notify local insert*/
  QObject::connect(editor, &Editor::symbolInserted, controller,
                   &Controller::onCharInserted);

  /*Editor notify local erase*/
  QObject::connect(editor, &Editor::symbolDeleted, controller,
                   &Controller::onCharErased);

  /*Editor notify local cursor position changed*/
  QObject::connect(editor, &Editor::cursorChanged, controller,
                   &Controller::onCursorChanged);

  /*Controller notify remote cursor position changed*/
  QObject::connect(controller, &Controller::userCursorChanged, editor,
                   &Editor::onUserCursorChanged);

  /*Controller notify remote user connected*/
  QObject::connect(controller, &Controller::remoteUserConnected, editor,
                   &Editor::onRemoteUserConnected);

  /*Controller notify remote user disconnected*/
  QObject::connect(controller, &Controller::remoteUserDisconnected, editor,
                   &Editor::onRemoteUserDisconnected);

  /*Editor asks the controller the text inserted by a user */
  QObject::connect(editor, &Editor::getUserText, controller,
                   &Controller::onUserTextAsked);

  /*Controller sends to the view the text inserted by a user */
  QObject::connect(controller, &Controller::sendUserText, editor,
                   &Editor::onUserTextReceived);

  /*Editor asks the controller the original text inserted by a user */
  QObject::connect(editor, &Editor::getUserTextOriginal, controller,
                   &Controller::onUserOriginalTextAsked);

  /*Controller sends to the view the original text inserted by a user */
  QObject::connect(controller, &Controller::sendUserOriginalText, editor,
                   &Editor::onUserOriginalTextReceived);

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
