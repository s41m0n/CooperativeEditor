#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include "client/controller/Controller.h"
#include "windows/Login.h"
#include "windows/Editor.h"
#include "windows/FileVisualizer.h"
#include "windows/EditUserProfile.h"
#include "windows/SignUp.h"

/**Class View, the responsible of the user interface
 *
 */
class View : public QObject {

Q_OBJECT

private:
    Controller *controller;

    Login *login{};
    SignUp *signUp{};
    Editor *editor{};
    FileVisualizer *fileVisualizer{};
    EditUserProfile *editProfile{};

    QMessageBox *msg;

public:
    explicit View(Controller *controller);

public slots:

    void init();

};


#endif //COOPERATIVEEDITOR_VIEW_H
