#include "TestControllerServer.h"

std::pair<bool, std::string> TestControllerServer::testInit() {
  Controller c(nullptr, 3000, nullptr);
  return {true, ""};
}
