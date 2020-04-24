#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include "client/controller/Controller.h"
#include "windows/EditUserProfile.h"
#include "windows/Editor.h"
#include "windows/FileVisualizer.h"
#include "windows/Login.h"
#include "windows/SignUp.h"


/**
 * Class View, the responsible of the user interface
 */
class View : public QWidget {

  Q_OBJECT
  Q_DISABLE_COPY(View);

private:
  Controller *controller;

  Login *login{};
  SignUp *signUp{};
  Editor *editor{};
  FileVisualizer *fileVisualizer{};
  EditUserProfile *editProfile{};

public:
  explicit View(Controller *controller, QWidget *parent = nullptr);

public slots:

  /**
   * Slot to handle a file opening/closure result
   * @param result  true if ok, false otherwise
   * @param isInviteLink true if it is a response to an invite link insertion, false otherwise
   */
  void onFileResult(bool result, bool isInviteLink);

  /**
   * Slot to handle a login response
   * @param result true if ok, false otherwise
   * @param isRegisterRequest true if it is a result for a registration, false otherwise
   */
  void onLoginResponse(bool result, bool isRegisterRequest);

  /**
   * Slot called when connection established
   */
  void init();
};

#endif // COOPERATIVEEDITOR_VIEW_H
