#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include "client/controller/Controller.h"
#include "src/client/view/struct/ResizableStackedWidget.h"
#include "windows/EditUserProfile.h"
#include "windows/Editor.h"
#include "windows/FileVisualizer.h"
#include "windows/Login.h"
#include "windows/SignUp.h"
#include <QtWidgets/QStackedWidget>

/**Class View, the responsible of the user interface
 *
 */
class View : public ResizableStackedWidget {

  Q_OBJECT

private:
  Controller *controller;
  QThread *worker;
  QMessageBox *msg;

public:
  explicit View(const std::string &host, int port, QWidget *parent = nullptr);
  ~View() override;

public slots:

  void init();
};

#endif // COOPERATIVEEDITOR_VIEW_H
