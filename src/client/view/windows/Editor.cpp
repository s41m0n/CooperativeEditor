#include <spdlog/spdlog.h>
#include <sstream>
#include <QGridLayout>
#include "Editor.h"

Editor::Editor(QWidget *parent) : QMainWindow(parent) {
}

void Editor::onRemoteUpdate(std::string text) {
  //RIAGGIORNARE LA VIEW CON IL NUOVO TESTO
  spdlog::debug("Un nuovo Crdt message arrivato");

}
