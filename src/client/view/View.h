#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include "client/controller/Controller.h"
#include "windows/EditUserProfile.h"
#include "windows/Editor.h"
#include "windows/FileVisualizer.h"
#include "windows/Login.h"
#include "windows/SignUp.h"
#include <QtWidgets/QStackedWidget>

/**Class View, the responsible of the user interface
 *
 */
class View : public QWidget {

  Q_OBJECT
  Q_DISABLE_COPY(View);
private:
  Controller *controller;
  QThread *worker;

  Login *login{};
  SignUp *signUp{};
  Editor *editor{};
  FileVisualizer *fileVisualizer{};
  EditUserProfile *editProfile{};

public:
  explicit View(Controller *controller, QWidget *parent = nullptr);
  ~View() override;

public slots:
  void onFileResult(bool result);
  void onLoginResponse(bool result);
  void init();
};

#endif // COOPERATIVEEDITOR_VIEW_H
