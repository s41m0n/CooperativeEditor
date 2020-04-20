#include <map>
#include <utility>
#include <spdlog/spdlog.h>

#include "tests/client/TestControllerClient.h"
#include "tests/client/TestModelClient.h"

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

  std::vector<Test> clientModelTests = {
    //Test("testInit()", TestModelClient::testInit)
  };

  std::vector<Test> clientControllerTests = {
    //Test("testInit()", TestControllerClient::testInit)
  };

  spdlog::info("@@@@@@@@@@@@@@@@@@@@ ClientModel functions tests @@@@@@@@@@@@@@@@@@@@");
  for(auto &test: clientModelTests) {
    auto res = test._function();
    spdlog::info("\t{} -> {}",test._name, res.first? "Passed" : "Not Passed" + res.second);
  }

  spdlog::info("@@@@@@@@@@@@@@@@@@@@ ClientController functions tests @@@@@@@@@@@@@@@@@@@@");
  for(auto &test: clientControllerTests) {
    auto res = test._function();
    spdlog::info("\t{} -> {}",test._name, res.first? "Passed" : "Not Passed" + res.second);
  }
  return 0;
}
