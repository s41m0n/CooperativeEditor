#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include "client/controller/Controller.h"
#include "client/view/windows/Login.h"
#include "client/view/windows/Editor.h"
#include "client/view/windows/FileVisualizer.h"
#include "client/view/windows/EditUserProfile.h"
#include "client/view/windows/SignUp.h"

/**Class View, the responsible of the user interface
 *
 */
class View {

private:
    Controller *controller;

public:
    explicit View(Controller *controller);

    void init();

};


#endif //COOPERATIVEEDITOR_VIEW_H
