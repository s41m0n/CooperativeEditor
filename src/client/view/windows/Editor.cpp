#include <spdlog/spdlog.h>
#include <sstream>
#include "Editor.h"

Editor::Editor(QWidget *parent)  : QMainWindow(parent) {
}

void Editor::onFileListing(std::vector<std::string> files) {
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
