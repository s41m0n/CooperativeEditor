#ifndef COOPERATIVEEDITOR_TESTCONTROLLERSERVER_H
#define COOPERATIVEEDITOR_TESTCONTROLLERSERVER_H

#include <string>
#include <utility>

#include <src/server/controller/Controller.h>

namespace TestControllerServer {

    std::pair<bool,std::string> testInit();
};


#endif //COOPERATIVEEDITOR_TESTCONTROLLERSERVER_H
