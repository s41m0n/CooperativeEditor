#include <map>
#include <utility>
#include <spdlog/spdlog.h>

#include "tests/server/TestControllerServer.h"
#include "tests/server/TestModelServer.h"

class Test {
public:

    std::string _name;
    std::function<std::pair<bool,std::string>()> _function;

    Test(std::string name, std::pair<bool,std::string> (*function)()) {
      _name = std::move(name);
      _function = function;
    }

};

int main(int argc, char **argv) {

  std::vector<Test> serverModelTests = {
    Test("testInit()", TestModelServer::testInit)
  };

  std::vector<Test> serverControllerTests = {
    Test("testInit()", TestControllerServer::testInit)
  };

  spdlog::info("@@@@@@@@@@@@@@@@@@@@ ServerModel functions tests @@@@@@@@@@@@@@@@@@@@");
  for(auto &test: serverModelTests) {
    auto res = test._function();
    spdlog::info("\t{} -> {}",test._name, res.first? "Passed" : "Not Passed" + res.second);
  }

  spdlog::info("@@@@@@@@@@@@@@@@@@@@ ServerController functions tests @@@@@@@@@@@@@@@@@@@@");
  for(auto &test: serverControllerTests) {
    auto res = test._function();
    spdlog::info("\t{} -> {}",test._name, res.first? "Passed" : "Not Passed" + res.second);
  }
  return 0;
}
