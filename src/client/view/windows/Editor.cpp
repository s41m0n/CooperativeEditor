#include <spdlog/spdlog.h>
#include "Editor.h"

Editor::Editor(QWidget *parent)  : QMainWindow(parent) {
}

void Editor::onFileListing(std::string files) {
  //ricevo lista di file separati da ; e li mostro al client
}
