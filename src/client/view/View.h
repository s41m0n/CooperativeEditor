//
// Created by s41m0n on 10/06/19.
//

#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include <QLabel>
#include <QMainWindow>

//Class forward declaration
class Controller;

/**Class View, the responsible of the user interface
 *
 * @author Simone Magnani - s41m0n
 */
class View {

private:
    ///A reference to the Controller
    Controller *controller;

    ///The main window
    QMainWindow mainWindow;

    ///Test Label
    QLabel *label;

public:
    ///Constructor which also assigns the controller
    explicit View(Controller *controller);

    ///Destructor, used for debug
    ~View();

    ///Method to initialize the user interface
    void init();

};


#endif //COOPERATIVEEDITOR_VIEW_H
