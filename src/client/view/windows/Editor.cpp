#include <spdlog/spdlog.h>
#include <sstream>
#include "Editor.h"

Editor::Editor(QWidget *parent)  : QMainWindow(parent) {
}

void Editor::onFileListing(std::string files) {

  //parsing of files string
  std::replace(files.begin(), files.end(), ';', ' ');
  std::vector<std::string> filesArray;
  std::istringstream iss(files);
  for(std::string s; iss >> s; ) {
    filesArray.push_back(s);
  }

  //Ora devo creare un interfaccia per far scegliere all'utente i file


}
