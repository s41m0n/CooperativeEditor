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

void Editor::onFileResult(bool result) {
  if(result) {
    //Il file può essere aperto/creato
    spdlog::debug("Action OK");
  } else {
    spdlog::debug("Action KO");
  }
}

void Editor::onRemoteUpdate(std::string text) {
  //RIAGGIORNARE LA VIEW CON IL NUOVO TESTO
  spdlog::debug("Un nuovo Crdt message arrivato");

}
