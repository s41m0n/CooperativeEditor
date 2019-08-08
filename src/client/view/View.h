//
// Created by s41m0n on 10/06/19.
//

#ifndef COOPERATIVEEDITOR_VIEW_H
#define COOPERATIVEEDITOR_VIEW_H

#include <QApplication>
#include <QLabel>
#include "../controller/Controller.h"

class Controller;

class View{

private:

    Controller* controller;

    QApplication* app;
    QLabel* label;

public:

    View(Controller* controller, int argc, char** argv);
    ~View();
    int init();

};


#endif //COOPERATIVEEDITOR_VIEW_H
