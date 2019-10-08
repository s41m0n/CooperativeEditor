#include <spdlog/spdlog.h>
#include "Editor.h"

Editor::Editor(QWidget *parent)  : QMainWindow(parent) {
}

void Editor::onFileListing(std::string files) {
  spdlog::debug(files);
}
